#ifndef MYTHREAD2_H
#define MYTHREAD2_H

#include <QObject>
#include <QThread>
#include <QTcpSocket> //tcp套接字
#include <QDebug>
#include <QTimer>

class MyThread2 : public QObject
{
    Q_OBJECT
public:
    explicit MyThread2(QObject *parent = nullptr);
    ~MyThread2();
    //定义线程处理函数
    void clink();
    void stopit(bool flags=true );
    void tcptransmit(QString ipstr,int port,QTcpSocket *,QTimer *,QObject *parent,QString str_sql);
    void sendMsg(char*,QString str_sql);//发送数据
    void sqlcreate(QString str_sql);
    void PLCdate();
    void strtoint();
    QString chartobinstr(char *,int star);
    uint chartoint1(char *ch2,int star);

   // void socket_Read_Data();


signals:
    void mysignal();
    void mysignal_err();

public slots:


private:
    bool isstop;
    bool is_setsql;
    QTcpSocket *tcpsocket; //tcp套接字
    QTcpSocket *tcpsocket1;
    QByteArray array ;
    QByteArray array1 ;
    QString sql_1;         //数据库名称
    QString sql_2;
};

#endif // MYTHREAD2_H
