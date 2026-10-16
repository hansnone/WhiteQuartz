#pragma once

#include <QVariant>
#include <QVariantList>
#include <kdecoration3/decoration.h>
#include <kdecoration3/decorationshadow.h>
#include <kdecoration3/decoratedwindow.h>
#include "quartzdecoration.h"

namespace Quartz::Compositor {

class QuartzDecorationPlugin : public KDecoration3::Decoration {
    Q_OBJECT

public:
    explicit QuartzDecorationPlugin(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~QuartzDecorationPlugin() override = default;

    bool init() override;
    void paint(QPainter *painter, const QRectF &repaintRegion) override;

protected:
    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void updateBordersAndTitleBar();
    void updateShadow();

    QuartzDecoration m_core;
    std::shared_ptr<KDecoration3::DecorationShadow> m_shadow;
};

} // namespace Quartz::Compositor
