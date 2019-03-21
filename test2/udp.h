#ifndef UDP_H
#define UDP_H

#include <QObject>
#include <QUdpSocket> //UDP套接字
#include <QDebug>
#include <QTimer>

class UDP : public QObject
{
    Q_OBJECT
public:
    explicit UDP(QObject *parent = nullptr);
    char* udptransmit(QString ipstr, int ports,QUdpSocket *udpsocket,QTimer *mytime,QObject *parent,bool dealdata1);

    void dealMsg(char *); //槽函数，处理对方发过来的数据
    void sendMsg();//发送数据
signals:
   // void getudp(char *);

public slots:

private:
     QUdpSocket *udpsocket; //UDP套接字

};

#endif // UDP_H
