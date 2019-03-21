#ifndef UDPTEST_H
#define UDPTEST_H

#include <QObject>
#include <QUdpSocket> //UDP套接字
#include <QDebug>
#include <QTimer>

class udptest : public QObject
{
    Q_OBJECT
public:
    explicit udptest(QObject *parent = nullptr);

    void udpbind(QString ipstr, int ports,QObject *parent);
    void dealMsg(); //槽函数，处理对方发过来的数据
    void sendMsg();//发送数据
signals:

public slots:

private:
     QUdpSocket *udpsocket; //UDP套接字
     QTimer *mytime;
     QString ip ;
     qint16 port ;

};

#endif // UDPTEST_H
