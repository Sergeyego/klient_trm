#ifndef MODBUSTHREAD_H
#define MODBUSTHREAD_H

#include <QDebug>
#include <QTime>
#include <string.h>
#include <QtSerialBus>
#include <QModbusDataUnit>

struct dataChannel{
    double val;
    double ust;
    double out;
    bool run;
    bool ok;
};

typedef QVector<dataChannel> dataOven;

class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(QString host, int port, int adr, int count, QObject *parent = 0);

signals:
    void newValues(dataOven);
    void newTime(QDateTime);
    void sigErr(QString err);

public slots:
    void startRead();

private slots:
    void sendRequest();
    void readReady();

private:
    QString ip;
    int tcpPort;
    int chCount;
    int address;
    QModbusTcpClient *modbusDeviceRead;

};

class Writer : public QObject
{
    Q_OBJECT
public:
    explicit Writer(QString host, int port, int adr, int regnum, QObject *parent = 0);

signals:
    void sigErr(QString err);

public slots:
    void write(double ust);

private slots:
    void sendRequest();

private:
    QString ip;
    int tcpPort;
    int address;
    int regn;
    double currentUst;
    QModbusTcpClient *modbusDeviceWrite;
};

#endif // MODBUSTHREAD_H
