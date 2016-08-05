#include "editor_client.h"
#include <stdlib.h>

#include <QString>
#include <QKeyEvent>
#include <iostream>

using namespace std;

QDataStream &operator<<(QDataStream &out, const Operation &operation)
{
    out << operation.pos << operation.c << operation.priority << operation.time_stamp[0] << operation.time_stamp[1];
    return out;
}

QDataStream &operator>>(QDataStream &in, Operation &operation)
{
    in >> operation.pos >> operation.c >> operation.priority >> operation.time_stamp[0] >> operation.time_stamp[1];
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

    m_cursor = m_textEdit.textCursor();

    QStringList args = QCoreApplication::arguments();

    if (args.count() == 1) {
        id_cliente = 1;
        connect(&server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
        server.listen(QHostAddress::Any, 2347);
        m_label.setText(QString("Cliente 1"));
    } else {
        id_cliente = 2;
        m_label.setText(QString("Cliente 2"));
        QHostAddress addr(args.at(1));
        sock = new QTcpSocket;
        sock->connectToHost(addr, 2347);
        connect(sock, SIGNAL(readyRead()), this, SLOT(m_read()));
    }

    m_layout.addWidget(&m_textEdit);
    m_layout.addWidget(&m_label);

    setLayout(&m_layout);
}

void EditorCliente::acceptConnection()
{
  sock = server.nextPendingConnection();
  quint16 port = sock->localPort();
  cout << "Puerto: ";
  cout << port << endl;
  connect(sock, SIGNAL(readyRead()), this, SLOT(m_read()));
}

EditorCliente::~EditorCliente(){
  sock->close();
  server.close();
}

void EditorCliente::onTextChanged(){
    if(writing_to_box)
        return;

    Operation new_operation;

    QByteArray block;
    QDataStream sendStream(&block, QIODevice::ReadWrite);

    t = m_textEdit.toPlainText();
    time_stamps[0]++;

    new_operation.pos = m_textEdit.textCursor().positionInBlock() - 1;
    new_operation.c = t[new_operation.pos].toLatin1();
    new_operation.priority = id_cliente;
    new_operation.time_stamp[0] = time_stamps[0];
    new_operation.time_stamp[1] = time_stamps[1];
    lista_local.push_front(new_operation);

    sendStream << new_operation;
    sock->write(block);

    cout << "Paquete enviado " << endl;
    cout << "Posicion: " ;
    cout << new_operation.pos << endl;
    cout << "Caracter: " ;
    cout << new_operation.c << endl;
    cout << "Prioridad: " << new_operation.priority << endl;
    //cout << "time_stamps: " << new_operation.time_stamp << ", " << time_stamps[1] << endl;
}

void EditorCliente::onCursorPositionChanged(){
}

void EditorCliente::m_read() {
    Operation operation;
    Operation operation_tmp;
    QTcpSocket *tcpSocket = (QTcpSocket*)sender();

    if (tcpSocket->bytesAvailable() < 17) {
        return;
    }

    QByteArray block = tcpSocket->read(17);
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream >> operation;

    // Verifica que el elemento que entro sea valido
    if(operation.time_stamp[0] - time_stamps[1] != 1){
        // Si la operationacion no es la siguiente que debe ser aplicada busca
        // en la lista por la existencia de esta
        //operation_tmp = buscaEnLista(lista_operaciones, operation);
        // Agrega la operationacion actual a la lista
        lista_operaciones.push_front(operation);
        return;

        // Si no encuentra el elemento en la lista regresa
        //if(operation_tmp.priority == -1){
         //   return;
        //}

        //operation = operation_tmp;
    }//else{
    //    operation_tmp = operation;
    //}
    //operation_tmp = buscaEnLista(lista_operaciones, operation);

    cout << "------------------------------------------" << endl;
    cout << "Respuesta del servidor:"<< endl;
    cout << "Posicion: " ;
    cout << operation.pos << endl;
    cout << "Caracter: " ;
    cout << operation.c << endl;
    cout << "Prioridad: " << operation.priority << endl;
    cout << "time_stamp: " << operation.time_stamp[0] << ", " << operation.time_stamp[1] << endl;

    do{
        //cout << "*****Caracter: " << caracter << endl;
        cout << "lista_local size: " << lista_local.size() << endl;
        cout << "time_stamp locales: " << time_stamps[0] << ", " << time_stamps[1] << endl;
        cout << "------------------------------------------" << endl;
        // Se necesita comparar que el times_stamp[0] sea igual al local del otro cliente

        if(operation.time_stamp[1] != time_stamps[0] && lista_local.size() > 0){
            operation_tmp = lista_local.front();
            operation = operat_transformation(operation, operation_tmp);
        }

        if (operation.priority == -1) {
            return;
        }

        t = t.insert(operation.pos, operation.c);

        QTextCursor tmp_cursor = m_textEdit.textCursor();
        int cur_position;
        if (operation.pos < m_textEdit.textCursor().position())
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

        operation = buscaEnLista(lista_operaciones, time_stamps[0]);

    }while(operation.priority != -1);

    //cout << "time_stamp recibido: " << operation.time_stamp << endl;
    //cout << "time_stamps: " << time_stamps[0] << ", " << time_stamps[1] << endl;


}

Operation EditorCliente::buscaEnLista(std::list<Operation> lista, int time_stamp){
    //cout << "Elementos en la lista" << endl;
    Operation new_operation;
    new_operation.priority = -1;

    for (std::list<Operation>::iterator it=lista.begin(); it != lista.end(); ++it){
        //cout << (*it).c << " ";
        if((*it).time_stamp[1] == time_stamp){
            new_operation = *it;
            lista.erase(it);
            break;
        }
    }

    return new_operation;
}

void EditorCliente::keyReleaseEvent(QKeyEvent *event){

    // Backspace: 16777219
    // Suprimir: 16777223
    if(event->key() == 16777219)
        cout << "Se presiono Backspace" << endl;
    else if(event->key() == 16777223)
        cout << "Se presiono suprimir" << endl;

    //cout << "Tecla soltada: " << event->key() << endl;
}

Operation EditorCliente::operat_transformation(Operation o1, Operation o2){
    cout << "En Operationacion" << endl;
    Operation res;
    res.c = o1.c;
    res.priority = o1.priority;
    if (o1.pos < o2.pos ||
            (o1.pos==o2.pos && o1.c!=o2.c && o1.priority<o2.priority)) {
        res.pos = o1.pos;
    } else if (o1.pos > o2.pos ||
            (o1.pos==o2.pos && o1.c!=o2.c && o1.priority>o2.priority)) {
        res.pos = o1.pos+1;
    } else {
        res.priority = -1;
    }
    return res;
}
