#ifndef CURVECHART_H
#define CURVECHART_H

/**
 * 简单曲线图表控件 作者:feiyangqingyun(QQ:517216493) 2016-10-23
 * 1:可设置间隔
 * 2:可设置标题
 * 3:可设置是否显示横线及坐标点
 * 4:可设置背景色文字颜色
 * 5:可设置范围值及x轴y轴步长
 * 6:暂时不支持最小值小于0的数据
 */

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT CurveChart : public QWidget
#else
class CurveChart : public QWidget
#endif

{
    Q_OBJECT
    Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue)
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue)
    Q_PROPERTY(double xStep READ getXStep WRITE setXStep)
    Q_PROPERTY(double yStep READ getYStep WRITE setYStep)

    Q_PROPERTY(double space READ getSpace WRITE setSpace)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(bool showHLine READ getShowHLine WRITE setShowHLine)
    Q_PROPERTY(bool showPoint READ getShowPoint WRITE setShowPoint)
    Q_PROPERTY(bool showPointBg READ getShowPointBg WRITE setShowPointBg)

    Q_PROPERTY(QColor bgColorStart READ getBgColorStart WRITE setBgColorStart)
    Q_PROPERTY(QColor bgColorEnd READ getBgColorEnd WRITE setBgColorEnd)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
    Q_PROPERTY(QColor pointColor READ getPointColor WRITE setPointColor)

public:
    explicit CurveChart(QWidget *parent = 0);
    void get_point( QVector<QString> list_date);
    void mov_button(int,QString);
    void but_show(QPushButton*,int,QString,QPointF);




protected:
    void paintEvent(QPaintEvent *);
    void drawBg(QPainter *painter);
    void drawBox(QPainter *painter);
    void drawText(QPainter *painter);
    void drawTitle(QPainter *painter);
    void drawPoint(QPainter *painter);
    void drawTextY(QPainter *painter, QVector<QString> list_date);

signals:
     void signal_chart(QVector<QPointF>);

private slots:
    void updateData();

    void check_find(bool);

public:
    bool write_text;                //可编辑操作

    QRectF pointRect;               //绘制数据区域
    double yStep;                   //y轴步长

    QPushButton *button0;
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QPushButton *button4;
    QPushButton *button5;
    QPushButton *button6;
    QPushButton *button7;
    QPushButton *button8;
    QPushButton *button9;
    QPushButton *button10;
    QPushButton *button11;
    QPushButton *button12;
    QPushButton *button13;

    QCheckBox *box1;
    QLineEdit *label_edit;
    QPushButton *button_find;
    QPushButton *button_day;
    QPushButton *button_night;

private:

    QVector<QString> listData;       //数据集合
    QString str_year;
    QVector<QString> list_date;     //时间数据
    QVector<double> list_capacity;

    double minValue;                //最小值
    double maxValue;                //最大值
    double xStep;                   //x轴步长


    double space;                   //间隔
    QString title;                  //标题
    bool showHLine;                 //是否显示横线
    bool showPoint;                 //是否显示坐标点
    bool showPointBg;               //是否显示坐标背景

    QColor bgColorStart;            //背景渐变开始颜色
    QColor bgColorEnd;              //背景渐变结束颜色
    QColor textColor;               //文字颜色
    QColor pointColor;              //坐标点颜色

public:
    double getMinValue()            const;
    double getMaxValue()            const;
    double getXStep()               const;
    double getYStep()               const;

    double getSpace()               const;
    QString getTitle()              const;
    bool getShowHLine()             const;
    bool getShowPoint()             const;
    bool getShowPointBg()           const;

    QColor getBgColorStart()        const;
    QColor getBgColorEnd()          const;
    QColor getTextColor()           const;
    QColor getPointColor()          const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:
    //添加和设置数据数据
    void addData(QString data);
    void setData(QVector<QString> data);
    void clearData();

    //设置范围值及步长
    void setMinValue(double minValue);
    void setMaxValue(double maxValue);
    void setXStep(double xStep);
    void setYStep(double yStep);

    //设置间隔
    void setSpace(double space);
    //设置标题
    void setTitle(const QString &title);
    //设置显示横线
    void setShowHLine(bool showHLine);
    //设置显示坐标点
    void setShowPoint(bool showPoint);
    //设置显示坐标背景
    void setShowPointBg(bool showPointBg);

    //设置颜色
    void setBgColorStart(const QColor &bgColorStart);
    void setBgColorEnd(const QColor &bgColorEnd);
    void setTextColor(const QColor &textColor);
    void setPointColor(const QColor &pointColor);
};

#endif // CURVECHART_H
