#ifndef THREAD_READSQL_H
#define THREAD_READSQL_H

#include <QObject>
#include <QTimer>

struct Readdata_struct
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    long capacity;
    QString abnormal;
    int type;
    int def1;
    int def2;
    int def3;
    int def4;
    int def5;
    int def6;
    int person1;
    int person2;
    int person3;
    int person4;
};

struct Savedata_struct{
    int year;
    int month;
    int day;
    int hour;
    double capacity;
    int abn1[32];
};

class thread_readsql : public QObject
{
    Q_OBJECT
public:

    explicit thread_readsql(QObject *parent = nullptr);
    void readdate();
    void read_all();
    void linksql();
    Readdata_struct select_capacity(int year,int month,int day,int hour,int minute,int second);
    void stopit(bool);
    void read_save_data();
    void sqlcreate();
    void data_read_save(int year,int month,int day,int hour);
    QString abnormal_record(int*);

    QTimer *mytime_read_save;

signals:
     void mysignal();
     void signal_readdata(int , int , int , int ,double, int );

public slots:

private:
     long capacity1;
     bool isstop;

     int year_current;
     int month_current;
     int day_cuurent;
     int hour_current;
     int mid_hour;

};

#endif // THREAD_READSQL_H
