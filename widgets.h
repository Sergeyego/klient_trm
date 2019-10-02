#ifndef WIDGETS_H
#define WIDGETS_H

#include <QComboBox>
#include <QStyledItemDelegate>
#include <QDebug>
#include <QGroupBox>
#include <QLayout>
#include <QLineEdit>
#include <QToolButton>
#include "inputdialog.h"

class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit ComboBox(QWidget *parent = 0);
public slots:

};

class GroupBox : public QGroupBox
{
    Q_OBJECT
public:
    GroupBox(QString title, QWidget *parent = 0);
public slots:
    void setTitle(QString title);
};

class LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *);


private:
    QToolButton *cmdButton;

private slots:
    void input();
};

#endif // WIDGETS_H
