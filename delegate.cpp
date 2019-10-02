#include "delegate.h"

ColorDelegate::ColorDelegate(QObject *parent):QItemDelegate(parent)
{

}

QWidget *ColorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QColor color = QColor(index.model()->data(index, Qt::EditRole).toString());
    QColorDialog *d = new QColorDialog(color,parent);
    return d;
}

void ColorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    return;
}

void ColorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QColorDialog *d= static_cast<QColorDialog*>(editor);
    if (d){
        model->setData(index,d->currentColor());
    }
}

bool ColorDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type()== QEvent::FocusOut)
        return false;
    return QItemDelegate::eventFilter(object,event);
}

LineDelegate::LineDelegate(QObject *parent):QItemDelegate(parent)
{

}

QWidget *LineDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    LineEdit *e= new LineEdit(parent);
    e->setValidator(new QDoubleValidator(parent));
    e->setText(index.model()->data(index,Qt::EditRole).toString());
    return e;
}

void LineDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    return;
}

void LineDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    LineEdit *le = static_cast<LineEdit*>(editor);
    if (le){
        bool ok;
        double val=le->text().toDouble(&ok);
        if (ok)
            model->setData(index,val);
        else if (le->text().isEmpty())
            model->setData(index,QVariant());
    }
}

void LineDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
