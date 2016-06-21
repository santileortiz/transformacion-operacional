#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server: public QObject
{
Q_OBJECT
public:
  Server(QObject * parent = 0);
  ~Server();
public slots:
  void acceptConnection();
  void startRead();
private:
  uint num_clients;
  QTcpServer server;
  QTcpSocket* clients[2];
};
