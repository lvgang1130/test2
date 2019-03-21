#include "mythread2.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

MyThread2::MyThread2(QObject *parent) : QObject(parent)
{
    isstop=false;
    is_setsql=false;
}

MyThread2::~MyThread2(){

}

void MyThread2::stopit(bool flags){
    isstop=flags;
}

void MyThread2::clink(){

     PLCdate();
     sql_1="PLCdate1";
     sql_2="test1";
     sqlcreate(sql_1);
     sqlcreate(sql_2);

   // 在线程中自定义函数实现TCP
    QTimer *mytime;
    QTimer *mytime1;

    qDebug()<<"进入线程2";
    this->tcptransmit(QString("172.168.1.100"),4997,tcpsocket,mytime,this,sql_1);
    //回环验证
    //this->tcptransmit(QString("127.0.0.1"),4997,tcpsocket1,mytime1,this,sql_2);
}

//创建tcp连接
void MyThread2::tcptransmit(QString ipstr, int ports,QTcpSocket *tcpsocket,QTimer *mytime,QObject *parent,QString str_sql){
    tcpsocket = new QTcpSocket(parent);
    //取消已有的连接
   //tcpsocket->abort();//容易造成内存泄漏  谨慎使用
    //为socket申请100M内存
    tcpsocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,1024*1024*100);
    //连接服务器
    //QTcpSocket类的方法connectToHost会泄露内存, 即使把调用这个方法的QTcpSocket实例delete掉，
    //内存也不会释放！反复connectToHost会导致段错误，十分危险。必须控制connectToHost的使用次数！
    tcpsocket->connectToHost(ipstr, ports); //会有内存泄漏，谨慎的使用！
    //等待连接成功
    if(!tcpsocket->waitForConnected(2000))
    {
        qDebug() << "Connection failed!";
        return;
    }
    qDebug() << "Connect successfully!";
    //定时发送数据
    //定时器设置
    mytime=new QTimer(parent);
    mytime->start(1000);
    //关联定时器
    connect(mytime,&QTimer::timeout,
            [=]()
            {
            //给指定的IP发送数据
            tcpsocket->write(array1);
            });

    //tcp通讯接收
    //当对方成功发送数据过来自动触发 readyRead()
   connect(tcpsocket, &QTcpSocket::readyRead,
           [=]()
           {
               emit mysignal_err();
               QByteArray buffer;
               char* cha1;
               //读取缓冲区数据
               buffer = tcpsocket->readAll();
               cha1=buffer.data();
               if(!buffer.isEmpty())
               {
                   sendMsg(cha1,str_sql);//接收数据
               }
           });
}

//void MyThread2::socket_Read_Data(){
//    QByteArray buffer;
//    char* cha1;
//    //读取缓冲区数据
//    buffer = tcpsocket->readAll();
//    cha1=buffer.data();
//    if(!buffer.isEmpty())
//    {
//        sendMsg(cha1);//接收数据
//    }
//}


void MyThread2::sendMsg(char *shuchu,QString str_sql){


     qDebug()<<"字符长度"<<sizeof (shuchu);
    qDebug()<<"数据处理:"<<shuchu;
    qDebug()<<"数据显示:"<<shuchu[4]<<"  "<<shuchu[5]<<"  "<<shuchu[6]<<"   "<<shuchu[7];

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
    //数据库写入判断
    bool is_abnormal=false;
    if(abnormal==QString("00000000000000000000000000000000")){
        is_abnormal=false;
    }else {
        is_abnormal=true;
}
    if(is_abnormal||second==0){
        is_setsql=true;
    }else {
        is_setsql=false;
}
    //条件满足并写入数据库
    if(is_setsql){
    //将数据写入写入数据库
    QString str=QString("insert into %20(year, month, day , hour, minute, second,"
                        "capacity , abnormal , type , def1 , def2 , def3 , def4 , def5,"
                        "def6 , person1 , person2 , person3 ,person4 )"
                        " values( %1 ,  %2,  %3 ,  %4 ,  %5 ,  %6 , %7 , '%8' , %9 , %10 ,"
                        "%11 , %12 , %13 , %14 , %15 , %16 , %17 , %18 , %19);")
                        .arg(year).arg(month).arg(day).arg(hour).arg(minute).arg(second).arg(capacity)
                        .arg(abnormal).arg(type).arg(def1).arg(def2).arg(def3).arg(def4).arg(def5)
                        .arg(def6).arg(person1).arg(person2).arg(person3).arg(person4).arg(str_sql);
    QSqlDatabase::database();
    QSqlQuery query;
    query.exec(str);
    }
}

void MyThread2::sqlcreate(QString str_sql){

    QString str1=QString("create table %1(year int , month int , day int ,"
                 "hour int , minute int , second int , "
                 "capacity long, abnormal varchar(255), type int,"
                 "def1 int , def2 int , def3 int ,def4 int , def5 int , def6 int ,"
                 "person1 int , person2 int , person3 int ,person4 int ,"
                 "primary key( year, month , day , hour ,minute , second ) );").arg(str_sql);
     QSqlDatabase::database();
    //创建表
    QSqlQuery query;
    query.exec(str1);
}

//发送指令组合
void MyThread2::PLCdate(){
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
uint MyThread2::chartoint1(char *ch2, int star){

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
QString  MyThread2::chartobinstr(char *ch1, int star){
    QString str;
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

