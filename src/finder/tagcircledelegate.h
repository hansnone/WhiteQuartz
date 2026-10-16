#pragma once

#include <QStyledItemDelegate>

namespace Quartz::Finder {

class TagCircleDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit TagCircleDelegate(QObject *parent = nullptr);
    ~TagCircleDelegate() override = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

} // namespace Quartz::Finder
