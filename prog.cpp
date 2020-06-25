#include "prog.h"
#include "ui_prog.h"

Prog::Prog(Oven *o, int ival, bool is_manual, QWidget *parent) :
    manual(is_manual), QWidget(parent),
    ui(new Ui::Prog),oven(o)
{
    ui->setupUi(this);

    tN=0;//число опросов таймера
    step=0;//текущий шаг
    tStep=0;//время на текущем шаге, сек
    interval=ival;//итрервал таймера, сек
    isRunning=false;//прокалка запущена
    waitFlg=true;//ожидаем набор температуры
    intervalRead=5;//интервал таймера опроса, сек
    ovenRun=false;//печь включена

    id_ow_rab=-1;
    id_part=0;

    modelElDim = new TableModel(this);
    modelElDim->setColumnCount(2);
    ui->comboBoxMark->setModel(modelElDim);
    ui->comboBoxMark->setModelColumn(1);
    ui->comboBoxMark->setEnabled(manual);
    modelRab = new TableModel(this);
    modelRab->setColumnCount(2);
    ui->comboBoxRab->setModel(modelRab);
    ui->comboBoxRab->setModelColumn(1);
    modelPart = new TableModel(this);
    modelPart->setColumnCount(6);
    modelPart->setHeaderData(1,Qt::Horizontal,tr("Партия"));
    modelPart->setHeaderData(2,Qt::Horizontal,tr("Марка"));
    modelPart->setHeaderData(3,Qt::Horizontal,tr("Ф"));
    modelPart->setHeaderData(4,Qt::Horizontal,tr("Дата"));
    ui->tableViewPart->verticalHeader()->setDefaultSectionSize(ui->tableViewPart->verticalHeader()->fontMetrics().height()*2);
    ui->tableViewPart->verticalHeader()->hide();
    ui->tableViewPart->setModel(modelPart);
    ui->tableViewPart->verticalScrollBar()->setStyleSheet("QScrollBar { width: 25px; }");

    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->setColumnHidden(5,true);
    ui->tableViewPart->setColumnWidth(1,55);
    ui->tableViewPart->setColumnWidth(2,110);
    ui->tableViewPart->setColumnWidth(3,30);
    ui->tableViewPart->setColumnWidth(4,65);

    QRegExpValidator *validator = new QRegExpValidator(QRegExp("[+-]?\\d*\\.?\\d+"),this);
    ui->lineEditT->setValidator(validator);
    ui->lineEditW->setValidator(validator);
    ui->lineEditKg->setValidator(validator);
    ui->lineEditT->setText("25");
    ui->lineEditW->setText("8");
    updatePart();
    ui->comboBoxMark->setCurrentIndex(-1);
    modelProg = new TableModel(this);
    modelProg->setColumnCount(3);
    modelProg->setHeaderData(0,Qt::Horizontal,tr("Шаг"));
    modelProg->setHeaderData(1,Qt::Horizontal,tr("t, мин."));
    modelProg->setHeaderData(2,Qt::Horizontal,tr("T, град."));
    ui->tableViewProg->verticalHeader()->setDefaultSectionSize(ui->tableViewProg->verticalHeader()->fontMetrics().height()*1.5);
    ui->tableViewProg->verticalHeader()->hide();
    ui->tableViewProg->setModel(modelProg);
    ui->tableViewProg->setColumnWidth(0,50);
    ui->tableViewProg->setColumnWidth(1,80);
    ui->tableViewProg->setColumnWidth(2,80);
    ui->tableViewProg->verticalScrollBar()->setStyleSheet("QScrollBar { width: 25px; }");
    modelProgType = new TableModel(this);
    modelProgType->setColumnCount(4);
    ui->comboBoxProg->setModel(modelProgType);
    ui->comboBoxProg->setModelColumn(1);

    timer=new QTimer(this);
    timer->setInterval(interval*1000);
    Plot::instance()->setInterval(interval);

    Plot::instance()->setXmax(2);

    viewStat();

    timerRead = new QTimer(this);
    timerRead->setInterval(intervalRead*1000);

    ui->lineEditStep->setValidator(new QIntValidator(0,15,this));

    if (!manual){
        ui->stackedWidgetProg->setCurrentIndex(0);
        ui->tableViewProg->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->lineEditPart->setReadOnly(true);
        connect(ui->comboBoxProg,SIGNAL(currentIndexChanged(int)),this,SLOT(setProg()));
        connect(ui->cmdUch,SIGNAL(clicked()),this,SLOT(setProg()));
        connect(ui->lineEditT,SIGNAL(editingFinished()),this,SLOT(setProg()));
        connect(ui->lineEditW,SIGNAL(editingFinished()),this,SLOT(setProg()));
    } else {
        ui->stackedWidgetProg->setCurrentIndex(1);
        ui->tableViewProg->setItemDelegateForColumn(1,new LineDelegate(this));
        ui->tableViewProg->setItemDelegateForColumn(2,new LineDelegate(this));
        ui->cmdUch->setVisible(false);
        connect(ui->lineEditStep,SIGNAL(textChanged(QString)),this,SLOT(setStepCount()));
    }

    //connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updateProg(QModelIndex)));
    connect(timer,SIGNAL(timeout()),this,SLOT(proc()));
    connect(timerRead,SIGNAL(timeout()),oven,SLOT(startRead()));
    connect(ui->cmdPart,SIGNAL(clicked()),this,SLOT(viewPart()));
    connect(ui->cmdProg,SIGNAL(clicked()),this,SLOT(viewProg()));
    connect(ui->cmdUpdPart,SIGNAL(clicked()),this,SLOT(updatePart()));
    connect(ui->tableViewPart,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(viewProg()));
    connect(ui->cmdStart,SIGNAL(clicked()),this,SLOT(startProg()));
    connect(ui->cmdStop,SIGNAL(clicked()),this,SLOT(stopProg()));
    connect(oven,SIGNAL(sigRead(int)),Plot::instance(),SLOT(setXmax(int)));
    connect(oven,SIGNAL(sigNewTemp(double)),ui->lcdNumberTemp,SLOT(display(double)));
    connect(oven,SIGNAL(sigNewOut(double)),this,SLOT(calcPw(double)));
    connect(oven,SIGNAL(sigNewRun(bool)),this,SLOT(setOwenRun(bool)));
    connect(modelProg,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(viewStat()));

    timer->start();
    timerRead->start();

}

Prog::~Prog()
{
    delete ui;
}

double Prog::calcProc()
{
    if (!isRunning) return -1;
    double ust=0;
    tStep+=interval;
    double temp=ui->lcdNumberTemp->value();
    if (tStep>=stepTime(step)){//переход на следующий шаг
        tStep=0;
        step++;
        waitFlg=true;
    }
    if (step>=stepCount()){//завершение прокалки
        progFinish();
        return 20;
    }
    if (stepTemp(step)==0 && step<(stepCount()-1) && step>0){//расчёт значения уставки
        double tBeg=stepTemp(step-1);
        double tEnd=stepTemp(step+1);
        ust=tBeg+tStep*(tEnd-tBeg)/stepTime(step);
    } else {
        if (waitFlg && temp>=(stepTemp(step)-4) && temp<=(stepTemp(step)+100))//ждем набора
            waitFlg=false;
        if (waitFlg && step!=stepCount()-1) tStep=0;
        ust=stepTemp(step);
    }
    return ust;
}

void Prog::proc()
{
    double nextUst=calcProc();
    oven->startChange(tN,nextUst,getValve());
    tN+=1;
    viewStat();
}

void Prog::viewPart()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Prog::viewProg()
{
    ui->stackedWidget->setCurrentIndex(1);
    updateProg(ui->tableViewPart->currentIndex());
}

void Prog::updateProg(QModelIndex index)
{
    id_part=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),0)).toInt();
    QString query="select r.id, r.nam, r.ide, r.fnam from  get_dry_progs(?) as r order by r.nam";
    sqlParams params;
    params.push_back(id_part);
    SqlEngine::executeQuery(query,params,modelProgType);
    if (!manual){
        setProg();
    }
    ui->lineEditPart->setText(ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),1)).toString());
    if (ui->comboBoxProg->count()){
        int pos=0;
        int currentIndex=ui->comboBoxProg->currentIndex();
        QString ide=ui->comboBoxProg->model()->data(ui->comboBoxProg->model()->index(currentIndex,2)).toString();
        while (ide!=modelElDim->data(modelElDim->index(pos,0)).toString()){
            pos++;
        }
        ui->comboBoxMark->setCurrentIndex(pos);
    }
}

void Prog::setProg()
{
    if (!ui->comboBoxProg->count()) {
        modelProg->setRowCount(0);
    } else {
        int pos=ui->comboBoxProg->currentIndex();
        int id_prog=ui->comboBoxProg->model()->data(ui->comboBoxProg->model()->index(pos,0)).toInt();
        //qDebug()<<pos<<" "<<id_prog;
        QString query="select step, tim, tra from calc_proga_new( ? , ? , ? , ? ) order by step";
        sqlParams params;
        params.push_back(id_prog);
        params.push_back(ui->lineEditW->text().toDouble());
        params.push_back(ui->lineEditT->text().toDouble());
        params.push_back(oven->getId());
        SqlEngine::executeQuery(query,params,modelProg);
    }
    viewStat();
}

void Prog::startProg()
{
    int mind=ui->comboBoxMark->currentIndex();
    if (!isRunning && stepCount() && mind>=0){
        beginDateTime=oven->getCurrentDateTime();
        Plot::instance()->setbaseTime(beginDateTime);
        tN=0;
        step=0;
        tStep=0;
        waitFlg=true;
        oven->clear();
        Plot::instance()->setXmax(tN);
        isRunning=true;
        ui->doubleSpinBoxEnerg->setValue(0.0);
        sqlWriteStart();
    }
    lockWidgets(isRunning);
    viewStat();
}

void Prog::stopProg()
{
    isRunning=false;
    id_ow_rab=-1;
    lockWidgets(isRunning);
    viewStat();
}

void Prog::lockWidgets(bool b)
{
    ui->cmdStart->setDisabled(b);
    ui->cmdStop->setDisabled(!b);
    ui->comboBoxProg->setDisabled(b);
    ui->comboBoxRab->setDisabled(b);
    ui->cmdPart->setDisabled(b);
    ui->cmdUch->setDisabled(b);
    ui->lineEditPart->setDisabled(b);
    ui->lineEditStep->setDisabled(b);
    ui->lineEditT->setDisabled(b);
    ui->lineEditW->setDisabled(b);
    ui->lineEditZms->setDisabled(b);
    ui->lineEditKg->setDisabled(b);
    if (manual){
        ui->comboBoxMark->setDisabled(b);
        ui->tableViewProg->setDisabled(b);
    }
    oven->lockUst(b);
}

void Prog::updatePart()
{
    QString equery="select ide, fnam from dry_els order by fnam";
    SqlEngine::executeQuery(equery,modelElDim);

    QString squery;
    QDate date2 =oven->getCurrentDateTime().date().addDays(-30);
    if (ui->checkBoxMonth->isChecked())
        squery="SELECT p.id, p.n_s, e.marka, p.diam, p.dat_part, p.id_el FROM parti p "
                "inner join elrtr e on e.id=p.id_el WHERE p.id_ist = 1 and p.dat_part>'"
               +date2.toString("yyyy-MM-dd") +"' ORDER BY p.dat_part DESC, p.n_s DESC";
    else
        squery="SELECT p.id, p.n_s, e.marka, p.diam, p.dat_part, p.id_el FROM parti p "
                "inner join elrtr e on e.id=p.id_el WHERE p.id_ist = 1 ORDER BY p.dat_part DESC, p.n_s DESC";

    SqlEngine::executeQuery(squery,modelPart);

    QString rquery="select r.id, r.snam from rab_rab r inner join rab_qual q on q.id_rab=r.id "
            "inner join rab_prof p on q.id_prof = p.id "
            "WHERE q.dat = (select max(dat) from rab_qual where dat <= '2999-04-01' "
            "and id_rab=r.id) and p.id=8 order by r.snam";
    SqlEngine::executeQuery(rquery,modelRab);

    oven->refreshChannel();
}

void Prog::calcPw(double pwr)
{
    if (ovenRun){
        ui->doubleSpinBoxPwr->setValue(pwr);
        double base=ui->doubleSpinBoxEnerg->value();
        base+=(pwr+oven->getPwrCool())*intervalRead/3600.0;
        ui->doubleSpinBoxEnerg->setValue(base);
    } else {
        ui->doubleSpinBoxPwr->setValue(0);
    }
}

void Prog::setOwenRun(bool b)
{
    ovenRun=b;
    if (b){
        ui->labelRun->setPixmap(QPixmap(":/on.png"));
    } else {
        ui->labelRun->setPixmap(QPixmap(":/off.png"));
    }
}

int Prog::stepCount()
{
    return ui->tableViewProg->model()->rowCount();
}

int Prog::stepTime(int st)
{
    double time=ui->tableViewProg->model()->data(ui->tableViewProg->model()->index(st,1),Qt::EditRole).toDouble();
    time=time*60;//переводим в секунды
    return (int)time;
}

double Prog::stepTemp(int st)
{
    double temp=ui->tableViewProg->model()->data(ui->tableViewProg->model()->index(st,2),Qt::EditRole).toDouble();
    return temp;
}

void Prog::viewStat()
{
    int proch = isRunning? (int)(tN*interval/60.0) : 0;
    int ost=(int)(getEndTime()/60.0);
    int prod=proch+ost;
    QTime t=oven->getCurrentDateTime().time().addSecs(ost*60);
    ui->labelProd->setText(QString::number(prod)+tr("    Оконч. ")+t.toString("hh:mm"));
    ui->labelProch->setText(QString::number(proch));
    ui->labelStep->setText(QString::number(step+1));
    ui->labelOst->setText(QString::number(ost));
    ui->labelTStep->setText(QString::number((int)(tStep/60.0)));
    ui->progressBar->setValue(prod>0 ? (int)proch*100/(prod) : 0);
}

double Prog::getEndTime()
{
    double Tkp=0, Tk=0;
    for (int i=step; i<stepCount(); i++)
        Tkp+=stepTime(i);
    Tk=Tkp-tStep;
    if (Tk<0) Tk=0;
    return Tk;
}

void Prog::progFinish()
{
    sqlWriteStop();
    stopProg();
    QMessageBox* pmbx = new QMessageBox(oven->getName()+" "+ui->comboBoxMark->currentText(),
                                        QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss")+": "+tr("Прокалка ")+ui->comboBoxMark->currentText()+tr(" завершена"),
                                        QMessageBox::Information, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
    pmbx->setModal(false);
    pmbx->setWindowFlags(Qt::WindowStaysOnTopHint);
    pmbx->show();
    connect(pmbx,SIGNAL(accepted()),pmbx,SLOT(deleteLater()));
}

void Prog::sqlWriteStart()
{
    id_ow_rab=-1;
    QString query;
    sqlParams params;
    TableModel resultModel;
    query="insert into owens_rab (id_owen, id_rab, id_eldim, id_prog, n_s, zms, w0, t0, dt_beg, kvo, id_part, new_format) values "
            "( ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? , ? ) returning id";
    params.push_back(oven->getId());
    params.push_back(modelRab->data(modelRab->index(ui->comboBoxRab->currentIndex(),0)).toInt());
    int mind=ui->comboBoxMark->currentIndex();
    params.push_back(ui->comboBoxMark->model()->data(ui->comboBoxMark->model()->index(mind,0)).toInt());
    params.push_back(manual ? QVariant() : modelProgType->data(modelProgType->index(ui->comboBoxProg->currentIndex(),0)).toInt());
    params.push_back(ui->lineEditPart->text());
    params.push_back(ui->lineEditZms->text());
    params.push_back(ui->lineEditW->text().toDouble());
    params.push_back(ui->lineEditT->text().toDouble());
    params.push_back(beginDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    params.push_back(ui->lineEditKg->text().toDouble());
    params.push_back(manual ? QVariant() : id_part);
    params.push_back(true);
    if (SqlEngine::executeQuery(query,params,&resultModel)){
        if (resultModel.rowCount()==1 && resultModel.columnCount()==1){
            id_ow_rab=resultModel.data(resultModel.index(0,0)).toInt();
        }
    }
}

void Prog::sqlWriteStop()
{
    QDateTime end_tim=oven->getCurrentDateTime();
    QString query;
    sqlParams params;
    query="update owens_rab set dt_end= ? , energ= ?  where id= ? ";
    params.push_back(end_tim.toString("yyyy-MM-dd hh:mm:ss"));
    params.push_back(ui->doubleSpinBoxEnerg->value());
    params.push_back(id_ow_rab);
    SqlEngine::executeQuery(query,params);
    query.clear();
    params.clear();
    query="update owens_data_new set id_ow_rab= ? where id_channel in (select id from owens_trm_channel where id_owen = ? ) and dat_time between ? and ?";
    params.push_back(id_ow_rab);
    params.push_back(oven->getId());
    params.push_back(beginDateTime);
    params.push_back(end_tim);
    SqlEngine::executeQuery(query,params);
}

double Prog::getValve()
{
    int max=0;
    for (int i=0; i<ui->tableViewProg->model()->rowCount(); i++){
        double val=ui->tableViewProg->model()->data(ui->tableViewProg->model()->index(i,2),Qt::EditRole).toDouble();
        if (val>max) max=val;
    }
    if (max<=200) max=380;
    return max-5.0;
}

void Prog::setStepCount()
{
    modelProg->setRowCount(ui->lineEditStep->text().toInt());
    for (int i=0; i<modelProg->rowCount(); i++){
        modelProg->setData(modelProg->index(i,0),i+1);
    }
}
