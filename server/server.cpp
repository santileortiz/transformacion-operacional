// server.cc
#include "server.h"
#include "mythread.h"
#include <iostream>

using namespace std;

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

Server::Server(QObject* parent): QTcpServer(parent)//QObject(parent)
{
  num_clients = 0;
  num_transformaciones = 0;

  //server1.moveToThread(&thread1);
  //server2.moveToThread(&thread2);

  connect(&server1, SIGNAL(newConnection()),
    this, SLOT(acceptConnection1()));
  server1.listen(QHostAddress::Any, 2345);

  connect(&server2, SIGNAL(newConnection()),
    this, SLOT(acceptConnection2()));
  server2.listen(QHostAddress::Any, 2346);

  //thread1.start();
  //thread2.start();
}

Server::~Server()
{
  server1.close();
  server2.close();
  //thread1.stop();
  //thread2.stop();

}

void Server::acceptConnection1()
{
  num_clients++;
  clients[1] = server1.nextPendingConnection();
  quint16 port = clients[1]->localPort();
  QString port_s = QString::number(port);
  cout << "Puerto: ";
  cout << port << endl;

  connect(clients[1], SIGNAL(readyRead()),
    this, SLOT(read_from_client_1()));
}

void Server::acceptConnection2()
{
  num_clients++;
  clients[2] = server2.nextPendingConnection();
  quint16 port = clients[2]->localPort();
  QString port_s = QString::number(port);
  cout << "Puerto: ";
  cout << port << endl;

  connect(clients[2], SIGNAL(readyRead()),
    this, SLOT(read_from_client_2()));
}

void Server::write_to_client (Transform transform, int cli_number) {
    QByteArray block;
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream << transform;
    clients[cli_number]->write(block);
}

Transform Server::operat_transformation (Transform t1, Transform t2){
    //mutex.lock();

    Transform res;
    res.c = t1.c;
    res.priority = t1.priority;
    if (t1.pos < t2.pos ||
            (t1.pos==t2.pos && t1.c!=t2.c && t1.priority<t2.priority)) {
        res.pos = t1.pos;
    } else if (t1.pos > t2.pos ||
            (t1.pos==t2.pos && t1.c!=t2.c && t1.priority>t2.priority)) {
        res.pos = t1.pos+1;
    } else {
        res.priority = -1;
    }
    return res;

    //mutex.unlock();
}

void Server::read_from_client_1()
{
    if (num_clients !=2)
        return;

    //mutex.lock();

    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
    Transform transform;

    cout << "(1) bytes: " << tcpSocket->bytesAvailable() << endl;

    if (tcpSocket->bytesAvailable() < 9)
        return;

    QByteArray block = tcpSocket->read(9);
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream >> transform;

    transform_client1 = transform;

    mutex.lock();
    num_transformaciones++;
    mutex.unlock();

    transform.priority = 2;

    int cont = 0;
    while(cont < 99999999){
        cont++;
    }

    if(num_transformaciones == 2){
        transform = operat_transformation(transform_client1, transform_client2);
    }

    cout << "(1) # Transformaciones: " << num_transformaciones << endl;

    cout << "Posicion: " ;
    cout << transform.pos << endl;
    cout << "Caracter: " ;
    cout << transform.c << endl;
    cout << "Prioridad: " << transform.priority << endl;

    //transform.priority = 1;
    write_to_client (transform, 2);

    num_transformaciones--;
    //mutex.unlock();
}

void Server::read_from_client_2()
{
    if (num_clients !=2)
        return;

    QTcpSocket *tcpSocket = (QTcpSocket*)sender();
    Transform transform;

    cout << "(2) bytes: " << tcpSocket->bytesAvailable() << endl;

    if (tcpSocket->bytesAvailable() < 9)
        return;

    QByteArray block = tcpSocket->read(9);
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream >> transform;

    transform_client2 = transform;

    mutex.lock();
    num_transformaciones++;
    mutex.unlock();

    transform.priority = 2;

    int cont = 0;
    while(cont < 99999999){
        cont++;
    }

    if(num_transformaciones == 2){
        transform = operat_transformation(transform_client2, transform_client1);
    }

    cout << "(2) # Transformaciones: " << num_transformaciones << endl;

    cout << "Posicion: " ;
    cout << transform.pos << endl;
    cout << "Caracter: " ;
    cout << transform.c << endl;
    cout << "Prioridad: " << transform.priority << endl;

    write_to_client (transform, 1);

    num_transformaciones--;
    //mutex.unlock();
}

/*
void Server::incomingConnections(int socketDescriptor)    //Incoming connections
{
  MyThread *thread = new MyThread(socketDescriptor,this);

  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

  //Start a new thread for the connection
  thread->start();    //Which will cause the run() function
}
*/
