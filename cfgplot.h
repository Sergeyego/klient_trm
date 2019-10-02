#ifndef CFGPLOT_H
#define CFGPLOT_H

#include <QWidget>
#include <QTime>
#include <qwt_plot.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_grid.h>
#include <QPen>
#include <qwt_plot_picker.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_textlabel.h>

namespace Ui {
class CfgPlot;
}

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw();
    virtual QwtText label(double v) const;
    void setInterval(int sec);
    void setBaseTime(QDateTime time);
private:
    QDateTime baseTime;
    int interval;
};

class TimePicker: public QwtPlotPicker
{
public:
   TimePicker(int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas *canvas );
   QwtText trackerTextF(const QPointF &pos) const;
   void setInterval(int sec);
   void setBaseTime(QDateTime time);
private:
    QDateTime baseTime;
    int interval;

} ;

class Plot : public QwtPlot{
    Q_OBJECT
public:
    Plot(QWidget *parent = 0);
    static Plot* instance();
    void setInterval(int sec);
    void setbaseTime(QDateTime time);
private:
    static Plot* plot_instance;
    TimePicker *timePicker;
    TimeScaleDraw *timeScaleDraw;
public slots:
    void setXmax(int value);
};

class CfgPlot : public QWidget
{
    Q_OBJECT
public:
    explicit CfgPlot(QwtPlot *plot, QWidget *parent = 0);
    ~CfgPlot();
private:
    Ui::CfgPlot *ui;
    QwtPlot *qwtPlot;
public slots:
    void setNx(int value);
    void setNy(int value);
    void setYmax(int value);
    void setYmin(int value);
};

#endif // CFGPLOT_H
