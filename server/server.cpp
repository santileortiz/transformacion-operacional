// server.cc
#include "server.h"
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

Server::Server(QObject* parent): QObject(parent)
{
  num_clients = 0;
  connect(&server, SIGNAL(newConnection()),
    this, SLOT(acceptConnection()));

  server.listen(QHostAddress::Any, 2346);
}

Server::~Server()
{
  server.close();
}

void Server::acceptConnection()
{
  clients[num_clients] = server.nextPendingConnection();
  quint16 port = clients[num_clients]->localPort();
  QString port_s = QString::number(port);
  cout << "Puerto: ";
  cout << port << endl;

  connect(clients[num_clients], SIGNAL(readyRead()),
    this, SLOT(startRead()));
  num_clients++;
}

void Server::startRead()
{
    cout << "Paquete recibido!"<< endl;
    Transform transform;
    QTcpSocket *tcpSocket = (QTcpSocket*)sender();

    if (tcpSocket->bytesAvailable() < 9) {
        return;
    }

    QByteArray block = tcpSocket->read(9);
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream >> transform;

    cout << "Posicion: " ;
    cout << transform.pos << endl;
    cout << "Caracter: " ;
    cout << transform.c << endl;
    cout << "Prioridad: " << transform.priority << endl;

    //transform.pos = 1;
    //sendStream << transform;

    tcpSocket->write(block);
}
