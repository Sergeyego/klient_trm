#ifndef MODELCHANNEL_H
#define MODELCHANNEL_H

#include <QAbstractTableModel>
#include <QColor>
#include "sqlengine.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_point_data.h>
#include "cfgplot.h"
#include <qwt_symbol.h>

struct pnt{
    QVector<double> x;
    QVector<double> y;
};


class ModelChannel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ModelChannel(int id, QObject *parent = 0);
    ~ModelChannel();
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    void newVal(const QVector<double> &vals);
    QString getName();

private:
    int id_channel;
    int number;
    QString name;
    double kUst;
    bool is_main;
    double power;
    int address;
    int addressReg;
    QVector <QwtPlotCurve*> curves;
    QVector <pnt> qwtdata;
    QVector <double> currentVal;
    void reloadData();
    bool readOnly;
    QVector <double> regOut;

signals:
    void ustChanged(double ust);
    void nameChanged(QString name);

public slots:
    void refresh();
    void clear();
    void plot(double t);
    void setUst(double ust);
    bool getIsMain();
    double getPower();
    void setReadOnly(bool b);
};

#endif // MODELCHANNEL_H
