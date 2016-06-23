
#include "mythread.h"

MyThread::MyThread(int ID, QObject *parent):
  QThread(parent)
{
  this->socketDescriptor = ID;            //Get the socket ID number
}


void MyThread::run()
{
  //thread starts here

  socket = new QTcpSocket();

  if ( !socket->setSocketDescriptor(this->socketDescriptor) )   //Here we set the socket ID
  {
    emit error (socket->error());         //emit the error signal
    return;
  }

  connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()),Qt::DirectConnection );  //Make a direct connection to the thread
  connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

  //client is connected

  //IMPORTANT
  //This function will cause the thread to stay alive until we tell it to close
  //Otherwise the run() function will end and the thread will be dropped / destroyed
  exec();
}


void MyThread::readyRead()
{
  QByteArray Data = socket->readAll();    //Get all the information from the connected client

  //Send the info back, (echo server)
  socket->write(Data);
}


void MyThread::disconnected()
{
  socket->deleteLater();
  exit(0);
}
