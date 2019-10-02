#include "modelchannel.h"

ModelChannel::ModelChannel(int id, QObject *parent) :
        QAbstractTableModel(parent), id_channel(id)
{
    curves.push_back(new QwtPlotCurve(tr("Темпер., C")));
    curves.push_back(new QwtPlotCurve(tr("Уставка, C")));
    curves.push_back(new QwtPlotCurve(tr("Выход, %")));

    readOnly=false;

    qwtdata.resize(curves.size());
    currentVal.resize(curves.size());
    for (int i=0; i<curves.size(); i++){
        QPen pen=curves[i]->pen();
        pen.setWidth(2);
        curves[i]->setPen(pen);
        curves[i]->attach(Plot::instance());
    }
    curves[2]->setVisible(false);
    refresh();
}

ModelChannel::~ModelChannel()
{
}

int ModelChannel::rowCount(const QModelIndex &parent) const
{
    return curves.size();
}

int ModelChannel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant ModelChannel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row()>=curves.size() || index.column()>=3) return QVariant();
    QVariant value;
    switch (role) {
        case Qt::DisplayRole:
            if (index.column()==0){
                value=curves.at(index.row())->title().text();
            } else if (index.column()==2){
                value=QLocale().toString(currentVal.at(index.row()),'f',1);
            }
        break;
        case Qt::EditRole:
            if (index.column()==1){
                value=curves.at(index.row())->pen().color();
            } else if (index.column()==2){
                value=currentVal.at(index.row());
            }
        break;
        case Qt::TextAlignmentRole:
            if (index.column()==2){
                value=int(Qt::AlignRight | Qt::AlignVCenter);
            } else {
                value=int(Qt::AlignLeft | Qt::AlignVCenter);
            }
        break;
        case Qt::CheckStateRole:
            if (index.column()==0){
                value= curves.at(index.row())->isVisible() ? Qt::Checked : Qt::Unchecked;
            }
        break;
        case Qt::BackgroundColorRole:
            if (index.column()==1){
                value=curves.at(index.row())->pen().color();
            }
        break;
        default:
            value=QVariant();
        break;
    }
    return value;
}

Qt::ItemFlags ModelChannel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;
    int col=index.column();
    switch (col) {
        case 0:
            flags=Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
        break;
        case 1:
            flags = Qt::ItemIsEditable | Qt::ItemIsEnabled;
        break;
        case 2:
            flags= (index.row()==1 && !readOnly) ? (Qt::ItemIsEditable | Qt::ItemIsEnabled) : (Qt::ItemIsEnabled);
        break;
        default:
            flags=Qt::ItemIsEnabled;
        break;
    }
    return flags;
}

bool ModelChannel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row()>=curves.size() || index.column()>=3) return false;
    bool ok=false;
    if (role==Qt::EditRole){
        if (index.column()==1){
            QPen pen=curves[index.row()]->pen();
            pen.setColor(QColor(value.toString()));
            curves[index.row()]->setPen(pen);
            curves[index.row()]->plot()->replot();
            ok=true;
        } else if (index.column()==2){
            if (index.row()==1){
                currentVal[index.row()]=value.toDouble();
                ok=true;
                //qDebug()<<value.toDouble();
                emit ustChanged(value.toDouble());
            }
        }
    }
    if (role==Qt::CheckStateRole){
        if (index.column()==0){
            curves[index.row()]->setVisible(value.toBool());
            curves[index.row()]->plot()->replot();
            ok=true;
        }
    }
    if (ok) emit dataChanged(index,index);
    return ok;
}

void ModelChannel::newVal(const QVector<double> &vals)
{
    if (vals.size()>=currentVal.size()){
        for (int i=0; i<currentVal.size(); i++){
            currentVal[i]=vals.at(i);
        }
        regOut.push_back(currentVal.at(2));
        emit dataChanged(this->index(0,2),this->index(this->rowCount()-1,2));
    }

}

QString ModelChannel::getName()
{
    return name;
}

void ModelChannel::reloadData()
{
    for (int i=0; i<curves.size(); i++){
        curves[i]->setSamples(qwtdata.at(i).x,qwtdata.at(i).y);
    }
}

void ModelChannel::refresh()
{
    QString query;
    sqlParams params;
    TableModel result;
    query="select id, number, nam, color_ust, color_val, k_ust, is_main, pwr "
          "from owens_trm_channel where id= ? ";
    params.push_back(id_channel);
    if (SqlEngine::executeQuery(query,params,&result)){
        if (result.rowCount()==1 && result.columnCount()==8){
            beginResetModel();
            number=result.data(result.index(0,1),Qt::EditRole).toInt();
            name=result.data(result.index(0,2),Qt::EditRole).toString();
            QStringList c_ust=result.data(result.index(0,3),Qt::EditRole).toString().split(":");
            QStringList c_val=result.data(result.index(0,4),Qt::EditRole).toString().split(":");
            if (c_ust.size()==3 && c_val.size()==3) {
                QVector<QColor> clr;
                clr.push_back(QColor(c_val.at(0).toInt(),c_val.at(1).toInt(),c_val.at(2).toInt()));
                clr.push_back(QColor(c_ust.at(0).toInt(),c_ust.at(1).toInt(),c_ust.at(2).toInt()));
                for (int i=0; i<clr.size(); i++){
                    QPen pen=curves[i]->pen();
                    pen.setColor(clr.at(i));
                    curves[i]->setPen(pen);
                }

            }
            kUst=result.data(result.index(0,5),Qt::EditRole).toDouble();
            is_main=result.data(result.index(0,6),Qt::EditRole).toBool();
            power=result.data(result.index(0,7),Qt::EditRole).toDouble();
            if (is_main) name+=tr(" <главный>");
            endResetModel();
            emit nameChanged(name);
        }
    }
}

void ModelChannel::clear()
{
    for (int i=0; i<qwtdata.size(); i++){
        qwtdata[i].x.clear();
        qwtdata[i].y.clear();
    }
    reloadData();
}

void ModelChannel::plot(double t)
{
    for (int i=0; i<qwtdata.size()-1; i++){
        qwtdata[i].x.push_back(t);
        qwtdata[i].y.push_back(currentVal.at(i));
    }
    double yout=0.0;
    for (int i=0; i<regOut.size(); i++){
        yout+=regOut.at(i);
    }
    if (regOut.size()) {
        yout/=regOut.size();
    }
    regOut.clear();
    qwtdata[2].x.push_back(t);
    qwtdata[2].y.push_back(yout);
    reloadData();
}

void ModelChannel::setUst(double ust)
{
    this->setData(this->index(1,2),ust*kUst,Qt::EditRole);
}

bool ModelChannel::getIsMain()
{
    return is_main;
}

double ModelChannel::getPower()
{
    return power;
}

void ModelChannel::setReadOnly(bool b)
{
    readOnly=b;
}
