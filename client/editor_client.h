#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>

#include <QtNetwork>
#include <QObject>
#include <QTcpSocket>

class EditorCliente : public QWidget
{
    Q_OBJECT

public:
    EditorCliente(QWidget *parent = 0);
    ~EditorCliente();

    void start(QString address, quint16 port);
    void sendMessage(char *operacion);
    QTcpSocket sock;

private slots:
    void onTextChanged();
    void onCursorPositionChanged();
    //void m_read();

public slots:
    void m_read();

private:
    bool writing_to_box;
    QTextCursor m_cursor;
    QVBoxLayout m_layout;
    QTextEdit m_textEdit;
    QLabel m_label;
    QString t;
    int position;
    char caracter;
};

#endif // WIDGET_H
