#include "colordialogdelegate.h"
#include <QColorDialog>
#include <QPainter>
#include <QPushButton>

void ColorDialogDelegate::paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    if (index.data().canConvert<QColor>()) {
        QColor color = qvariant_cast<QColor>(index.data());

        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        }

        painter->setPen(Qt::transparent);
        painter->setBrush(Qt::white);
        painter->drawRect(option.rect.x() + 2, option.rect.y() + 2, option.rect.width() - 4, option.rect.height() - 4);
        painter->setBrush(color);
        painter->drawRect(option.rect.x() + 2, option.rect.y() + 2, option.rect.width() - 4, option.rect.height() - 4);
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ColorDialogDelegate::sizeHint(
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* ColorDialogDelegate::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    if (index.data().canConvert<QColor>()) {
        QColorDialog *dialog = new QColorDialog(parent);
        return dialog;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void ColorDialogDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.data().canConvert<QColor>()) {
        QColor color = qvariant_cast<QColor>(index.data());
        QColorDialog *dialog = qobject_cast<QColorDialog*>(editor);
        dialog->setCurrentColor(color);
    }
    else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ColorDialogDelegate::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex &index) const
{
    if (index.data().canConvert<QColor>()) {
        QColorDialog *dialog = qobject_cast<QColorDialog*>(editor);
        model->setData(index, dialog->currentColor());
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
