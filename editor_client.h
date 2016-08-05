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

struct Operation
{
    qint32 pos;
    qint32 priority;
    qint32 time_stamp[2];
    quint8 c;
};

class EditorCliente : public QWidget
{
    Q_OBJECT

public:
    EditorCliente(QWidget *parent = 0);
    ~EditorCliente();

    void sendMessage(char *operacion);
    Operation operat_transformation(Operation o1, Operation o2);

    QTcpSocket *sock;

private slots:
    void onTextChanged();
    void onCursorPositionChanged();

public slots:
    void m_read();
    void acceptConnection();

protected:
    virtual void keyReleaseEvent(QKeyEvent * event);

private:
    bool writing_to_box;
    QTextCursor m_cursor;
    QVBoxLayout m_layout;
    QTextEdit m_textEdit;
    QLabel m_label;
    QString t;
    int position;
    char caracter;
    int id_cliente;
    int time_stamps[2];

    //Operation lista_operaciones[20];
    std::list<Operation> lista_local;
    std::list<Operation> lista_operaciones;

    Operation buscaEnLista(std::list<Operation>, int);
    //void push(Operation);

    QTcpServer server;
};

#endif // WIDGET_H
