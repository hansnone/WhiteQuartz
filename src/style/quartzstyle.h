#pragma once

#include <QProxyStyle>
#include <QPainter>
#include <QStyleOption>
#include "quartzmetrics.h"

namespace Quartz::Style {

class QuartzStyle : public QProxyStyle {
    Q_OBJECT

public:
    explicit QuartzStyle();
    ~QuartzStyle() override = default;

    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w = nullptr) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w = nullptr) const override;

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
    QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *w = nullptr) const override;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget = nullptr) const override;

    // Métodos para dibujo de elementos Aqua específicos
    void drawTrafficLightButton(QPainter *p, const QRect &r, const QColor &fillColor, char symbol, bool hasUnsavedChanges = false) const;
    void drawModalSheet(QPainter *p, const QRect &sheetRect, const QRect &parentWindowRect) const;
};

} // namespace Quartz::Style
