#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mythread.h"
#include "mythread2.h"
#include "thread_readsql.h"
#include <QThread>
#include <QTimer>
#include <QMessageBox>
#include <QSqlTableModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initwindows();
    void colsethread1();
    void colsethread2();
    void colsethread_read();
    void sqlcreate();
    QVector<QString> string_trans(QString);
    QString time_package(QString ,QString,int);

signals:
    void startthread();//启动子线程的信号
    void startthread2();
    void startthread_read();

    //统计按钮中转信号
    void middle_but(QString);
    //统计查询信号
    void chart_find();
    void signal_find(int , int , int , int ,double, int );

private slots:
    void on_button_statis_clicked();

    void on_button_analyze_clicked();

    void on_button_warning_clicked();

    void on_button_real_clicked();

    void on_putton_set_clicked();

    void on_button_connect1_clicked();

    void on_button_connect2_clicked();

    void on_button_select1_clicked();

    //产能展示
    void read_capacity(int , int , int , int ,double, int );

    //关闭线程3
    void on_button_thread3_close_clicked();

    //通讯异常处理
    void restart_tcp();

    void start_tcp();

    void judge_tcp();

    //点击按钮弹出窗口
    void middle_message0();
    void middle_message1();
    void middle_message2();
    void middle_message3();
    void middle_message4();
    void middle_message5();
    void middle_message6();
    void middle_message7();
    void middle_message8();
    void middle_message9();
    void middle_message10();
    void middle_message11();
    void middle_message12();
    void middle_message13();
    void popup_message(QString);
    void btn_find();

    //统计图设置
    void on_ckShowHLine_stateChanged(int arg1);

    void on_ckShowPoint_stateChanged(int arg1);

    void on_ckShowPointBg_stateChanged(int arg1);

    void on_btnClearData_clicked();

    void on_ckShowHLine_2_stateChanged(int arg1);

    void on_ckShowPoint_2_stateChanged(int arg1);

    void on_ckShowPointBg_2_stateChanged(int arg1);

    void on_btnClearData_2_clicked();

    void on_checkBox_clicked(bool checked);

    void on_button_Find_sql_clicked();

    void on_lineEdit_date_sql_editingFinished();

private:
    Ui::MainWindow *ui;
    QThread* thread1;
    QThread* thread2;
    QThread* thread_read;
    MyThread* myT1;
    MyThread2 * myT2;
    thread_readsql* myT_read;
    bool T_1=false;
    bool T_2=false;
    bool T_3=false;
    QTimer *mytime_tcp_restart;
    QTimer *mytime_tcp_start;

    //QMessageBox定义
    QMessageBox *btn_message1;

    //判定白夜班数据
    bool day_bool;
    bool night_bool;
    //数据库模型
    QSqlTableModel *model;
};

#endif // MAINWINDOW_H
