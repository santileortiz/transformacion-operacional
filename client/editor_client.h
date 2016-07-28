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

struct Transform
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

    void start(QString address, quint16 port);
    void sendMessage(char *operacion);
    Transform operat_transformation (Transform t1, Transform t2);

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
    int time_stamps[2];

    //Transform lista_transformaciones[20];
    std::list<Transform> lista_local;
    std::list<Transform> lista_transformaciones;

    Transform buscaEnLista(std::list<Transform>, int);
    //void push(Transform);
};

#endif // WIDGET_H
