#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QPushButton>
#include <QSqlQuery>
#include "commonhelper.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    //初始化参数
    day_bool=true;
    night_bool=false;
    //界面初始化
    initwindows();
    //定时器定义
    mytime_tcp_restart=new QTimer(this);
    mytime_tcp_start=new QTimer(this);
    //数据库连接
    sqlcreate();

    //数据库显示与关联
    //设置模型
    model = new QSqlTableModel(this);
    model->setTable("Finddata");//制定使用哪个表
    //把model放在view
    ui->tableView_show->setModel(model);
    //显示model里的数据
    model->select();
    //设置view中的数据库
    ui->tableView_show->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView_show->setColumnWidth(0,100);
    ui->tableView_show->setColumnWidth(1,100);
    ui->tableView_show->setColumnWidth(2,100);
    ui->tableView_show->setColumnWidth(3,100);
    ui->tableView_show->setColumnWidth(4,1000);
    ui->tableView_show->hideColumn(4);

    model->setHeaderData(0, Qt::Horizontal, "开始日期");
    model->setHeaderData(1, Qt::Horizontal, "开始时间");
    model->setHeaderData(2, Qt::Horizontal, "产能(pcs/h)");
    model->setHeaderData(3, Qt::Horizontal, "总异常时间(s)");
    model->setHeaderData(4, Qt::Horizontal, "异常详情");
//    //combox设置
//    for(int i=19;i<30;i++){
//        QString str=QString("%1").arg(i);
//        ui->comboBox_year->addItem(str);
//    }
//    for(int i=1;i<13;i++){
//        QString str;
//        if(i>9){
//            str=QString("%1").arg(i);
//        }else {
//            str=QString("0%1").arg(i);
//        }
//        ui->comboBox_month->addItem(str);
//    }
//    for(int i=1;i<32;i++){
//        QString str;
//        if(i>9){
//            str=QString("%1").arg(i);
//        }else {
//            str=QString("0%1").arg(i);
//        }
//        ui->comboBox_day->addItem(str);
//    }

    //子线程的调用
    myT1=new MyThread;
    myT2=new MyThread2;
    myT_read=new thread_readsql;
    thread1=new QThread(this);
    thread2=new QThread(this);
    thread_read=new QThread(this);
    myT1->moveToThread(thread1);
    myT2->moveToThread(thread2);
    myT_read->moveToThread(thread_read);
    connect(this,&MainWindow::startthread,myT1,&MyThread::clink);
    connect(this,&MainWindow::startthread2,myT2,&MyThread2::clink);
    connect(this,&MainWindow::startthread_read,myT_read,&thread_readsql::readdate);
    //产能数据传入主线程并关联显示函数
    connect(myT_read,&thread_readsql::signal_readdata,this,&MainWindow::read_capacity);
    //通讯异常处理
    connect(mytime_tcp_restart,&QTimer::timeout,this,&MainWindow::restart_tcp);
    connect(mytime_tcp_start,&QTimer::timeout,this,&MainWindow::start_tcp);
    connect(myT2,&MyThread2::mysignal_err,this, &MainWindow::judge_tcp);
    //统计界面按钮按下动作
    connect(ui->widgetChart->button0,&QPushButton::clicked,this,&MainWindow::middle_message0);
    connect(ui->widgetChart->button1,&QPushButton::clicked,this,&MainWindow::middle_message1);
    connect(ui->widgetChart->button2,&QPushButton::clicked,this,&MainWindow::middle_message2);
    connect(ui->widgetChart->button3,&QPushButton::clicked,this,&MainWindow::middle_message3);
    connect(ui->widgetChart->button4,&QPushButton::clicked,this,&MainWindow::middle_message4);
    connect(ui->widgetChart->button5,&QPushButton::clicked,this,&MainWindow::middle_message5);
    connect(ui->widgetChart->button6,&QPushButton::clicked,this,&MainWindow::middle_message6);
    connect(ui->widgetChart->button7,&QPushButton::clicked,this,&MainWindow::middle_message7);
    connect(ui->widgetChart->button8,&QPushButton::clicked,this,&MainWindow::middle_message8);
    connect(ui->widgetChart->button9,&QPushButton::clicked,this,&MainWindow::middle_message9);
    connect(ui->widgetChart->button10,&QPushButton::clicked,this,&MainWindow::middle_message10);
    connect(ui->widgetChart->button11,&QPushButton::clicked,this,&MainWindow::middle_message11);
    connect(ui->widgetChart->button12,&QPushButton::clicked,this,&MainWindow::middle_message12);
    connect(ui->widgetChart->button13,&QPushButton::clicked,this,&MainWindow::middle_message13);
    connect(this,&MainWindow::middle_but,this,&MainWindow::popup_message);
    btn_message1=new QMessageBox(this);

    //统计界面查询
    connect(ui->widgetChart->button_find,&QPushButton::clicked,this,&MainWindow::btn_find);
    connect(ui->widgetChart->label_edit,&QLineEdit::editingFinished,this,&MainWindow::btn_find);
    connect(this,&MainWindow::chart_find,this,&MainWindow::btn_find);
    connect(ui->widgetChart->button_day,&QPushButton::clicked,
            [=]()
            {
                day_bool=true;
                night_bool=false;
                emit chart_find();
            }
            );
    connect(ui->widgetChart->button_night,&QPushButton::clicked,
            [=]()
            {
                day_bool=false;
                night_bool=true;
                emit chart_find();
            }
            );
    connect(this,&MainWindow::signal_find,this,&MainWindow::read_capacity);
}

MainWindow::~MainWindow()
{
    delete ui;
    //变量不允许被二次析构，否则程序报错
    //当未进入线程时，关闭线程回出现二次析构情况
    if(T_1)
    colsethread1();
    if(T_2)
    colsethread2();
    if(T_3)
    colsethread_read();

}

//通讯异常重启
void MainWindow::judge_tcp(){
    mytime_tcp_restart->stop();
    mytime_tcp_restart->start(10000);
}
void MainWindow::restart_tcp(){
    on_button_connect2_clicked();
    mytime_tcp_start->start(3000);
}
void MainWindow::start_tcp(){
    on_button_connect2_clicked();
    mytime_tcp_start->stop();
}

//产能统计展示
void MainWindow::read_capacity(int year, int month, int day, int hour,double capacity,
                               int abnormal_alltime){

    //创建时间字符"%1-%2-%3 %4:00"
     QString str_date_add1=QString("");
     QString str_date_add2=QString("");
    if(month>=10){
        str_date_add1+=QString("%1-%2-").arg(year).arg(month);
        str_date_add2+=QString("%1-%2-").arg(year).arg(month);
    }else {
        str_date_add1+=QString("%1-0%2-").arg(year).arg(month);
        str_date_add2+=QString("%1-0%2-").arg(year).arg(month);
    }
    if(day>=10){
        str_date_add1+=QString("%1$").arg(day);
        str_date_add2+=QString("%1$").arg(day);
    }else {
        str_date_add1+=QString("0%1$").arg(day);
        str_date_add2+=QString("0%1$").arg(day);
    }
    if(hour>=10){
        str_date_add1+=QString("%1:00").arg(hour);
        str_date_add2+=QString("%1:00").arg(hour);
    }else {
        str_date_add1+=QString("0%1:00").arg(hour);
        str_date_add2+=QString("0%1:00").arg(hour);
    }
    str_date_add1+=QString("#%1").arg(capacity);
    str_date_add2+=QString("#%1").arg(abnormal_alltime);
    QString str1=QString("%1").arg(capacity);

    //产能展示
    ui->capacity_number->display(str1);
    ui->capacity_label->setText(str1);
    ui->widgetChart->addData(str_date_add1);
    //报警展示
    ui->widgetChart2->addData(str_date_add2);
}

void MainWindow::on_button_statis_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_button_analyze_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_button_warning_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_button_real_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_putton_set_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

//连接数据库
void MainWindow::sqlcreate(){
    //添加MySql数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    //连接数据库
    db.setHostName("127.0.0.1"); //数据库服务器IP
    db.setUserName("root"); //数据库用户名
    db.setPassword("123456"); //密码
    db.setDatabaseName("sys"); //使用哪个数据库


    //打开数据库
    if( !db.open() ) //数据库打开失败
    {
        QMessageBox::warning(this, "错误", db.lastError().text());
        qDebug()<<"dakaicuo";
        return;
    }
}

//界面初始化
void MainWindow::initwindows(){
    //开始界面
    on_putton_set_clicked();

    // 加载QSS样式
        CommonHelper::setStyle(":/image1/white.qss");


    //将界面设置为背景可填充
    //ui->widget_choose->setAutoFillBackground(true);

//    //将图片加载到QPalette
//    QPalette palette1 ;
//    palette1.setBrush(QPalette::Window,
//               QBrush(QPixmap(":/n/image1/beijng/backeground1.jpg").scaled(// 缩放背景图.
//                   ui->page5->size(),
//                   Qt::IgnoreAspectRatio,
//                   Qt::SmoothTransformation)));
//    QPalette palette2;
//    palette2.setBrush(QPalette::Window,
//               QBrush(QPixmap(":/n/image1/beijng/backeground1.jpg").scaled(// 缩放背景图.
//                   ui->page5->size(),
//                   Qt::IgnoreAspectRatio,
//                   Qt::SmoothTransformation)));

//    this->setPalette(palette1);
//    ui->widget_choose->setPalette(palette2);

//    //按钮风格设置
//    ui->widget_choose->setStyleSheet("QPushButton{background-color: rgb(80, 250, 255);}"
//                                  "QPushButton:hover{background-color: rgb(160, 250, 255);}"
//                                  "QPushButton:pressed{background-color: rgb(120, 250, 255);}");

//    ui->page5->setStyleSheet("QPushButton{background-color: rgb(220, 250, 255);}"
//                             "QPushButton:hover{background-color: rgb(192, 192, 192);}"
//                             "QPushButton:pressed{background-color: rgb(255, 255, 255);}");
}
void MainWindow::on_button_connect1_clicked()
{
    if(ui->button_connect1->text()=="连接1"){
        T_1=true;
        //启动线程，但没有启动线程处理函数
        qDebug()<<"线程调用开始";
        thread1
                ->start();
        myT1->stopit(false);

        //不能直接调用线程处理函数，直接调用导致线程处理函数和主线程在同一线程
        //myT->timeout();

        //只能通过signal-slot方式调用
        emit startthread();

        //更改按钮显示
        ui->button_connect1->setText("取消连接1");
         qDebug()<<"线程调用完成";
    }else{
       colsethread1();
       ui->button_connect1->setText("连接1");
    }
}
void MainWindow::colsethread1(){
    T_1=false;
    //改变子线程标志位，不让其死循环
    myT1->stopit(true);
    //关闭线程，此种方式比较温柔不会造成内存问题，要等待线程执行完成后退出
    thread1->quit();
    thread1->wait();
    //myT1->~MyThread();
}

void MainWindow::on_button_connect2_clicked()
{
    if(ui->button_connect2->text()=="连接2"){
        T_2=true;
        //通讯异常定时器开启
        mytime_tcp_restart->start(600000);
        //启动线程，但没有启动线程处理函数
        qDebug()<<"线程2调用开始";
        thread2->start();
        myT2->stopit(false);

        //不能直接调用线程处理函数，直接调用导致线程处理函数和主线程在同一线程
        //myT->timeout();

        //只能通过signal-slot方式调用
        emit startthread2();

        //更改按钮显示
        ui->button_connect2->setText("取消连接1");
         qDebug()<<"线程2调用完成";
    }else{
       colsethread2();
       ui->button_connect2->setText("连接2");
       mytime_tcp_restart->stop();
    }
}
void MainWindow::colsethread2(){
    T_2=false;
    //改变子线程标志位，不让其死循环
    myT2->stopit(true);
    //关闭线程，此种方式比较温柔不会造成内存问题，要等待线程执行完成后退出
    thread2->quit();
    thread2->wait();
   // myT2->~MyThread2();
}

void MainWindow::on_button_select1_clicked()
{
    T_3=true;
    //启动线程，但没有启动线程处理函数
    qDebug()<<"线程3调用开始";
    thread_read->start();
    myT_read->stopit(false);

    //只能通过signal-slot方式调用
    emit startthread_read();
}
void MainWindow::colsethread_read(){
    T_3=false;
    //改变子线程标志位，不让其死循环
    myT_read->stopit(true);
    //关闭线程，此种方式比较温柔不会造成内存问题，要等待线程执行完成后退出
    thread_read->quit();
    thread_read->wait();
    qDebug()<<"线程3关闭";
}
void MainWindow::on_button_thread3_close_clicked()
{
    colsethread_read();
}

//统计页面按钮动作
void MainWindow::on_ckShowHLine_stateChanged(int arg1)
{
    bool show = (arg1 != 0);
    ui->widgetChart->setShowHLine(show);
}

void MainWindow::on_ckShowPoint_stateChanged(int arg1)
{
    bool show = (arg1 != 0);
    ui->widgetChart->setShowPoint(show);
}

void MainWindow::on_ckShowPointBg_stateChanged(int arg1)
{
    bool show = (arg1 != 0);
    ui->widgetChart->setShowPointBg(show);
}

void MainWindow::on_btnClearData_clicked()
{
    ui->widgetChart->clearData();
}

void MainWindow::on_ckShowHLine_2_stateChanged(int arg1)
{
    bool show = (arg1 != 0);
    ui->widgetChart2->setShowHLine(show);
}

void MainWindow::on_ckShowPoint_2_stateChanged(int arg1)
{
    bool show = (arg1 != 0);
    ui->widgetChart2->setShowPoint(show);
}

void MainWindow::on_ckShowPointBg_2_stateChanged(int arg1)
{
    bool show = (arg1 != 0);
    ui->widgetChart2->setShowPointBg(show);
}

void MainWindow::on_btnClearData_2_clicked()
{
     ui->widgetChart2->clearData();
}

//按钮弹出窗口
void MainWindow::popup_message(QString str_key_find){
    //传入字符串解析
    QString str_start_date="";
    QString str_start_hour="";
    for(int i=0;i<14;i++){
        if(i<8)
            str_start_date+=str_key_find.at(i);
        if(i>8)
            str_start_hour+=str_key_find.at(i);
    }


    QString str1=QString("select * from Finddata where start_date='%1' and start_hour='%2' ")
            .arg(str_start_date).arg(str_start_hour);
    QString str_message=" ";
    QVector<QString> show_list;
    //关联已经连接的数据库
    QSqlDatabase::database();
    //查询表
    QSqlQuery query;
    query.exec(str1);
    //开始读取数据
    while (query.next()) {
        str_message=query.value(4).toString();
    }
    qDebug()<<str_message;
    //开始拆包工作
    show_list=string_trans(str_message);
    //重新对数据组包
    QString send_message="";
    if(!show_list.isEmpty()){
        for(int i=0;i<show_list.count();i++){
            send_message+=QString("%1%2").arg(show_list.at(i)).arg("\n");
        }
    }else {
        send_message+=QString("此时间段无通讯记录");
    }
    //将数据写入弹出框
    btn_message1->setText(send_message);
    QFont ft;
    ft.setPointSize(12);
    btn_message1->setFont(ft);
    btn_message1->open();

}

//字符串拆包
QVector<QString> MainWindow::string_trans(QString str_message){
    QVector<QString> show_list;

    //创建中间转化变量
    QString middle_str="";
    //创建位置计数
    int char_mark=0;
    int mark_where=0;

    show_list.clear();//容器复位操作
    while(char_mark<str_message.count()){
        if('$'!=str_message.at(char_mark)){
            if('#'==str_message.at(char_mark)){
               show_list.insert(mark_where ,middle_str);
                mark_where++;
                middle_str="";
                char_mark++;
            }else{
                middle_str+=str_message.at(char_mark);
                char_mark++;
            }
        }else {
            qDebug()<<"over";
            char_mark++;
            //qDebug()<<show_list.at(0)<<show_list.at(1)<<show_list.at(2);
        }
    }
    return  show_list;
}

//按钮按下中转
void MainWindow::middle_message0(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button0->text();
    QString str_key_find=time_package(str_date_find,str_time_find,0);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message1(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button1->text();
    QString str_key_find=time_package(str_date_find,str_time_find,1);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message2(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button2->text();
    QString str_key_find=time_package(str_date_find,str_time_find,2);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message3(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button3->text();
    QString str_key_find=time_package(str_date_find,str_time_find,3);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message4(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button4->text();
    QString str_key_find=time_package(str_date_find,str_time_find,4);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message5(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button5->text();
    QString str_key_find=time_package(str_date_find,str_time_find,5);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message6(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button6->text();
    QString str_key_find=time_package(str_date_find,str_time_find,6);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message7(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button7->text();
    QString str_key_find=time_package(str_date_find,str_time_find,7);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message8(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button8->text();
    QString str_key_find=time_package(str_date_find,str_time_find,8);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message9(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button9->text();
    QString str_key_find=time_package(str_date_find,str_time_find,9);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message10(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button10->text();
    QString str_key_find=time_package(str_date_find,str_time_find,10);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message11(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button11->text();
    QString str_key_find=time_package(str_date_find,str_time_find,11);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message12(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button12->text();
    QString str_key_find=time_package(str_date_find,str_time_find,12);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
void MainWindow::middle_message13(){
    QString str_date_find=ui->widgetChart->label_edit->text();
    QString str_time_find=ui->widgetChart->button13->text();
    QString str_key_find=time_package(str_date_find,str_time_find,13);
    qDebug()<<str_key_find;
    emit middle_but(str_key_find);
}
//隔天时间转换
QString MainWindow::time_package(QString str_date_find,QString str_time_find,int btn){
    QString str_hour="";
    QString str_day="";
    str_hour+=str_time_find.at(0);
    str_hour+=str_time_find.at(1);
    str_day+=str_date_find.at(6);
    str_day+=str_date_find.at(7);
    int hour=str_hour.toInt();
    int day=str_day.toInt();

    QString str_day_end;
    if(hour>=24-btn){
        day=day-1;
        if(day<10){
            str_day_end=QString("0%1").arg(day);
            str_date_find.replace(6,2,"");
        }else {
            str_day_end=QString("%1").arg(day);
        }
        str_date_find.replace(6,2,str_day_end);
    }
    QString str_key_find=str_date_find+QString(" ")+str_time_find;
    return str_key_find;
}

//数据查询函数
void MainWindow::btn_find(){
    QString str_chart_date=ui->widgetChart->label_edit->text();
    QString str_chart_year="";
    QString str_chart_month="";
    QString str_chart_day="";
    int year;
    int month;
    int day;
    if(!str_chart_date.isEmpty()&&str_chart_date.at(2)=='-'){
        str_chart_year+=str_chart_date.at(0);
        str_chart_year+=str_chart_date.at(1);
        year=str_chart_year.toInt();
        str_chart_month+=str_chart_date.at(3);
        str_chart_month+=str_chart_date.at(4);
        month=str_chart_month.toInt();
        str_chart_day+=str_chart_date.at(6);
        str_chart_day+=str_chart_date.at(7);
        day=str_chart_day.toInt();
    }else {
        qDebug()<<"查询输入错误";
        return ;
    }

//开始数据查询与传送
    QString str_select=QString("select * from Savedata where year=%1 and month=%2  "
                               "and day=%3 ").arg(year).arg(month).arg(day);
    //关联已经连接的数据库
    QSqlDatabase::database();
    //查询表
    QSqlQuery query;
    query.exec(str_select);
    //开始读取数据
    int mark_count=0;
    while (query.next()) {

        int hour;
        double capacity;
        int abnormal_alltime;
        if(day_bool&&mark_count<14){
            hour=query.value(3).toInt();
            capacity=query.value(4).toDouble();
            abnormal_alltime=query.value(5).toInt();
            emit signal_find(year,month,day,hour,capacity,abnormal_alltime);
        }else if(night_bool&&mark_count>=10){
            hour=query.value(3).toInt();
            capacity=query.value(4).toDouble();
            abnormal_alltime=query.value(5).toInt();
            emit signal_find(year,month,day,hour,capacity,abnormal_alltime);
        }
        mark_count++;
    }
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    if(checked){
        ui->tableView_show->showColumn(4);
    }else {
        ui->tableView_show->hideColumn(4);
    }
}

void MainWindow::on_button_Find_sql_clicked()
{
    QString str;
    if(ui->button_Find_sql->text()=="查询"){

        QString condition=ui->lineEdit_date_sql->text();
        if(condition.count()==8&&condition.at(5)=='-'){
            str = QString("start_date = '%1' ").arg(condition);
            ui->button_Find_sql->setText("取消查询");
        }else {
            QMessageBox::about(this, "错误提示", "日期格式错误");
        }

    }
    else if(ui->button_Find_sql->text()=="取消查询"){
        str="";
        ui->button_Find_sql->setText("查询");
    }

    model->setFilter(str);
    model->select();
}

void MainWindow::on_lineEdit_date_sql_editingFinished()
{
    QString str;
    QString condition=ui->lineEdit_date_sql->text();
    if(condition.count()==8&&condition.at(5)=='-'){
        str = QString("start_date = '%1' ").arg(condition);
        ui->button_Find_sql->setText("取消查询");
    }else {
        QMessageBox::about(this, "错误提示", "日期格式错误");
    }
    model->setFilter(str);
    model->select();
}
