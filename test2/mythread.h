#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QThread>
#include <QUdpSocket> //UDP套接字
#include <QDebug>
#include <QTimer>

class MyThread : public QObject
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    ~MyThread();
    //定义线程处理函数
    void clink();
    void stopit(bool flags=true );
    void udptransmit(QString ipstr,int port,QUdpSocket *,QTimer *,QObject *parent);
    void sendMsg(char*);//发送数据
    void sqlcreate();
    void PLCdate();
    QString chartobinstr(char *,int star);
    uint chartoint1(char *ch2,int star);

signals:
    void mysignal();

public slots:


private:
    bool isstop;
    QUdpSocket *udpsocket; //UDP套接字
    QByteArray array ;
    QByteArray array1 ;
};

#endif // MYTHREAD_H
