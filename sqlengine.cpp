#include "sqlengine.h"

SqlEngine::SqlEngine(QObject *parent) :
    QObject(parent)
{
}

bool SqlEngine::executeQuery(QString query)
{
    sqlParams params;
    return executeQuery(query,params,NULL);
}

bool SqlEngine::executeQuery(QString query, sqlParams params)
{
    return executeQuery(query,params,NULL);
}

bool SqlEngine::executeQuery(QString query, TableModel *result)
{
    sqlParams params;
    return executeQuery(query,params,result);
}

bool SqlEngine::executeQuery(QString query, sqlParams params, TableModel *result)
{
    bool ok=false;

    int randInt;
    QString randomName;
    for(int i=0; i<5; i++){
        randInt = qrand()%('Z'-'A'+1)+'A';
        randomName.append(randInt);
    }
    //qDebug() << randomName;
    {
        QSqlDatabase db=QSqlDatabase::addDatabase("QPSQL",randomName);
        db.setDatabaseName("neo_rtx");
        db.setHostName("192.168.1.10");
        db.setPort(5432);
        db.setUserName("user");
        db.setPassword("szsm");
        if (db.open()){
            QSqlQuery qu(db);
            //qu.setForwardOnly(true);
            qu.prepare(query);
            for (int n=0; n<params.size(); n++){
                qu.addBindValue(params.at(n));
            }
            ok=qu.exec();
            if (ok && result){
                int n=qu.record().count();
                result->setRowCount(qu.size());
                result->setColumnCount(n);
                int i=0;
                while(qu.next()){
                    for (int j=0; j<n; j++){
                        result->setData(result->index(i,j),qu.value(j));
                    }
                    i++;
                }
            } else if (!ok) {
                if (result) result->setRowCount(0);
                QMessageBox::critical(NULL,"Error",qu.lastError().text(),QMessageBox::Ok);
            }
        } else {
            if (result) result->setRowCount(0);
            QMessageBox::critical(NULL,"Error",db.lastError().text(),QMessageBox::Ok);
        }
        if (db.isOpen()) db.close();
    }
    QSqlDatabase::removeDatabase(randomName);
    return ok;
}
