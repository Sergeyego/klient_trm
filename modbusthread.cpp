#include "modbusthread.h"

Reader::Reader(QString host, int port, int adr, int count, QObject *parent) :
    ip(host), tcpPort(port), address(adr), chCount(count), QObject(parent)
{
    qRegisterMetaType<dataOven>("dataOven");
    modbusDeviceRead = new QModbusTcpClient(this);
    modbusDeviceRead->setConnectionParameter(QModbusDevice::NetworkPortParameter, tcpPort);
    modbusDeviceRead->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);
    modbusDeviceRead->setTimeout(500);
    modbusDeviceRead->setNumberOfRetries(3);
    connect(modbusDeviceRead,SIGNAL(stateChanged(QModbusDevice::State)),this,SLOT(sendRequest()));
    connect(modbusDeviceRead, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        if (!((modbusDeviceRead->error()==QModbusDevice::ConnectionError) && (modbusDeviceRead->state()==QModbusDevice::ConnectedState))){
            emit sigErr(tr("Ошибка сервера терморегуляторов: ")+modbusDeviceRead->errorString());
        }
    });

}

void Reader::startRead()
{
    if (modbusDeviceRead->state()==QModbusDevice::ConnectedState){
        modbusDeviceRead->disconnectDevice();
    }
    if (!modbusDeviceRead->connectDevice()){
        emit sigErr(tr("Ошибка подключения к серверу терморегуляторов. ")+modbusDeviceRead->errorString());
    }
}

void Reader::sendRequest()
{
    if (modbusDeviceRead->state()==QModbusDevice::ConnectedState) {
        int kvo=3+5*chCount;
        QModbusDataUnit readRequest(QModbusDataUnit::HoldingRegisters,8,kvo);
        if (auto reply = modbusDeviceRead->sendReadRequest(readRequest,address)){
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, &Reader::readReady);
            } else {
                delete reply;
            }
        }
    }/* else {
        qDebug()<<modbusDeviceRead->state();
    }*/
}

void Reader::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
            if (!reply)
                return;
        if (reply->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = reply->result();
            reply->deleteLater();
            int rc=unit.valueCount();
            int kvo=3+5*chCount;

            if (rc==kvo){
                QDateTime dateTime;
                uint32_t t;
                uint16_t reg[10];
                reg[0]=unit.value(0);
                reg[1]=unit.value(1);
                memcpy(((uint8_t*)&t)+0, reg+0, 2);
                memcpy(((uint8_t*)&t)+2, reg+1, 2);
                dateTime=QDateTime::fromTime_t(t);
                emit newTime(dateTime);
            }

            dataOven dov;
            for (int i=0; i<chCount; i++){
                dataChannel ch;
                int base= 3+i*5;
                if (rc==kvo){
                    int dec=(int16_t)unit.value(base+3);
                    bool ok=dec!=-1;
                    double znam = 10.0;
                    ch.ust=ok? ((int16_t)(unit.value(base+0)))/znam : 0.0;
                    ch.val=ok? ((int16_t)(unit.value(base+1)))/znam : 0.0;
                    ch.out=ok? ((int16_t)(unit.value(base+2)))/znam : 0.0;
                    ch.run=((int16_t)(unit.value(base+4)))==1 && ok;
                    ch.ok=ok;
                } else {
                    ch.ust=0;
                    ch.val=0;
                    ch.out=0;
                    ch.run=false;
                    ch.ok=false;
                }
                dov.push_back(ch);
            }
            emit newValues(dov);
        } else {
            emit sigErr(tr("Ошибка сервера терморегуляторов: ")+reply->errorString());
        }
}


Writer::Writer(QString host, int port, int adr, int regnum, QObject *parent) :
    ip(host), tcpPort(port), address(adr), regn(regnum), QObject(parent)
{
    modbusDeviceWrite = new QModbusTcpClient(this);
    modbusDeviceWrite->setConnectionParameter(QModbusDevice::NetworkPortParameter, tcpPort);
    modbusDeviceWrite->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);
    modbusDeviceWrite->setTimeout(500);
    modbusDeviceWrite->setNumberOfRetries(3);
    connect(modbusDeviceWrite,SIGNAL(stateChanged(QModbusDevice::State)),this,SLOT(sendRequest()));
    connect(modbusDeviceWrite, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        if (!((modbusDeviceWrite->error()==QModbusDevice::ConnectionError) && (modbusDeviceWrite->state()==QModbusDevice::ConnectedState))){
            emit sigErr(tr("Ошибка сервера терморегуляторов: ")+modbusDeviceWrite->errorString());
        }
    });
}

void Writer::write(double ust)
{
    currentUst=ust;
    if (modbusDeviceWrite->state()==QModbusDevice::ConnectedState){
        modbusDeviceWrite->disconnectDevice();
    }
    if (!modbusDeviceWrite->connectDevice()){
        emit sigErr(tr("Ошибка подключения к серверу терморегуляторов. ")+modbusDeviceWrite->errorString());
    }
}

void Writer::sendRequest()
{
    if (modbusDeviceWrite->state()==QModbusDevice::ConnectedState) {
        QModbusDataUnit writeRequest(QModbusDataUnit::HoldingRegisters,regn,1);
        qint16 ust=currentUst*10.0;
        writeRequest.setValue(0,ust);
        modbusDeviceWrite->sendWriteRequest(writeRequest,address);
    }
}
