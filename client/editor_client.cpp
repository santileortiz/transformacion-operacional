#include "editor_client.h"
#include <stdlib.h>

#include <QString>
#include <iostream>

using namespace std;

struct Transform
{
    qint32 pos;
    quint8 c;
    qint32 priority;
};

QDataStream &operator<<(QDataStream &out, const Transform &transform)
{
    out << transform.pos << transform.c << transform.priority;
    return out;
}

QDataStream &operator>>(QDataStream &in, Transform &transform)
{
    in >> transform.pos >> transform.c >> transform.priority;
    return in;
}

EditorCliente::EditorCliente(QWidget *parent)
    : QWidget(parent)
{
    connect(&m_textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(&m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    //connect(&sock, SIGNAL(readyRead()), this, SLOT(m_read()));
    //connect(&sock, SIGNAL(bytesWritten()), this, SLOT(m_read()));
    connect(&sock, SIGNAL(readyRead()), this, SLOT(m_read()));

    m_cursor = m_textEdit.textCursor();

    QStringList args = QCoreApplication::arguments();
    
    if (args.count() == 1) {
        m_label.setText(QString("Cliente 1"));
        start("127.0.0.1", 2345);
    } else {
        m_label.setText(QString("Cliente 2"));
        //start(args.at(1), 2346);
        start("127.0.0.1", 2346);
    }

    m_layout.addWidget(&m_textEdit);
    m_layout.addWidget(&m_label);

    setLayout(&m_layout);
}

EditorCliente::~EditorCliente(){
  sock.close();
}

void EditorCliente::onTextChanged(){
    if (writing_to_box) 
        return;

    Transform new_transform;

    QByteArray block;
    QDataStream sendStream(&block, QIODevice::ReadWrite);

    t = m_textEdit.toPlainText();

    new_transform.pos = m_textEdit.textCursor().positionInBlock() - 1;
    new_transform.c = t[new_transform.pos].toLatin1();
    new_transform.priority = 0;
    sendStream << new_transform;
    sock.write(block);

    cout << "Paquete enviado"<< endl;
    cout << "Posicion: " ;
    cout << new_transform.pos << endl;
    cout << "Caracter: " ;
    cout << new_transform.c << endl;
    cout << "Prioridad: " << new_transform.priority << endl;
}

void EditorCliente::onCursorPositionChanged(){
}

void EditorCliente::start(QString address, quint16 port)
{
  QHostAddress addr(address);
  sock.connectToHost(addr, port);
}

void EditorCliente::m_read(/* arguments */) {
    Transform transform;
    QTcpSocket *tcpSocket = (QTcpSocket*)sender();

    if (tcpSocket->bytesAvailable() < 9) {
        return;
    }

    QByteArray block = tcpSocket->read(9);
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream >> transform;

    cout << "Respuesta del servidor:"<< endl;
    cout << "Posicion: " ;
    cout << transform.pos << endl;
    cout << "Caracter: " ;
    cout << transform.c << endl;
    cout << "Prioridad: " << transform.priority << endl;

    cout << "*****Caracter: " << caracter << endl;

    t = t.insert(transform.pos, transform.c);

    writing_to_box = true;
    m_textEdit.setText(t);
    writing_to_box = false;
}
