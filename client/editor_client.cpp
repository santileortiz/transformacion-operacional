#include "editor_client.h"


#include <QString>
#include <iostream>

using namespace std;

EditorCliente::EditorCliente(QWidget *parent)
    : QWidget(parent)
{
    connect(&client, SIGNAL(connected()), this, SLOT(startTransfer()));
    connect(&m_textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    connect(&m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));

    start("127.0.0.1", 8888);

    m_layout.addWidget(&m_textEdit);
    m_layout.addWidget(&m_label);

    setLayout(&m_layout);
}

EditorCliente::~EditorCliente(){
  client.close();
}

void EditorCliente::onTextChanged(){
    t = m_textEdit.toPlainText();
    //QString tt;// = m_textEdit.toPlainText();

    //char c;
    int cursor = m_textEdit.textCursor().positionInBlock() - 1;

    //QStringRef subString(tt, cursor, cursor+1);
    char c = t[cursor].toLatin1();
    //int i = QString("myTextHere")[0].unicode();

    //char c = t.at(cursor).toAscii();

    cout << t.toStdString() << endl;
    //cout << "(" << cursor << ", " << tt.toStdString() << ")" << endl;
    cout << "(" << c << ", " << cursor << ")" << endl;
    sendMessage();
    //cliente.client.write("Hello, world", 13);
    //cout << cursor << endl;
}

void EditorCliente::onCursorPositionChanged(){
    // Code that executes on cursor change here
    m_cursor = m_textEdit.textCursor();
    m_label.setText(QString("Position: %1").arg(m_cursor.positionInBlock()));
    //int cursor = m_cursor.positionInBlock();
    //cout << "cursor: #" << cursor << endl;
}

void EditorCliente::start(QString address, quint16 port)
{
  QHostAddress addr(address);
  client.connectToHost(addr, port);
}

void EditorCliente::startTransfer()
{
printf("intento mandar un mensaje al servidor\n");
  //client.write("Hello, world", 13);
}

void EditorCliente::sendMessage(){
    printf("llamo a sendMessage\n");

    client.write("Hola", 4);
}
