#include "thread_readsql.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

thread_readsql::thread_readsql(QObject *parent) : QObject(parent)
{
    isstop=false;
}

void thread_readsql::stopit(bool flags){
    isstop=flags;
}

void thread_readsql::readdate(){
    //创建finddata数据库
    sqlcreate();
    //数据初始化

    //时间寄存器分配空间
    mytime_read_save=new QTimer(this);

    //开始数据处理
    read_save_data();
    mid_hour=0;
}

//数据库创建
void thread_readsql::sqlcreate(){
    QString str1=QString("create table Savedata(year int , month int , day int ,"
                         "hour int , capacity long, abn1 int, abn2 int, abn3 int,"
                         "abn4 int, abn5 int, abn6 int, abn7 int, abn8 int, abn9 int,"
                         "abn10 int, abn11 int, abn12 int, abn13 int, abn14 int, abn15 int,"
                         "abn16 int, abn17 int, abn18 int, abn19 int, abn20 int, abn21 int,"
                         "abn22 int, abn23 int, abn24 int, abn25 int, abn26 int, abn27 int,"
                         "abn28 int, abn29 int, abn30 int, abn31 int, abn32 int, "
                         "primary key( year, month , day , hour ) );");

    QString str2=QString("create table Finddata(start_date varchar(255) ,start_hour varchar(255) ,"
                         "capacity long ,abn_all int , abn_str varchar(500),"
                         "primary key( start_date, start_hour) );");

    QSqlDatabase::database();
    //创建表
    QSqlQuery query;
    query.exec(str1);
    query.exec(str2);
}

//数据库读取与保存触发函数
void thread_readsql::read_save_data(){
    //定时器开始
    mytime_read_save->start(400);

    connect(mytime_read_save,&QTimer::timeout,
            [=](){
                if(mid_hour<24){
                    //获取当前时间
                    QDateTime local(QDateTime::currentDateTime());
                    year_current = local.toString("yy").toInt();
                    month_current= local.toString("MM").toInt();
                    day_cuurent = local.toString("dd").toInt();
                    hour_current = local.toString("hh").toInt();
                    //开始数据采集
                    data_read_save(year_current,3,3,mid_hour);

                    mid_hour++;
                }else {
                    mytime_read_save->stop();
                    mid_hour=0;
                }
            });
}

//操作函数
void thread_readsql::data_read_save(int year, int month, int day, int hour){
    Readdata_struct data_tosql;
    Savedata_struct data_savesql;
    //数组定义及初始化
    int abn_record[32];
    for (int i=0;i<32;i++) {
     abn_record[i]=0;
    data_savesql.abn1[i]=0;
    }

    //find数据库组包字节开始
    QString str_date_find="";
    QString str_hour_find="";
    if(month>9){
        str_date_find+=QString("%1-%2-").arg(year).arg(month);
    }else {
        str_date_find+=QString("%1-0%2-").arg(year).arg(month);
    }
    if(day>9){
        str_date_find+=QString("%1").arg(day);
    }else {
        str_date_find+=QString("0%1").arg(day);
    }
    if(hour>9){
        str_hour_find+=QString("%1:00").arg(hour);
    }else {
         str_hour_find+=QString("0%1:00").arg(hour);
    }
    QString str_abn_find=QString("时间:%1 %2#").arg(str_date_find).arg(str_hour_find);


    long capacity_start=0;
    long capacity_end=0;
    int abn_record_all=0;
    int start_mark=0;
    //数据库读取
    QString str1=QString("select * from PLCdate1 where year=%1 and month=%2 and day=%3 "
                         "and hour=%4").arg(year).arg(month).arg(day).arg(hour);
    QSqlDatabase::database();
    QSqlQuery query;
    query.exec(str1);
    while (query.next()) {
            data_tosql.year=query.value(0).toInt();
            data_tosql.month=query.value(1).toInt();
            data_tosql.day=query.value(2).toInt();
            data_tosql.hour=query.value(3).toInt();
            data_tosql.capacity=query.value(6).toLongLong();
            data_tosql.abnormal=query.value(7).toString();
            data_tosql.type=query.value(8).toInt();
            data_tosql.def1=query.value(9).toInt();
            /*.....*/
            //起始产能记录
            if(start_mark==0){
                data_savesql.year=data_tosql.year;
                data_savesql.month=data_tosql.month;
                data_savesql.day=data_tosql.day;
                data_savesql.hour=data_tosql.hour;
                capacity_start=data_tosql.capacity;
                start_mark++;
            }else {
                capacity_end=data_tosql.capacity;
            }
            //异常记录
            if(data_tosql.abnormal!="00000000000000000000000000000000"&&
               data_tosql.abnormal.count()==32){
                for (int i=0;i<32;i++) {
                    if(data_tosql.abnormal.at(i)!='0')
                        abn_record[i]++;
                }
               abn_record_all++;
            }

    }

    data_savesql.capacity=capacity_end-capacity_start;

    str_abn_find+=QString("产能:%1#").arg(data_savesql.capacity);
    //将异常转移至结构体
    for(int i=0;i<32;i++){
    data_savesql.abn1[i]=abn_record[i];
    }

    str_abn_find+=QString("总异常时间:%1(s)#").arg(abn_record_all);
    str_abn_find+=abnormal_record(data_savesql.abn1);

    if(data_savesql.year>18&&data_savesql.year<30&&data_savesql.capacity>0&&data_savesql.capacity<5000){
        QString str_insert_save=QString("insert into Savedata(year, month, day , hour, capacity,"
                                "abn1 , abn2 , abn3 , abn4 , abn5 , abn6 , abn7 , abn8,"
                                "abn9 , abn10 , abn11 , abn12 , abn13 , abn14 , abn15 , abn16,"
                                "abn17 , abn18 , abn19 , abn20 , abn21 , abn22 , abn23 , abn24,"
                                "abn25 , abn26 , abn27 , abn28 , abn29 , abn30 , abn31 , abn32)"
                                "values( %1 ,  %2,  %3 ,  %4 ,  %5 ,  %6 , %7 , %8 , %9 , %10 ,"
                                "%11 , %12 , %13 , %14 , %15 , %16 , %17 , %18 , %19 , %20 , %21 ,"
                                "%22 , %23 , %24 , %25 , %26 , %27 , %28 , %29 , %30 , %31 , %32 ,"
                                "%33 , %34 , %35 , %36 , %37);")
                .arg(year).arg(month).arg(day).arg(hour).arg(data_savesql.capacity)
                .arg(data_savesql.abn1[0]).arg(data_savesql.abn1[1]).arg(data_savesql.abn1[2])
                .arg(data_savesql.abn1[3]).arg(data_savesql.abn1[4]).arg(data_savesql.abn1[5])
                .arg(data_savesql.abn1[6]).arg(data_savesql.abn1[7]).arg(data_savesql.abn1[8])
                .arg(data_savesql.abn1[9]).arg(data_savesql.abn1[10]).arg(data_savesql.abn1[11])
                .arg(data_savesql.abn1[12]).arg(data_savesql.abn1[13]).arg(data_savesql.abn1[14])
                .arg(data_savesql.abn1[15]).arg(data_savesql.abn1[16]).arg(data_savesql.abn1[17])
                .arg(data_savesql.abn1[18]).arg(data_savesql.abn1[19]).arg(data_savesql.abn1[20])
                .arg(data_savesql.abn1[21]).arg(data_savesql.abn1[22]).arg(data_savesql.abn1[23])
                .arg(data_savesql.abn1[24]).arg(data_savesql.abn1[25]).arg(data_savesql.abn1[26])
                .arg(data_savesql.abn1[27]).arg(data_savesql.abn1[28]).arg(data_savesql.abn1[29])
                .arg(data_savesql.abn1[30]).arg(data_savesql.abn1[31]);

        QString str_insert_find=QString("insert into Finddata(start_date,start_hour, capacity, "
                                        "abn_all , abn_str)"
                                        "values('%1' ,'%2' ,%3 ,%4 , '%5' );").arg(str_date_find)
                                        .arg(str_hour_find).arg(data_savesql.capacity)
                                        .arg(abn_record_all).arg(str_abn_find);
        query.exec(str_insert_save);
        query.exec(str_insert_find);

        emit signal_readdata(year,month,day,hour,data_savesql.capacity,abn_record_all);
    }else {
        emit signal_readdata(year,month,day,hour,0,0);
        QString str_insert_save=QString("insert into Savedata(year, month, day , hour, capacity)"
                                        "values( %1 ,  %2,  %3 ,  %4 ,  %5);")
                                .arg(year).arg(month).arg(day).arg(hour).arg(data_savesql.capacity);

        QString str_insert_find=QString("insert into Finddata(start_date ,start_hour ,capacity, "
                                        "abn_all , abn_str)"
                                        "values('%1' ,'%2',0 ,0 ,'%3' );").arg(str_date_find)
                                        .arg(str_hour_find).arg("通讯异常");
        query.exec(str_insert_save);
        query.exec(str_insert_find);
    }
}

QString thread_readsql::abnormal_record(int abn_record[32]){
    QString str_return="";
    for(int i=0;i<32;i++){
        if(i==0&&abn_record[i]>0){
            str_return+=QString("异常1时间:%1").arg(abn_record[i]);
        }
    }
    str_return+=QString("$");
    return  str_return;
}
