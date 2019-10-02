#ifndef PROG_H
#define PROG_H

#include <QWidget>
#include "oven.h"
#include <QModelIndex>
#include <QThread>
#include <QDateTime>
#include <QScrollBar>
#include "sqlengine.h"

namespace Ui {
class Prog;
}

class Prog : public QWidget
{
    Q_OBJECT
    
public:
    explicit Prog(Oven *o, int ival, bool is_manual=false, QWidget *parent = 0);
    ~Prog();
    
private:
    Ui::Prog *ui;
    Oven *oven;
    QTimer *timer;
    QTimer *timerRead;
    int tN;
    TableModel *modelPart;
    TableModel *modelProgType;
    TableModel *modelProg;
    TableModel *modelRab;
    TableModel *modelElDim;
    int id_part;
    int step;
    int tStep;
    double calcProc();
    int interval;
    int intervalRead;
    int stepCount();
    int stepTime(int st);
    double stepTemp(int st);
    bool isRunning;
    QDateTime beginDateTime;
    double getEndTime();
    void progFinish();
    bool waitFlg;
    void sqlWriteStart();
    void sqlWriteStop();
    int id_ow_rab;
    bool ovenRun;
    bool manual;
    double getValve();

private slots:
    void setStepCount();
    void proc();
    void viewPart();
    void viewProg();
    void updateProg(QModelIndex index);
    void setProg();
    void startProg();
    void stopProg();
    void lockWidgets(bool b);
    void updatePart();
    void calcPw(double pwr);
    void setOwenRun(bool b);
    void viewStat();
};

#endif // PROG_H
