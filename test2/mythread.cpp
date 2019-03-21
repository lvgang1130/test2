#include "mythread.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

MyThread::MyThread(QObject *parent) : QObject(parent)
{
    isstop=false;

}
MyThread::~MyThread(){

}
void MyThread::stopit(bool flags){
    isstop=flags;
}

void MyThread::clink(){
    sqlcreate();
    PLCdate();
   // 在线程中自定义函数实现UDP
    QTimer *mytime;
    qDebug()<<"进入线程1";
    this->udptransmit(QString("192.168.1.100"),4997,udpsocket,mytime,this);
}

//创建UDP连接
void MyThread::udptransmit(QString ipstr, int ports,QUdpSocket *udpsocket,QTimer *mytime,QObject *parent){

    //分配空间，指定父对象
      udpsocket=new QUdpSocket(parent);
    //绑定端口
    udpsocket->bind(QHostAddress::AnyIPv4,8887);
    //udp通讯接收
    //当对方成功发送数据过来自动触发 readyRead()
   connect(udpsocket, &QUdpSocket::readyRead,
           [=]()
           {
           char buf[1024] = {0};
           char *shuchu=buf;
           qint64 len = udpsocket->readDatagram(buf, sizeof(buf));
           if(len>0){
               qDebug()<<"接收开始:"<<shuchu;
               sendMsg(shuchu);//接收数据
               }
           });
    //定时发送数据
    //定时器设置
    mytime=new QTimer(parent);
    mytime->start(1000);
    //关联定时器
    connect(mytime,&QTimer::timeout,
            [=]()
            {
            //给指定的IP发送数据
            udpsocket->writeDatagram(array1, QHostAddress(ipstr), ports);
            });
}

void MyThread::sendMsg(char *shuchu){


     qDebug()<<"字符长度"<<sizeof (shuchu);

    qDebug()<<"数据处理:"<<shuchu;
    qDebug()<<"数据显示:"<<shuchu[4]<<"  "<<shuchu[5]<<"  "<<shuchu[6]<<"   "<<shuchu[7];

    //接收总产能数据
    long capacity=chartoint1(shuchu,5)+chartoint1(shuchu,9)*16*16*16*16;
    qDebug()<<capacity;
    QString abnormal=chartobinstr(shuchu,13)+chartobinstr(shuchu,18);
    uint type=chartoint1(shuchu,21);
    uint def1=chartoint1(shuchu,25);
    uint def2=chartoint1(shuchu,29);
    uint def3=chartoint1(shuchu,33);
    uint def4=chartoint1(shuchu,37);
    uint def5=chartoint1(shuchu,41);
    uint def6=chartoint1(shuchu,45);
    uint person1=chartoint1(shuchu,49);
    uint person2=chartoint1(shuchu,53);
    uint person3=chartoint1(shuchu,57);
    uint person4=chartoint1(shuchu,61);



    //获取当前时间
    QDateTime local(QDateTime::currentDateTime());
    int year = local.toString("yy").toInt();
    int month = local.toString("MM").toInt();
    int day = local.toString("dd").toInt();
    int hour = local.toString("hh").toInt();
    int minute = local.toString("mm").toInt();
    int second = local.toString("ss").toInt();
    //将数据写入写入数据库
    QString str=QString("insert into PLCdate(year, month, day , hour, minute, second,"
                        "capacity , abnormal , type , def1 , def2 , def3 , def4 , def5,"
                        "def6 , person1 , person2 , person3 ,person4 )"
                        " values( %1 ,  %2,  %3 ,  %4 ,  %5 ,  %6 , %7 , '%8' , %9 , %10 ,"
                        "%11 , %12 , %13 , %14 , %15 , %16 , %17 , %18 , %19);")
                        .arg(year).arg(month).arg(day).arg(hour).arg(minute).arg(second).arg(capacity)
                        .arg(abnormal).arg(type).arg(def1).arg(def2).arg(def3).arg(def4).arg(def5)
                        .arg(def6).arg(person1).arg(person2).arg(person3).arg(person4);
    QSqlDatabase::database();
    QSqlQuery query;
    query.exec(str);
}

void MyThread::sqlcreate(){
    //关联已经连接的数据库
    QSqlDatabase::database();
    //创建表
    QSqlQuery query;
    query.exec("create table PLCdate(year int , month int , day int ,"
               "hour int , minute int , second int , "
               "capacity long, abnormal varchar(255), type int,"
               "def1 int , def2 int , def3 int ,def4 int , def5 int , def6 int ,"
               "person1 int , person2 int , person3 int ,person4 int ,"
               "primary key( year, month , day , hour ,minute , second ) );");
}

//发送指令组合
void MyThread::PLCdate(){
    //读取m900到m905数据
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

    //读取D1000到D1005值
    array1[0]=0x05;
    array1[1]=0x30;
    array1[2]=0x30;
    array1[3]=0x46;
    array1[4]=0x46;
    array1[5]=0x57;
    array1[6]=0x52;
    array1[7]=0x30;
    array1[8]=0x44;
    array1[9]=0x31;
    array1[10]=0x30;
    array1[11]=0x30;
    array1[12]=0x30;
    array1[13]=0x33;
    array1[14]=0x30;
}

//将字符转化为int类型，并对字符顺序处理后计算
uint MyThread::chartoint1(char *ch2, int star){

    uint in1;
    double doub=0;
    for(int i=3;i>=0;i--){
        QString str;
        str=ch2[star+i];
        int mm=str.toInt();
        if(mm>0&mm<10){

        }else{
           if(ch2[star+i]=='A')
               mm=10;
           if(ch2[star+i]=='B')
               mm=11;
           if(ch2[star+i]=='C')
               mm=12;
           if(ch2[star+i]=='D')
               mm=13;
           if(ch2[star+i]=='E')
               mm=14;
           if(ch2[star+i]=='F')
               mm=15;

        }
        double doub1=1;
        for(int j=0;j<3-i;j++){
            doub1=doub1*16;
        }
        doub+=mm*doub1;
    }
    in1=(uint)doub;
    return in1;
}

//将字符转化为二进制字符串显示，用于报警输出
QString  MyThread::chartobinstr(char *ch1, int star){
    QString str="";
    for(int i=3;i>=0;i--){
        char ch2=ch1[star+i];
        if(ch2=='0')
            str+="0000";
        if(ch2=='1')
            str+="1000";
        if(ch2=='2')
            str+="0100";
        if(ch2=='3')
            str+="1100";
        if(ch2=='4')
            str+="0010";
        if(ch2=='5')
            str+="1010";
        if(ch2=='6')
            str+="0110";
        if(ch2=='7')
            str+="1110";
        if(ch2=='8')
            str+="0001";
        if(ch2=='9')
            str+="1001";
        if(ch2=='A')
            str+="0101";
        if(ch2=='B')
            str+="1101";
        if(ch2=='C')
            str+="0011";
        if(ch2=='D')
            str+="1011";
        if(ch2=='E')
            str+="0111";
        if(ch2=='F')
            str+="1111";
    }
    return str;
}


