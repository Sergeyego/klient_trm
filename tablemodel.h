#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QtSql>
#include <QMessageBox>

typedef QVector<QVariant> typeRow;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role=Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;

private:
    QVector<typeRow> modelData;
    int cCount;
    typeRow hHeader;

signals:

public slots:
    void setColumnCount(int count);
    void setRowCount(int count);
    void clear();
};

#endif // TABLEMODEL_H
