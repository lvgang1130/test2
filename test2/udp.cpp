#include "udp.h"

UDP::UDP(QObject *parent) : QObject(parent)
{

}
char* UDP::udptransmit(QString ipstr, int ports,QUdpSocket *udpsocket,QTimer *mytime,QObject *parent,bool dealdata1){

    //分配空间，指定父对象
      udpsocket=new QUdpSocket(parent);

    //绑定
    udpsocket->bind(QHostAddress::AnyIPv4,8887);

    qDebug()<<"连接完成";
//udp通讯接收
    //当对方成功发送数据过来自动触发 readyRead()
   connect(udpsocket, &QUdpSocket::readyRead,
           [&]()
           {

               char buf[1024] = {0};
               char *shuchu=buf;
               QHostAddress cliAddr; //对方地址
               quint16 port;    //对方端口
               qint64 len = udpsocket->readDatagram(buf, sizeof(buf), &cliAddr, &port);

               qDebug()<<"接收开始:"<<shuchu;
               return  shuchu;

           });

//定时发送数据
    //定时器设置
    mytime=new QTimer(parent);
    mytime->start(1000);
    //关联定时器
    connect(mytime,&QTimer::timeout,
            [=]()
            {
        qDebug()<<"定时器开启";
        //先获取对方的IP和端口
        QString ip = ipstr;
        qint16 port = ports;

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
            });
}

