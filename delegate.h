#ifndef DELEGATE_H
#define DELEGATE_H

#include <QItemDelegate>
#include <QColorDialog>
#include "widgets.h"

class ColorDelegate : public QItemDelegate {
    Q_OBJECT
public:
    ColorDelegate(QObject *parent = 0);
     QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const;
     void setEditorData(QWidget *editor,const QModelIndex &index) const;
     void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
     bool eventFilter(QObject *object, QEvent *event);
 };

class LineDelegate : public QItemDelegate {
    Q_OBJECT
public:
     LineDelegate(QObject *parent = 0);
     QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const;
     void setEditorData(QWidget *editor,const QModelIndex &index) const;
     void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
     void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &index) const;
 };

#endif // DELEGATE_H
