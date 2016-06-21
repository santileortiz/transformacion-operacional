#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

struct Transform
{
    qint32 pos;
    quint8 c;
    qint32 priority;
};

class Server: public QObject
{
Q_OBJECT
public:
  Server(QObject * parent = 0);
  ~Server();
public slots:
  void acceptConnection1();
  void acceptConnection2();
  void read_from_client_1();
  void read_from_client_2();
  void write_to_client (Transform transform, int cli_number);
private:
  uint num_clients;
  QTcpServer server1;
  QTcpServer server2;
  QTcpSocket* clients[2];
};
