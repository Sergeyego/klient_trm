#include "inputdialog.h"
#include "ui_inputdialog.h"

InputDialog::InputDialog(QString txt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDialog)
{
    ui->setupUi(this);
    signalMapper = new QSignalMapper(this);
    ui->lineEdit->setText(txt);
    QDoubleValidator *validator = new QDoubleValidator(this);
    validator->setLocale(QLocale::English);
    ui->lineEdit->setValidator(validator);
    keyMap[Qt::Key_1]= new BtnKey(ui->pushButton1,QString("1"),this);
    keyMap[Qt::Key_2]= new BtnKey(ui->pushButton2,QString("2"),this);
    keyMap[Qt::Key_3]= new BtnKey(ui->pushButton3,QString("3"),this);
    keyMap[Qt::Key_4]= new BtnKey(ui->pushButton4,QString("4"),this);
    keyMap[Qt::Key_5]= new BtnKey(ui->pushButton5,QString("5"),this);
    keyMap[Qt::Key_6]= new BtnKey(ui->pushButton6,QString("6"),this);
    keyMap[Qt::Key_7]= new BtnKey(ui->pushButton7,QString("7"),this);
    keyMap[Qt::Key_8]= new BtnKey(ui->pushButton8,QString("8"),this);
    keyMap[Qt::Key_9]= new BtnKey(ui->pushButton9,QString("9"),this);
    keyMap[Qt::Key_0]= new BtnKey(ui->pushButton0,QString("0"),this);
    keyMap[Qt::Key_At]= new BtnKey(ui->pushButtonDot,QString("."),this);
    keyMap[Qt::Key_Backspace]= new BtnKey(ui->pushButtonBack,QString(),this);

    QMap<int, BtnKey *>::const_iterator i = keyMap.constBegin();
    while (i != keyMap.constEnd()) {
        signalMapper->setMapping(i.value()->btn,i.key());
        connect(i.value()->btn,SIGNAL(clicked()),signalMapper,SLOT(map()));
        ++i;
    }
    connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(send(int)));
    connect(ui->pushButtonEnter,SIGNAL(clicked()),this,SLOT(accept()));
}

InputDialog::~InputDialog()
{
    delete ui;
}

int InputDialog::exec()
{
    ui->lineEdit->selectAll();
    return QDialog::exec();
}

void InputDialog::accept()
{
    emit editFinished(ui->lineEdit->text());
    QDialog::accept();
}

void InputDialog::send(int key)
{
    QKeyEvent* pe = new QKeyEvent(QEvent::KeyPress,key,Qt::NoModifier,keyMap.value(key)->text);
    QApplication::sendEvent(ui->lineEdit,pe);
}
