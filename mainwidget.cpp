#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(int id_ov, int interval, bool isManual, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    ui->horizontalLayoutPlot->addWidget(Plot::instance());
    cfgPlot = new CfgPlot(Plot::instance(),this);
    oven = new Oven(id_ov,this);
    Plot::instance()->setTitle(oven->getName());
    QString title=oven->getName();
    if (isManual){
        title+=tr(" ручной режим");
    }
    setWindowTitle(title);
    prog = new Prog(oven,interval,isManual,this);
    ui->verticalLayoutProg->addWidget(prog);
    ui->verticalLayoutOven->addWidget(cfgPlot);
    ui->verticalLayoutOven->addWidget(oven);
    connect(ui->cmdExit,SIGNAL(clicked()),this,SLOT(quit()));
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::closeEvent(QCloseEvent *e)
{
    e->ignore();
    this->showMinimized();
}

void MainWidget::quit()
{
    int n=QMessageBox::question(this,tr("Выход"),tr("Завершить программу?"),QMessageBox::Yes,QMessageBox::No);
    if (n==QMessageBox::Yes) {
        QApplication::exit();
    }
}

