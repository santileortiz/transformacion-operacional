#include "editor_client.h"
#include <stdlib.h>

#include <QString>
#include <QKeyEvent>
#include <iostream>

using namespace std;

QDataStream &operator<<(QDataStream &out, const Operation &operation)
{
    out << operation.pos << operation.c << operation.priority << operation.time_stamp[0] << operation.time_stamp[1] << operation.type;
    return out;
}

QDataStream &operator>>(QDataStream &in, Operation &operation)
{
    in >> operation.pos >> operation.c >> operation.priority >> operation.time_stamp[0] >> operation.time_stamp[1] >> operation.type;
    return in;
}

EditorCliente::EditorCliente(QWidget *parent)
    : QWidget(parent)
{
    time_stamps[0] = 0;
    time_stamps[1] = 0;

    connect(&m_textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(&m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));

    m_cursor = m_textEdit.textCursor();
    ignored_msgs = 0;

    QStringList args = QCoreApplication::arguments();

    setFocusPolicy(Qt::StrongFocus);

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
    if(writing_to_box) {
        return;
    }

    if(m_textEdit.is_backspace) {
        send_operation (2);
    } else {
        send_operation(1);
    }
}

void EditorCliente::onCursorPositionChanged(){
}

void print_operation (Operation op) {
    if (op.type == 1) {
        cout << "Tipo: ins" << endl;
    } else {
        cout << "Tipo: del" << endl;
    }
    cout << "Posicion: ";
    cout << op.pos << endl;
    cout << "Caracter: " ;
    cout << op.c << endl;
    cout << "Prioridad: " << op.priority << endl;
    cout << "time_stamp: " << op.time_stamp[0] << ", " << op.time_stamp[1] << endl;
    cout << "------------------------------------------" << endl;
}

void EditorCliente::apply_operation (Operation operation) {
    if (operation.priority == -1) {
        return;
    }

    qint8 cur_correct;
    if (operation.type == 1) {
        t = t.insert(operation.pos, operation.c);
        cur_correct = 1;
    } else {
        t = t.remove(operation.pos, 1);
        cur_correct = -1;
    }

    QTextCursor tmp_cursor = m_textEdit.textCursor();
    int cur_position;
    if (operation.pos < m_textEdit.textCursor().position())
        cur_position = m_textEdit.textCursor().position() + cur_correct;
    else
        cur_position = m_textEdit.textCursor().position();

    writing_to_box = true;
    m_textEdit.setText(t);
    writing_to_box = false;

    tmp_cursor.setPosition(cur_position);
    m_textEdit.setTextCursor(tmp_cursor);
}

Operation read_operation (QTcpSocket *sender) {
    Operation operation;

    if (sender->bytesAvailable() < 18) {
        operation.priority = -1;
        return operation;
    }

    QByteArray block = sender->read(18);
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream >> operation;
    return operation;
}

void EditorCliente::m_read() {
    Operation operation;
    if(m_textEdit.ignore_incoming) {
        ignored_msgs++;
        return;
    }

    while(ignored_msgs>=0) {
        operation = read_operation((QTcpSocket*)sender());
        cout << "Operacion recibida:" << endl;
        print_operation (operation);

        while (lista_local.size()!=0 && lista_local.front().time_stamp[0] <= operation.time_stamp[1]) {
            lista_local.pop_front();
        }

        std::list<Operation>::iterator it=lista_local.begin();
        while (it!=lista_local.end() && (*it).time_stamp[0] > operation.time_stamp[1]) {
            cout << "lista_local size: " << lista_local.size() << endl;
            cout << "time_stamp locales: " << time_stamps[0] << ", " << time_stamps[1] << endl;
            cout << "------------------------------------------" << endl;

            operation = operat_transformation(operation, *it);
            *it = operat_transformation(*it, operation);
            ++it;
        }

        apply_operation(operation);
        time_stamps[1]++;

        if (ignored_msgs==0)
            break;
        ignored_msgs--;
    }
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

void EditorCliente::send_operation (quint8 type) {
    Operation new_operation;

    QByteArray block;
    QDataStream sendStream(&block, QIODevice::ReadWrite);

    t = m_textEdit.toPlainText();
    time_stamps[0]++;

    if (type == 1) {
        new_operation.pos = m_textEdit.textCursor().positionInBlock() - 1;
    } else {
        new_operation.pos = m_textEdit.textCursor().positionInBlock();
    }
    new_operation.c = t[new_operation.pos].toLatin1();
    new_operation.priority = id_cliente;
    new_operation.time_stamp[0] = time_stamps[0];
    new_operation.time_stamp[1] = time_stamps[1];
    new_operation.type = type;
    lista_local.push_back(new_operation);

    sendStream << new_operation;
    sock->write(block);

    cout << "Operacion enviada:" << endl;
    print_operation (new_operation);
}

Operation EditorCliente::operat_transformation(Operation o1, Operation o2){
    cout << "Transformando o1:" << endl;
    print_operation (o1);
    cout << "Transformando o2:" << endl;
    print_operation (o2);
    Operation res;
    res.c = o1.c;
    res.priority = o1.priority;
    res.type = o1.type;
    res.time_stamp[0] = o1.time_stamp[0];
    res.time_stamp[1] = o1.time_stamp[1];
    if (o1.pos < o2.pos ||
            (o1.pos==o2.pos && o1.c!=o2.c && o1.priority<o2.priority)) {
        res.pos = o1.pos;
    } else if (o1.pos > o2.pos ||
            (o1.pos==o2.pos && o1.c!=o2.c && o1.priority>o2.priority)) {
        res.pos = o1.pos+1;
    } else {
        res.priority = -1;
    }
    cout << "En:" << endl;
    print_operation (res);
    return res;
}
