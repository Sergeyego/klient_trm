#include "tablemodel.h"

TableModel::TableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    cCount=0;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row()>=rowCount() || index.column()>=columnCount())
        return QVariant();
    QVariant val=modelData[index.row()][index.column()];
    if (role==Qt::EditRole){
        return val;
    }
    if (role==Qt::DisplayRole){
        if (val.type()==QVariant::Double){
            return val.isNull() ? QVariant() : QLocale().toString(val.toDouble(),'f',1);
        } else {
            return val;
        }
    }
    if (role==Qt::TextAlignmentRole && (val.type()==QVariant::Int || val.type()==QVariant::Double)){
        return int(Qt::AlignRight | Qt::AlignVCenter);
    }
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=false;
    if (role==Qt::EditRole){
        if (index.isValid() && index.row()<rowCount() && index.column()<columnCount()){
            modelData[index.row()][index.column()]=value;
            emit dataChanged(index,index);
            ok=true;
        }
    }
    return ok;
}

void TableModel::setColumnCount(int count)
{
    if (count>=0){
        hHeader.resize(count);
        if (count>columnCount()){
            beginInsertColumns(QModelIndex(),columnCount(),count-1);
            for (int i=0; i<modelData.size(); i++){
                modelData[i].resize(count);
            }
            cCount=count;
            endInsertColumns();
        } else if (count<columnCount()) {
            beginRemoveColumns(QModelIndex(),count,columnCount()-1);
            for (int i=0; i<modelData.size(); i++){
                modelData[i].resize(count);
            }
            cCount=count;
            endRemoveColumns();
        }
    }
}

void TableModel::setRowCount(int count)
{
    if (count>=0){
        if (count>rowCount()){
            int pos=rowCount();
            beginInsertRows(QModelIndex(),pos,count-1);
            modelData.resize(count);
            for (int i=pos; i<modelData.size(); i++){
                modelData[i].resize(cCount);
            }
            endInsertRows();
        } else if (count<rowCount()) {
            beginRemoveRows(QModelIndex(),count,rowCount()-1);
            modelData.resize(count);
            endRemoveRows();
        }
    }
}

void TableModel::clear()
{
    typeRow t;
    t.resize(columnCount());
    modelData.fill(t);
    emit dataChanged(this->index(0,0),this->index(rowCount()-1,columnCount()-1));
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return modelData.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return cCount;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | Qt::ItemIsSelectable |Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
}

bool TableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role==Qt::EditRole && orientation==Qt::Horizontal && section<columnCount() && section>=0){
        hHeader[section]=value;
        emit headerDataChanged(orientation,section,section);
        return true;
    }
    return false;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role==Qt::DisplayRole && orientation==Qt::Horizontal && section<columnCount() && section>=0){
        QVariant val=hHeader.at(section);
        return val.isNull() ? QAbstractTableModel::headerData(section,orientation,role) : val;
    }
    return QAbstractTableModel::headerData(section,orientation,role);
}

