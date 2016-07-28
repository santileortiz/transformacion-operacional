#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
//#include <QMutex>

struct Transform
{
    qint32 pos;
    qint32 priority;
    qint32 time_stamp;
    quint8 c;

};

class Server: public QTcpServer//QObject
{
Q_OBJECT
public:
  explicit Server(QObject *parent=0);

  //Server(QObject * parent = 0);
  ~Server();
  //Transform operat_transformation (Transform t1, Transform t2);
public slots:
  void acceptConnection1();
  void acceptConnection2();
  void read_from_client_1();
  void read_from_client_2();
  void write_to_client (Transform transform, int cli_number);
private:
  uint num_clients;
  //uint num_transformaciones;
  Transform transform_client1;
  Transform transform_client2;
  //QMutex mutex;
  QTcpServer server1;
  QTcpServer server2;
  QTcpSocket* clients[2];
protected:
   void incomingConnections(int socketDescriptor);   //This is where we deal with incoming connections

};
