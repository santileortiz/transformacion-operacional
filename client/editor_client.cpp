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

    start("127.0.0.1", 2345);

    m_layout.addWidget(&m_textEdit);
    m_layout.addWidget(&m_label);

    setLayout(&m_layout);
}

EditorCliente::~EditorCliente(){
  sock.close();
}

void EditorCliente::onTextChanged(){
    Transform new_transform;

    QByteArray block;
    QDataStream sendStream(&block, QIODevice::ReadWrite);

    t = m_textEdit.toPlainText();

    new_transform.pos = m_textEdit.textCursor().positionInBlock() - 1;
    new_transform.c = t[new_transform.pos].toLatin1();
    new_transform.priority = 0;
    sendStream << new_transform;
    sock.write(block);

    cout << t.toStdString() << endl;
    cout << "(" << new_transform.c << ", " << new_transform.pos << ")" << endl;

    cout << "Tamaño:" ;
    cout << block.size() << endl;
}

void EditorCliente::onCursorPositionChanged(){
    m_cursor = m_textEdit.textCursor();
    m_label.setText(QString("Position: %1").arg(m_cursor.positionInBlock()));
}

void EditorCliente::start(QString address, quint16 port)
{
  QHostAddress addr(address);
  sock.connectToHost(addr, port);
}

