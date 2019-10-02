#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QSpacerItem>
#include "oven.h"
#include "cfgplot.h"
#include "prog.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWidget(int id_ov, int interval, bool isManual, QWidget *parent = 0);
    ~MainWidget();
    void closeEvent(QCloseEvent *e);
    
private:
    Ui::MainWidget *ui;
    Oven *oven;
    Prog *prog;
    CfgPlot *cfgPlot;
private slots:
    void quit();
};

#endif // MAINWIDGET_H
