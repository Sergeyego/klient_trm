#include "oven.h"
#include "ui_oven.h"

Oven::Oven(int id, QWidget *parent) :
    QWidget(parent), ui(new Ui::Oven), id_oven(id)
{
    ui->setupUi(this);
    isFirst=true;
    currentX=0;
    currentDateTime=QDateTime::currentDateTime();
    ui->lineEditUst->setValidator(new QDoubleValidator(this));
    modelOven = new TableModel(this);
    loadParams();

    reader = new Reader(this->getHost(),this->getTcpPort(),this->getAddr(),channel.size(),this);
    writerValve = new Writer(this->getHost(),this->getTcpPort(),this->getAddr(),10,this);

    errMessageBox = new QMessageBox(this);
    errMessageBox->setModal(false);
    errMessageBox->setWindowTitle(getName()+tr(": Ошибка"));
    errMessageBox->setIcon(QMessageBox::Critical);

    connect(this,SIGNAL(error(QString)),this,SLOT(showMessage(QString)));
    connect(ui->cmdUst,SIGNAL(clicked()),this,SLOT(writeManualUst()));
    connect(reader,SIGNAL(newValues(dataOven)),this,SLOT(readFinished(dataOven)));
    connect(reader,SIGNAL(newTime(QDateTime)),this,SLOT(newDateTime(QDateTime)));
    connect(reader,SIGNAL(sigErr(QString)),this,SLOT(showMessage(QString)));
}

Oven::~Oven()
{
    delete ui;
}

bool Oven::writeUst(double val)
{
    bool ok=true;
    foreach (ModelChannel *chn , channel) {
        chn->setUst(val);
    }
    return ok;
}

void Oven::plot()
{
    foreach (ModelChannel *chn , channel) {
        chn->plot(currentX);
    }
    emit sigRead(currentX);
}

void Oven::clear()
{
    foreach (ModelChannel *chn , channel) {
        chn->clear();
    }
    currentX=0;
}

QString Oven::getHost()
{
    return modelOven->rowCount()? modelOven->data(modelOven->index(0,2),Qt::EditRole).toString() : QString();
}

QString Oven::getName()
{
    return modelOven->rowCount()? modelOven->data(modelOven->index(0,1),Qt::EditRole).toString() : QString();
}

QDateTime Oven::getCurrentDateTime()
{
    return currentDateTime;
}

int Oven::getId()
{
    return modelOven->rowCount()? modelOven->data(modelOven->index(0,0),Qt::EditRole).toInt() : -1;
}

int Oven::getAddr()
{
    return modelOven->rowCount()? modelOven->data(modelOven->index(0,7),Qt::EditRole).toInt() : 0;
}

int Oven::getTcpPort()
{
    return modelOven->rowCount()? modelOven->data(modelOven->index(0,6),Qt::EditRole).toInt() : 0;
}

double Oven::getPwrCool()
{
    return modelOven->rowCount()? modelOven->data(modelOven->index(0,5),Qt::EditRole).toDouble() : 0;
}

void Oven::lockUst(bool b)
{
    ui->cmdUst->setDisabled(b);
    ui->lineEditUst->setDisabled(b);
    foreach (ModelChannel *chn , channel) {
        chn->setReadOnly(b);
    }
}

void Oven::showMessage(QString mess)
{
    errMessageBox->setText(QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss")+": "+mess);
    errMessageBox->show();
}

void Oven::writeManualUst()
{
    writeUst(ui->lineEditUst->text().toDouble());
}

void Oven::readFinished(dataOven d)
{
    double min=800;
    QVector<double> tmp;
    bool ok=true;
    bool run=true;
    double pwr=0.0;

    for (int i=0; i<channel.size(); i++){
        QVector<double> vals;
        vals.push_back(d.at(i).val);
        vals.push_back(d.at(i).ust);
        vals.push_back(d.at(i).out);
        channel[i]->newVal(vals);
        if (channel.at(i)->getIsMain()) tmp.push_back(d.at(i).val);
        run = run && d.at(i).run;
        pwr+=(d.at(i).out*channel.at(i)->getPower()/100.0);
        ok= d.at(i).ok && ok;
    }

    if (!ok) emit error(tr("Нет связи с прибором"));
    foreach (double t, tmp) {
        if (t<min) min=t;
    }
    if (!ok) min=800;
    emit sigNewTemp(min);
    emit sigNewRun(run);
    emit sigNewOut(pwr);
}

void Oven::newDateTime(QDateTime t)
{
    if (isFirst){
        isFirst=false;
        Plot::instance()->setbaseTime(t);
    }
    currentDateTime=t;
    Plot::instance()->setTitle(getName()+" "+t.toString("dd.MM.yy hh:mm"));
}

void Oven::startChange(int t, double ust, double valv)
{
    if (ust!=-1) {
        writeUst(ust);
        writerValve->write(valv);
    }
    currentX=t;
    QTimer::singleShot(1000*12,this,SLOT(plot()));
}

void Oven::startRead()
{
    reader->startRead();
}

void Oven::refreshChannel()
{
    foreach (ModelChannel *chn , channel) {
        chn->refresh();
    }
}

void Oven::loadParams()
{
    QString query;
    sqlParams params;
    query="select id, num, host, port, pwr, pwr_cool, tcp_port, addr from owens where id= ? ";
    params.push_back(id_oven);
    SqlEngine::executeQuery(query,params,modelOven);

    query.clear();

    TableModel result;

    query="select id from owens_trm_channel where id_owen = ? and is_enabled=true order by number";
    if (SqlEngine::executeQuery(query,params,&result)){
        for (int i=0; i<result.rowCount(); i++){
            ModelChannel *ch = new ModelChannel(result.data(result.index(i,0),Qt::EditRole).toInt(),this);
            Writer *wr = new Writer(this->getHost(),this->getTcpPort(),this->getAddr(),11+i*5,this);
            connect(ch,SIGNAL(ustChanged(double)),wr,SLOT(write(double)));
            connect(wr,SIGNAL(sigErr(QString)),this,SLOT(showMessage(QString)));
            channel.push_back(ch);
        }
        QTableView *view;
        GroupBox *grp;
        for (int i=0; i<channel.size(); i++){
            grp = new GroupBox(channel.at(i)->getName(),this);
            view = new QTableView(this);
            grp->layout()->addWidget(view);
            ui->scrollAreaWidgetContents->layout()->addWidget(grp);
            view->setModel(channel[i]);
            view->horizontalHeader()->hide();
            view->verticalHeader()->hide();
            view->setColumnWidth(0,110);
            view->setColumnWidth(1,30);
            view->setColumnWidth(2,80);
            view->setItemDelegateForColumn(1,new ColorDelegate(this));
            view->setItemDelegateForColumn(2,new LineDelegate(this));
            view->verticalScrollBar()->setStyleSheet("QScrollBar { width: 25px; }");
            connect(channel[i],SIGNAL(nameChanged(QString)),grp,SLOT(setTitle(QString)));
        }
    }

}
