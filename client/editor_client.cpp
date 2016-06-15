#include "editor_client.h"
#include <stdlib.h>

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
    char c[2] = "";
    char operacion[256] = "";
    char cursor_char[3];

    c[0] = t[cursor].toLatin1();
    //itoa(cursor, cursos_char, 10);
    sprintf(cursor_char, "%d", cursor);

    //int i = QString("myTextHere")[0].unicode();

    //char c = t.at(cursor).toAscii();

    cout << t.toStdString() << endl;
    //cout << "(" << cursor << ", " << tt.toStdString() << ")" << endl;
    cout << "(" << c << ", " << cursor << ")" << endl;
    //strcat(operacion, c);
    if(cursor < 100) strcat(operacion, "0");
    if(cursor < 10) strcat(operacion, "0");

    strcat(operacion, cursor_char);

    strcat(operacion, c);

    sendMessage(operacion);
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
//printf("intento mandar un mensaje al servidor\n");
  //client.write("Hello, world", 13);
}

void EditorCliente::sendMessage(char *operacion){
    printf("llamo a sendMessage %s\n", operacion);

    client.write(operacion, strlen(operacion));
    //client.write("hola", 4);
    client.waitForReadyRead(-1);
    //qDebug() << "Reading: " << client.bytesAvailable();
    qDebug() << client.readAll();

}
