#include "editor_client.h"
#include <stdlib.h>

#include <QString>
#include <iostream>

using namespace std;

QDataStream &operator<<(QDataStream &out, const Transform &transform)
{
    out << transform.pos << transform.c << transform.priority << transform.time_stamp;
    return out;
}

QDataStream &operator>>(QDataStream &in, Transform &transform)
{
    in >> transform.pos >> transform.c >> transform.priority >> transform.time_stamp;
    return in;
}

EditorCliente::EditorCliente(QWidget *parent)
    : QWidget(parent)
{
    writing_to_box = false;
    time_stamps[0] = 0;
    time_stamps[1] = 0;

    connect(&m_textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(&m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    connect(&sock, SIGNAL(readyRead()), this, SLOT(m_read()));

    m_cursor = m_textEdit.textCursor();

    QStringList args = QCoreApplication::arguments();

    if (args.count() == 1) {
        m_label.setText(QString("Cliente 1"));
        start("127.0.0.1", 2347);
    } else {
        m_label.setText(QString("Cliente 2"));
        start(args.at(1), 2348);
        //start("127.0.0.1", 2346);
    }

    m_layout.addWidget(&m_textEdit);
    m_layout.addWidget(&m_label);

    setLayout(&m_layout);
}

EditorCliente::~EditorCliente(){
  sock.close();
}

void EditorCliente::onTextChanged(){
    if(writing_to_box)
        return;

    Transform new_transform;

    QByteArray block;
    QDataStream sendStream(&block, QIODevice::ReadWrite);

    t = m_textEdit.toPlainText();
    time_stamps[0]++;

    new_transform.pos = m_textEdit.textCursor().positionInBlock() - 1;
    new_transform.c = t[new_transform.pos].toLatin1();
    new_transform.priority = 0;
    new_transform.time_stamp = time_stamps[0];
    lista_local.push_front(new_transform);

    sendStream << new_transform;
    sock.write(block);

    cout << "Paquete enviado " << endl;
    cout << "Posicion: " ;
    cout << new_transform.pos << endl;
    cout << "Caracter: " ;
    cout << new_transform.c << endl;
    cout << "Prioridad: " << new_transform.priority << endl;
    //cout << "time_stamps: " << new_transform.time_stamp << ", " << time_stamps[1] << endl;
}

void EditorCliente::onCursorPositionChanged(){
}

void EditorCliente::start(QString address, quint16 port)
{
  QHostAddress addr(address);
  sock.connectToHost(addr, port);
}

void EditorCliente::m_read() {
    Transform transform;
    Transform transform_tmp;
    QTcpSocket *tcpSocket = (QTcpSocket*)sender();

    if (tcpSocket->bytesAvailable() < 13) {
        return;
    }

    QByteArray block = tcpSocket->read(13);
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream >> transform;
/*
    if(transform.time_stamp - time_stamps[1] == 1){
        cout << "La Transformacion esta permitida" << endl;

    }//else{*/
    lista_transformaciones.push_front(transform);
    transform_tmp = buscaEnLista(lista_transformaciones, transform);

    // Si no encuetra el elemento la funcion de busqueda
    // retorna una estructura con priority = -1
    if(transform_tmp.priority != -1 && transform_tmp.time_stamp - time_stamps[1] == 1){
        cout << "La Transformacion esta permitida" << endl;
        transform = transform_tmp;
    }else{
        cout << "La transformacion no esta permitida" << endl;
        return;
    }

    cout << "Respuesta del servidor:"<< endl;
    cout << "Posicion: " ;
    cout << transform.pos << endl;
    cout << "Caracter: " ;
    cout << transform.c << endl;
    cout << "Prioridad: " << transform.priority << endl;
    cout << "time_stamp: " << transform.time_stamp << endl;

    //cout << "*****Caracter: " << caracter << endl;
    t = t.insert(transform.pos, transform.c);

    QTextCursor tmp_cursor = m_textEdit.textCursor();
    int cur_position;
    if (transform.pos < m_textEdit.textCursor().position())
        cur_position = m_textEdit.textCursor().position() + 1;
    else
        cur_position = m_textEdit.textCursor().position();

    writing_to_box = true;
    m_textEdit.setText(t);
    writing_to_box = false;

    tmp_cursor.setPosition(cur_position);
    m_textEdit.setTextCursor(tmp_cursor);
    time_stamps[1]++;
    cout << "time_stamp recibido: " << transform.time_stamp << endl;
    cout << "time_stamps: " << time_stamps[0] << ", " << time_stamps[1] << endl;


}

Transform EditorCliente::operat_transformation (Transform t1, Transform t2){
    cout << "En Transformacion" << endl;
    Transform res;
    res.c = t1.c;
    res.priority = t1.priority;
    if (t1.pos < t2.pos ||
            (t1.pos==t2.pos && t1.c!=t2.c && t1.priority<t2.priority)) {
        res.pos = t1.pos;
    } else if (t1.pos > t2.pos ||
            (t1.pos==t2.pos && t1.c!=t2.c && t1.priority>t2.priority)) {
        res.pos = t1.pos+1;
    } else {
        res.priority = -1;
    }
    return res;
}

Transform EditorCliente::buscaEnLista(std::list<Transform> lista, Transform transform){
    //cout << "Elementos en la lista" << endl;
    Transform new_transform;
    new_transform.priority = -1;

    for (std::list<Transform>::iterator it=lista.begin(); it != lista.end(); ++it){
        //cout << (*it).c << " ";
        if((*it).time_stamp == transform.time_stamp){
            new_transform = *it;
            lista.erase(it);
            break;
        }
    }

    return new_transform;
}