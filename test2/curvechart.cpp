#pragma execution_character_set("utf-8")

#include "curvechart.h"
#include "qpainter.h"
#include "qdebug.h"



CurveChart::CurveChart(QWidget *parent) : QWidget(parent)
{
    minValue = 0;
    maxValue = 1000;
    xStep = 100;
    yStep = 50;

    space = 40;
    title = "曲线图";
    showHLine = true;
    showPoint = true;
    showPointBg = true;

    bgColorStart = QColor(79, 79, 79);
    bgColorEnd = QColor(51, 51, 51);
    textColor = QColor(250, 250, 250);
    pointColor = QColor(38, 114, 179);

    //按钮分配内存
    button0=new QPushButton(this);
    button1=new QPushButton(this);
    button2=new QPushButton(this);
    button3=new QPushButton(this);
    button4=new QPushButton(this);
    button5=new QPushButton(this);
    button6=new QPushButton(this);
    button7=new QPushButton(this);
    button8=new QPushButton(this);
    button9=new QPushButton(this);
    button10=new QPushButton(this);
    button11=new QPushButton(this);
    button12=new QPushButton(this);
    button13=new QPushButton(this);
    button_find=new QPushButton(this);
    button_day=new QPushButton(this);
    button_night=new QPushButton(this);
    //将按钮隐藏
    button0->hide();
    button1->hide();
    button2->hide();
    button3->hide();
    button4->hide();
    button5->hide();
    button6->hide();
    button7->hide();
    button8->hide();
    button9->hide();
    button10->hide();
    button11->hide();
    button12->hide();
    button13->hide();
    button_day->hide();
    button_night->hide();


    //可输入判定
    box1=new QCheckBox(this);
    box1->move(200,10);
    connect(box1,&QCheckBox::toggled,this,&CurveChart::check_find);

    label_edit=new QLineEdit(this);
    label_edit->setMaximumSize(120,40);
    //设置字号
    QFont ft;
    ft.setPointSize(18);
    label_edit->setFont(ft);
    //设置颜色
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    label_edit->setPalette(pa);
    label_edit->move(0,0);
    label_edit->setInputMask("00-00-00");

    //设置查询按钮
    button_find->move(130,5);
    button_find->setText("查 询");
    button_find->setMaximumSize(60,50);
    //白夜班产能按钮设置
    button_day->move(194,5);
    button_day->setText("白");
    button_day->setMaximumSize(20,50);
    button_night->move(214,5);
    button_night->setText("夜");
    button_night->setMaximumSize(20,50);
    //按钮触发设置
    connect(button_find,&QPushButton::clicked,
            [=](){
                 if(button_find->text()=="查 询"){
                     button_day->show();
                     button_night->show();
                     button_find->setText("取消查询");
                     box1->move(240,10);
                 }else if(button_find->text()=="取消查询"){
                     button_day->hide();
                     button_night->hide();
                     button_find->setText("查 询");
                     box1->move(200,10);
                 }
                });
}
//可编辑判定
void CurveChart::check_find(bool check){
    write_text=check;
}

void CurveChart::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景
    drawBg(&painter);
    //绘制盒子
    drawBox(&painter);
    //绘制文字
    drawText(&painter);
    //绘制标题
    drawTitle(&painter);
    //绘制数据点
    drawPoint(&painter);
}
//绘制背景
void CurveChart::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    //线性渐变
    QLinearGradient topGradient(rect().topLeft(), rect().bottomLeft());
    topGradient.setColorAt(0.0, bgColorStart);
    topGradient.setColorAt(1.0, bgColorEnd);
    painter->setBrush(topGradient);
    painter->drawRect(rect());
    painter->restore();
}

void CurveChart::drawBox(QPainter *painter)
{
    painter->save();

    QPointF topLeftPot(space, space);
    QPointF bottomRightPot(width() - space / 2, height() - space / 2);
    painter->setPen(textColor);
    painter->setBrush(Qt::NoBrush);

    pointRect = QRectF(topLeftPot, bottomRightPot);
    painter->drawRect(pointRect);

    //绘制横线
    if (showHLine) {
        QPen pen(textColor, 1, Qt::DotLine);
        painter->setPen(pen);

        int step = (maxValue - minValue) / xStep;
        double increment = (double)pointRect.height() / step;
        double startY = pointRect.topLeft().y();

        for (int i = 0; i < step - 1; i++) {
            startY += increment;
            QPointF leftPot(pointRect.topLeft().x(), startY);
            QPointF rightPot(pointRect.topRight().x(), startY);
            painter->drawLine(leftPot, rightPot);
        }
    }

    painter->restore();
}

void CurveChart::drawText(QPainter *painter)
{
    painter->save();

    painter->setPen(textColor);
    painter->setBrush(Qt::NoBrush);

    int step = (maxValue - minValue) / xStep;
    int value = maxValue;
    double increment = (double)pointRect.height() / step;
    double startY = pointRect.topLeft().y();
    QString strValue;

    for (int i = 0; i <= step; i++) {
        strValue = QString("%1").arg(value);
        double textWidth = fontMetrics().width(strValue);
        double textHeight = fontMetrics().height();
        QPointF textPot(pointRect.topLeft().x() - 5 - textWidth, startY + textHeight / 2);
        painter->drawText(textPot, strValue);
        value -= xStep;
        startY += increment;
    }

    painter->restore();
}

void CurveChart::drawTextY(QPainter *painter, QVector<QString> list_date){
    painter->save();

    painter->setPen(textColor);
    painter->setBrush(Qt::NoBrush);

    double startY = pointRect.bottomRight().y();
    double startX = pointRect.topLeft().x();
    QString strValue;


    for (int i = 0; i < list_date.count(); i++) {
        strValue = QString("%1").arg(list_date.at(i));
        double textWidth = fontMetrics().width(strValue);
        double textHeight = fontMetrics().height();
        QPointF textPot(startX  - textWidth/2, startY +5+ textHeight);
        painter->drawText(textPot, strValue);
        startX += yStep;
    }
}

void CurveChart::drawTitle(QPainter *painter)
{
    painter->save();

    painter->setPen(textColor);
    painter->setBrush(Qt::NoBrush);

    double titleX = (double)width() / 2;
    double titleY = space;
    double textWidth = fontMetrics().width(title);
    double textHeight = fontMetrics().height();

    //标题加粗显示
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(10);
    painter->setFont(titleFont);

    QPointF textPot(titleX - textWidth / 2, titleY / 2 + textHeight / 2);
    painter->drawText(textPot, title);

    painter->restore();
}

void CurveChart::drawPoint(QPainter *painter)
{
    painter->save();

    double startX = pointRect.topRight().x();
    QVector<QPointF> points;

    yStep=pointRect.width()/13;
    if (showPointBg) {
        points.push_back(QPointF(startX, pointRect.bottomRight().y()));
    }

//数据容器的转化
    list_date.clear();
    list_capacity.clear();

    for (int i = 0; i < listData.count(); i++){
        str_year="";
        QString str1="";
        QString str2="";
        double doub_mid=0;
        QString middle_str=listData.at(i);
        //创建位置标志位
        int char_mark=0;
        //变量数据转换
        bool mark_bool=true;
        bool mark_year=true;
        while(char_mark<middle_str.count()){
            if('$' ==middle_str.at(char_mark)){
                mark_year=false;
                char_mark++;
            }
            if('#' ==middle_str.at(char_mark)){
                mark_bool=false;
                char_mark++;
            }
            if(mark_year){
                str_year+=middle_str.at(char_mark);
            }
            if(mark_bool&&(!mark_year)){
                str1+=middle_str.at(char_mark);
            }
            if(!mark_bool){
                str2+=middle_str.at(char_mark);
            }
            char_mark++;
        }
        doub_mid=str2.toDouble();
        list_date.append(str1);
        list_capacity.append(doub_mid);
    }
    //drawTextY(painter,list_date);
    //移动绘制按钮
     get_point(list_date);
     //显示日期
     QString str4=QString("%1").arg(str_year);
     if(!write_text)
     label_edit->setText(str4);
    for (int i = 0; i < list_capacity.count(); i++) {
        QPointF dataPot(startX, pointRect.bottomRight().y() - list_capacity.at(i)
                        * (pointRect.height() / (maxValue - minValue)));
        QPointF textPot(startX-8, pointRect.bottomRight().y()-10- list_capacity.at(i)
                        * (pointRect.height() / (maxValue - minValue)));
        points.push_back(dataPot);
        QString str=QString("%1").arg(list_capacity.at(i));
        painter->drawText(textPot, str);
        startX -= yStep;
    }

    if(list_capacity.count()>14)
    points.remove(13,1);

    if (showPointBg) {
        points.push_back(QPointF(startX, pointRect.bottomRight().y()));
    }

    //如果只有两个数据点不需要处理
    if (showPointBg && points.count() <= 2) {
        painter->restore();
        return;
    }

    painter->setPen(pointColor);
    //定义画笔
    QPen pen;
    pen.setWidth(3); //设置线宽
    pen.setColor(pointColor); //设置颜色

    //绘制背景及多段线
    if (showPointBg) {
        painter->setBrush(QColor(pointColor.red(), pointColor.green(), pointColor.blue(), 150));
        painter->drawPolygon(QPolygonF(points));
    } else {
        //把画笔交给画家
        painter->setPen(pen);

        painter->setBrush(Qt::NoBrush);
        painter->drawPolyline(QPolygonF(points));
    }

    //绘制坐标点
    if (showPoint) {
        for (int i = 0; i < points.count(); i++) {
            QPointF dataPot = points.at(i);
            painter->setBrush(pointColor);
            painter->drawEllipse(dataPot, 3, 3);
        }
    }

    painter->restore();
}

void CurveChart::updateData()
{
    int count = pointRect.width() / yStep+2;
    int i = listData.count() - count;

    if (i > 0) {
        listData.remove(count, i);
    }

    update();
}

double CurveChart::getMinValue() const
{
    return this->minValue;
}

double CurveChart::getMaxValue() const
{
    return this->maxValue;
}

double CurveChart::getXStep() const
{
    return this->xStep;
}

double CurveChart::getYStep() const
{
    return this->yStep;
}

double CurveChart::getSpace() const
{
    return this->space;
}

QString CurveChart::getTitle() const
{
    return this->title;
}

bool CurveChart::getShowHLine() const
{
    return this->showHLine;
}

bool CurveChart::getShowPoint() const
{
    return this->showPoint;
}

bool CurveChart::getShowPointBg() const
{
    return this->showPointBg;
}

QColor CurveChart::getBgColorStart() const
{
    return this->bgColorStart;
}

QColor CurveChart::getBgColorEnd() const
{
    return this->bgColorEnd;
}

QColor CurveChart::getTextColor() const
{
    return this->textColor;
}

QColor CurveChart::getPointColor() const
{
    return this->pointColor;
}

QSize CurveChart::sizeHint() const
{
    return QSize(500, 300);
}

QSize CurveChart::minimumSizeHint() const
{
    return QSize(200, 70);
}

void CurveChart::addData(QString data)
{
    listData.push_front(data);
    updateData();
}
//加载数据
void CurveChart::setData(QVector<QString> data)
{
    listData = data;
    updateData();
}

void CurveChart::clearData()
{
    listData.clear();
    update();
}

void CurveChart::setMinValue(double minValue)
{
    if (this->minValue != minValue) {
        this->minValue = minValue;
        update();
    }
}

void CurveChart::setMaxValue(double maxValue)
{
    if (this->maxValue != maxValue) {
        this->maxValue = maxValue;
        update();
    }
}

void CurveChart::setXStep(double xStep)
{
    if (this->xStep != xStep) {
        this->xStep = xStep;
        update();
    }
}

void CurveChart::setYStep(double yStep)
{
    if (this->yStep != yStep) {
        this->yStep = yStep;
        update();
    }
}

void CurveChart::setSpace(double space)
{
    if (this->space != space) {
        this->space = space;
        update();
    }
}

void CurveChart::setTitle(const QString &title)
{
    if (this->title != title) {
        this->title = title;
        update();
    }
}

void CurveChart::setShowHLine(bool showHLine)
{
    if (this->showHLine != showHLine) {
        this->showHLine = showHLine;
        update();
    }
}

void CurveChart::setShowPoint(bool showPoint)
{
    if (this->showPoint != showPoint) {
        this->showPoint = showPoint;
        update();
    }
}

void CurveChart::setShowPointBg(bool showPointBg)
{
    if (this->showPointBg != showPointBg) {
        this->showPointBg = showPointBg;
        update();
    }
}

void CurveChart::setBgColorStart(const QColor &bgColorStart)
{
    if (this->bgColorStart != bgColorStart) {
        this->bgColorStart = bgColorStart;
        update();
    }
}

void CurveChart::setBgColorEnd(const QColor &bgColorEnd)
{
    if (this->bgColorEnd != bgColorEnd) {
        this->bgColorEnd = bgColorEnd;
        update();
    }
}

void CurveChart::setTextColor(const QColor &textColor)
{
    if (this->textColor != textColor) {
        this->textColor = textColor;
        update();
    }
}

void CurveChart::setPointColor(const QColor &pointColor)
{
    if (this->pointColor != pointColor) {
        this->pointColor = pointColor;
        update();
    }
}

//绘制X坐标的时间值，并且显示为按钮
void CurveChart::get_point( QVector<QString> list_date){
    for(int i=0;i<list_date.count();i++){
        mov_button(i,list_date.at(i));
    }

}
//移动单个并显示按钮
void CurveChart::mov_button(int number,QString str){
    double startY = pointRect.bottomRight().y();
    double startX = pointRect.topRight().x();
    startX=startX-yStep*number;
    QPointF butPot(startX-36, startY );

    switch (number) {
    case 0:
    {
        but_show(button0,number,str,butPot);
        break;
    }
    case 1:
    {
        but_show(button1,number,str,butPot);
        break;
    }
    case 2:
    {
        but_show(button2,number,str,butPot);
        break;
    }
    case 3:
    {
        but_show(button3,number,str,butPot);
        break;
    }
    case 4:
    {
        but_show(button4,number,str,butPot);
        break;
    }
    case 5:
    {
        but_show(button5,number,str,butPot);
        break;
    }
    case 6:
    {
        but_show(button6,number,str,butPot);
        break;
    }
    case 7:
    {
        but_show(button7,number,str,butPot);
        break;
    }
    case 8:
    {
        but_show(button8,number,str,butPot);
        break;
    }
    case 9:
    {
        but_show(button9,number,str,butPot);
        break;
    }
    case 10:
    {
        but_show(button10,number,str,butPot);
        break;
    }
    case 11:
    {
        but_show(button11,number,str,butPot);
        break;
    }
    case 12:
    {
        but_show(button12,number,str,butPot);
        break;
    }
    case 13:
    {
        but_show(button13,number,str,butPot);
        break;
    }
    default:
        break;
    }
}

//按钮显示
void CurveChart::but_show(QPushButton* button,int number,QString str,QPointF butPot){

    button->move(butPot.toPoint());
    button->setText(str);
    button->setMinimumSize(40,25);
    button->show();
    button->setFlat(true);
    update();
}
