#include "widgets.h"

ComboBox::ComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setItemDelegate(new QStyledItemDelegate(this));
    setStyleSheet(QString("QListView::item { margin: %1px }").arg(10));
}


GroupBox::GroupBox(QString title, QWidget *parent) : QGroupBox(title, parent)
{
    setLayout(new QHBoxLayout(this));
    setMinimumHeight(150);
}

void GroupBox::setTitle(QString title)
{
    QGroupBox::setTitle(title);
}

LineEdit::LineEdit(QWidget *parent)
    : QLineEdit(parent){
    cmdButton = new QToolButton(this);
    QPixmap pixmap(":/key.png");
    cmdButton->setIcon(QIcon(pixmap));
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    cmdButton->setIconSize(QSize(this->size().height()-frameWidth,this->size().height()-frameWidth));
    cmdButton->setCursor(Qt::ArrowCursor);
    cmdButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    connect(cmdButton, SIGNAL(clicked()), this, SLOT(input()));
}

void LineEdit::resizeEvent(QResizeEvent *)
{
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    int h = this->size().height();
    cmdButton->setIconSize(QSize(h-frameWidth,h-frameWidth));
    cmdButton->move(this->size().width()-cmdButton->iconSize().width()-7,-frameWidth);
}

void LineEdit::input()
{
    InputDialog d(this->text(),this);
    connect(&d,SIGNAL(editFinished(QString)),this,SLOT(setText(QString)));
    d.exec();
}
