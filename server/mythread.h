#include <QThread>
#include <QTcpSocket>

class MyThread : public QThread
{
  Q_OBJECT

  public:
    explicit MyThread(int ID, QObject *parent =0);        //We added an ID to the constructor (For the socket ID number)

    //Just because it's a thread we will ad a run function. This will run when the socket connects.
    //Basically we're overiding the QThread run function
    void run();

  signals:
    void error(QTcpSocket::SocketError socketerror);    //If something goes wrong we'll emit that

  public slots:
    void readyRead();
    void disconnected();

  private:
    QTcpSocket *socket;
    int socketDescriptor;
};
