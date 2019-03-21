#include "udptest.h"

udptest::udptest(QObject *parent) : QObject(parent)
{
    //分配空间，指定父对象


}
void udptest::udpbind(QString ipstr, int ports,QObject *parent){
    udpsocket = new QUdpSocket(parent);
    mytime=new QTimer(parent);
    //绑定
    //udpSocket->bind(8888);
    udpsocket->bind(QHostAddress::AnyIPv4, 8888);
    mytime->start(1000);
    //先获取对方的IP和端口
    QString ip = ipstr;
    qint16 port = ports;
    connect(udpsocket, &QUdpSocket::readyRead, this, &udptest::dealMsg);
    connect(mytime,&QTimer::timeout,this,&udptest::sendMsg);

}
void udptest::dealMsg()
{
    char buf[1024] = {0};
    char *shuchu=buf;
    QHostAddress cliAddr; //对方地址
    quint16 port;    //对方端口
    qint64 len = udpsocket->readDatagram(buf, sizeof(buf), &cliAddr, &port);
    if(len>0)
    qDebug()<<"接收开始:"<<shuchu;
}

void udptest::sendMsg(){


    //获取编辑区内容
    QByteArray array ;
    array[0]=0x05;
    array[1]=0x30;
    array[2]=0x30;
    array[3]=0x46;
    array[4]=0x46;
    array[5]=0x42;
    array[6]=0x52;
    array[7]=0x41;
    array[8]=0x4D;
    array[9]=0x30;
    array[10]=0x39;
    array[11]=0x30;
    array[12]=0x30;
    array[13]=0x30;
    array[14]=0x35;

    //给指定的IP发送数据
    udpsocket->writeDatagram(array, QHostAddress(ip), port);
}
