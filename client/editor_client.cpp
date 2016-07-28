#include "editor_client.h"
#include <stdlib.h>

#include <QString>
#include <iostream>

using namespace std;

QDataStream &operator<<(QDataStream &out, const Transform &transform)
{
    out << transform.pos << transform.c << transform.priority << transform.time_stamp[0] << transform.time_stamp[1];
    return out;
}

QDataStream &operator>>(QDataStream &in, Transform &transform)
{
    in >> transform.pos >> transform.c >> transform.priority >> transform.time_stamp[0] >> transform.time_stamp[1];
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
    new_transform.time_stamp[0] = time_stamps[0];
    new_transform.time_stamp[1] = time_stamps[1];
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

    if (tcpSocket->bytesAvailable() < 17) {
        return;
    }

    QByteArray block = tcpSocket->read(17);
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream >> transform;

    // Verifica que el elemento que entro sea valido
    if(transform.time_stamp[0] - time_stamps[1] != 1){
        // Si la transformacion no es la siguiente que debe ser aplicada busca
        // en la lista por la existencia de esta
        //transform_tmp = buscaEnLista(lista_transformaciones, transform);
        // Agrega la transformacion actual a la lista
        lista_transformaciones.push_front(transform);
        return;

        // Si no encuentra el elemento en la lista regresa
        //if(transform_tmp.priority == -1){
         //   return;
        //}

        //transform = transform_tmp;
    }//else{
    //    transform_tmp = transform;
    //}
    //transform_tmp = buscaEnLista(lista_transformaciones, transform);

    cout << "------------------------------------------" << endl;
    cout << "Respuesta del servidor:"<< endl;
    cout << "Posicion: " ;
    cout << transform.pos << endl;
    cout << "Caracter: " ;
    cout << transform.c << endl;
    cout << "Prioridad: " << transform.priority << endl;
    cout << "time_stamp: " << transform.time_stamp[0] << ", " << transform.time_stamp[1] << endl;

    do{
        //cout << "*****Caracter: " << caracter << endl;
        cout << "lista_local size: " << lista_local.size() << endl;
        cout << "time_stamp locales: " << time_stamps[0] << ", " << time_stamps[1] << endl;
        cout << "------------------------------------------" << endl;
        // Se necesita comparar que el times_stamp[0] sea igual al local del otro cliente

        if(transform.time_stamp[1] != time_stamps[0] && lista_local.size() > 0){
            transform_tmp = lista_local.front();
            transform = operat_transformation(transform, transform_tmp);
        }

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

        //lista_local.pop_front();

        transform = buscaEnLista(lista_transformaciones, time_stamps[0]);

    }while(transform.priority != -1);

    //cout << "time_stamp recibido: " << transform.time_stamp << endl;
    //cout << "time_stamps: " << time_stamps[0] << ", " << time_stamps[1] << endl;


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

Transform EditorCliente::buscaEnLista(std::list<Transform> lista, int time_stamp){
    //cout << "Elementos en la lista" << endl;
    Transform new_transform;
    new_transform.priority = -1;

    for (std::list<Transform>::iterator it=lista.begin(); it != lista.end(); ++it){
        //cout << (*it).c << " ";
        if((*it).time_stamp[1] == time_stamp){
            new_transform = *it;
            lista.erase(it);
            break;
        }
    }

    return new_transform;
}