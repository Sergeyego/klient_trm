#ifndef OVEN_H
#define OVEN_H

#include <QWidget>
#include <QMessageBox>
#include <QtSql>
#include <QSqlError>
#include <QLayout>
#include <QTableView>
#include <QGroupBox>
#include <QTimer>
#include <QDebug>
#include <QScrollBar>
#include <QHeaderView>
#include "widgets.h"
#include "inputdialog.h"
#include "tablemodel.h"
#include "sqlengine.h"
#include "modelchannel.h"
#include "delegate.h"
#include "modbusthread.h"

namespace Ui {
class Oven;
}

class Oven : public QWidget
{
    Q_OBJECT
public:
    explicit Oven(int id, QWidget *parent = 0);
    ~Oven();
    bool writeUst(double val);
    void clear();
    QString getHost();
    QString getName();
    QDateTime getCurrentDateTime();
    int getId();
    int getAddr();
    int getTcpPort();
    double getPwrCool();
    void startChange(int t, double ust, double valv);

public slots:
    void lockUst(bool b);
    void startRead();
    void refreshChannel();
    void plot();

private:
    Ui::Oven *ui;
    ModelChannel *mChannel;
    QVector<ModelChannel*> channel;
    int tN, ustN;
    QMessageBox* errMessageBox;
    TableModel *modelOven;
    int id_oven;
    void loadParams();
    Reader *reader;
    int currentX;
    bool isFirst;
    QDateTime currentDateTime;
    Writer *writerValve;
    
signals:
    void sigRead(int t);
    void sigNewTemp(double temp);
    void sigNewOut(double out);
    void error(QString message);
    void sigNewRun(bool b);
    
private slots:
    void showMessage(QString mess);
    void writeManualUst();
    void readFinished(dataOven d);
    void newDateTime(QDateTime t);
    
};

#endif // OVEN_H
