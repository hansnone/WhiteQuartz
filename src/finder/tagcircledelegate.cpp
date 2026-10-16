#include "tagcircledelegate.h"
#include "fileoperations.h"
#include <QPainter>
#include <QFileSystemModel>

namespace Quartz::Finder {

TagCircleDelegate::TagCircleDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void TagCircleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QString filePath = index.data(QFileSystemModel::FilePathRole).toString();
    if (filePath.isEmpty()) {
        return;
    }

    QString tag = FileOperations::getFileTag(filePath);
    if (!tag.isEmpty()) {
        QColor color = FileOperations::colorForTag(tag);
        if (color.isValid()) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->setBrush(color);
            painter->setPen(Qt::NoPen);
            int d = 8;
            int x = option.rect.right() - d - 8;
            int y = option.rect.top() + (option.rect.height() - d) / 2;
            painter->drawEllipse(x, y, d, d);
            painter->restore();
        }
    }
}

} // namespace Quartz::Finder
