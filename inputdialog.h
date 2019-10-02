#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QLineEdit>
#include <QKeyEvent>
#include <QSignalMapper>
#include <QMap>
#include <QDoubleValidator>

namespace Ui {
class InputDialog;
}

class BtnKey : public QObject {
    Q_OBJECT
public:
    BtnKey(QPushButton *b, QString t, QObject *parent=0){
        btn=b;
        text=t;
    }
    QPushButton *btn;
    QString text;
};

class InputDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit InputDialog(QString txt, QWidget *parent = 0);
    ~InputDialog();
    
private:
    Ui::InputDialog *ui;
    QSignalMapper *signalMapper;
    QMap <int,BtnKey*> keyMap;

public slots:
    int exec();
    void accept();

private slots:
    void send(int key);

signals:
    void editFinished(QString text);
};

#endif // INPUTDIALOG_H
