#ifndef SQLENGINE_H
#define SQLENGINE_H

#include <QObject>
#include <QtSql>
#include "tablemodel.h"
#include <QMessageBox>
#include <QDebug>

typedef QVector<QVariant> sqlParams;

class SqlEngine : public QObject
{
    Q_OBJECT
public:
    explicit SqlEngine(QObject *parent = 0);
    static bool executeQuery(QString query);
    static bool executeQuery(QString query, sqlParams params);
    static bool executeQuery(QString query, TableModel *result);
    static bool executeQuery(QString query, sqlParams params, TableModel *result);

signals:

public slots:

};

#endif // SQLENGINE_H
