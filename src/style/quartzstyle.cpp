#include "quartzstyle.h"
#include <QPainterPath>
#include <QApplication>

namespace Quartz::Style {

QuartzStyle::QuartzStyle()
    : QProxyStyle(QStringLiteral("fusion"))
{
}

void QuartzStyle::drawTrafficLightButton(QPainter *p, const QRect &r, const QColor &fillColor, char symbol, bool hasUnsavedChanges) const
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);

    // Fondo circular con borde ligeramente más oscuro
    p->setPen(fillColor.darker(115));
    p->setBrush(fillColor);
    p->drawEllipse(r);

    int cx = r.center().x();
    int cy = r.center().y();

    // Si tiene cambios no guardados en el botón rojo (sin hover), mostrar punto oscuro en el centro
    if (hasUnsavedChanges && symbol == '\0') {
        p->setPen(Qt::NoPen);
        p->setBrush(fillColor.darker(250));
        p->drawEllipse(QPoint(cx, cy), 2, 2);
    }

    // Dibujo del símbolo interior al estar bajo el cursor (Hover)
    p->setPen(QPen(fillColor.darker(220), 1.3));
    switch (symbol) {
    case 'x': // Botón rojo: ×
        p->drawLine(cx - 2, cy - 2, cx + 2, cy + 2);
        p->drawLine(cx - 2, cy + 2, cx + 2, cy - 2);
        break;
    case '-': // Botón amarillo: -
        p->drawLine(cx - 3, cy, cx + 3, cy);
        break;
    case '+': // Botón verde: flechas diagonales opuestas
        p->drawLine(cx - 2, cy - 2, cx - 2, cy);
        p->drawLine(cx - 2, cy - 2, cx, cy - 2);
        p->drawLine(cx + 2, cy + 2, cx + 2, cy);
        p->drawLine(cx + 2, cy + 2, cx, cy + 2);
        p->drawLine(cx - 2, cy - 2, cx + 2, cy + 2);
        break;
    default:
        break;
    }

    p->restore();
}

void QuartzStyle::drawModalSheet(QPainter *p, const QRect &sheetRect, const QRect &parentWindowRect) const
{
    Q_UNUSED(parentWindowRect);
    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);

    // Fondo blanco translúcido con esquinas inferiores redondeadas (11px)
    QPainterPath path;
    path.moveTo(sheetRect.topLeft());
    path.lineTo(sheetRect.topRight());
    path.lineTo(sheetRect.right(), sheetRect.bottom() - 11);
    path.arcTo(sheetRect.right() - 22, sheetRect.bottom() - 22, 22, 22, 0, -90);
    path.lineTo(sheetRect.left() + 11, sheetRect.bottom());
    path.arcTo(sheetRect.left(), sheetRect.bottom() - 22, 22, 22, -90, -90);
    path.closeSubpath();

    p->fillPath(path, QColor(255, 255, 255, 245));
    p->setPen(QColor(210, 210, 215));
    p->drawPath(path);

    // Borde perimetral interior subpíxel de 1px
    p->setPen(QColor(255, 255, 255, 120));
    p->drawPath(path);

    p->restore();
}

void QuartzStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    switch (pe) {
    case PE_PanelButtonCommand: {
        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);
        QRect r = opt->rect;

        QPainterPath path;
        path.addRoundedRect(r.adjusted(1, 1, -1, -1), StyleMetrics::PushButtonRadius, StyleMetrics::PushButtonRadius);

        bool isDefault = false;
        if (const auto *btnOpt = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            isDefault = (btnOpt->features & QStyleOptionButton::DefaultButton);
        }

        if (isDefault) {
            p->fillPath(path, QColor(0, 122, 255));
            p->setPen(QColor(0, 100, 220));
            p->drawPath(path);
        } else {
            QColor bg = (opt->state & State_Sunken) ? QColor(220, 220, 224) : QColor(245, 245, 247);
            p->fillPath(path, bg);
            p->setPen(QColor(210, 210, 215));
            p->drawPath(path);
        }
        p->restore();
        return;
    }
    case PE_FrameGroupBox: {
        // Tarjetas Inset Grouped
        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);
        QPainterPath path;
        path.addRoundedRect(opt->rect.adjusted(1, 1, -1, -1), StyleMetrics::InsetGroupCardRadius, StyleMetrics::InsetGroupCardRadius);
        p->fillPath(path, QColor(255, 255, 255, 235));
        p->setPen(QColor(225, 225, 230));
        p->drawPath(path);

        // Borde perimetral subpíxel interior de 1px
        p->setPen(QColor(255, 255, 255, 180));
        p->drawRoundedRect(opt->rect.adjusted(2, 2, -2, -2), StyleMetrics::InsetGroupCardRadius - 1, StyleMetrics::InsetGroupCardRadius - 1);
        p->restore();
        return;
    }
    default:
        QProxyStyle::drawPrimitive(pe, opt, p, w);
        break;
    }
}

void QuartzStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    switch (element) {
    case CE_PushButtonLabel: {
        if (const auto *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            p->save();
            if (btn->features & QStyleOptionButton::DefaultButton) {
                p->setPen(Qt::white);
            } else {
                p->setPen(QColor(28, 28, 30));
            }
            p->drawText(btn->rect, Qt::AlignCenter, btn->text);
            p->restore();
            return;
        }
        break;
    }
    default:
        break;
    }
    QProxyStyle::drawControl(element, opt, p, w);
}

void QuartzStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    if (control == CC_TitleBar) {
        if (const auto *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt)) {
            p->save();
            p->setRenderHint(QPainter::Antialiasing, true);

            // Fondo de la cabecera unificada
            p->fillRect(tb->rect, QColor(246, 246, 246));
            p->setPen(QColor(218, 218, 218));
            p->drawLine(tb->rect.bottomLeft(), tb->rect.bottomRight());

            // Traffic Lights a la izquierda
            QRect closeR = subControlRect(CC_TitleBar, tb, SC_TitleBarCloseButton, w);
            QRect minR = subControlRect(CC_TitleBar, tb, SC_TitleBarMinButton, w);
            QRect maxR = subControlRect(CC_TitleBar, tb, SC_TitleBarMaxButton, w);

            char closeSym = (tb->activeSubControls & SC_TitleBarCloseButton) ? 'x' : '\0';
            char minSym = (tb->activeSubControls & SC_TitleBarMinButton) ? '-' : '\0';
            char maxSym = (tb->activeSubControls & SC_TitleBarMaxButton) ? '+' : '\0';

            drawTrafficLightButton(p, closeR, QColor(255, 95, 87), closeSym, false);
            drawTrafficLightButton(p, minR, QColor(254, 188, 46), minSym, false);
            drawTrafficLightButton(p, maxR, QColor(40, 201, 64), maxSym, false);

            // Título de ventana en negrita
            p->setPen(QColor(60, 60, 67));
            QFont font = p->font();
            font.setBold(true);
            p->setFont(font);
            p->drawText(tb->rect, Qt::AlignCenter, tb->text);
            p->restore();
            return;
        }
    } else if (control == CC_ScrollBar) {
        if (const auto *sb = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            p->save();
            p->setRenderHint(QPainter::Antialiasing, true);
            QRect sliderRect = subControlRect(CC_ScrollBar, sb, SC_ScrollBarSlider, w);
            if (sliderRect.isValid()) {
                QPainterPath path;
                path.addRoundedRect(sliderRect.adjusted(2, 2, -2, -2), 4, 4);
                QColor thumbColor = (sb->activeSubControls & SC_ScrollBarSlider)
                    ? QColor(100, 100, 105, 180)
                    : QColor(140, 140, 145, 130);
                p->fillPath(path, thumbColor);
            }
            p->restore();
            return;
        }
    }
    QProxyStyle::drawComplexControl(control, opt, p, w);
}

int QuartzStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    case PM_TitleBarHeight:
        return StyleMetrics::StandardTitleBarHeight;
    case PM_ButtonMargin:
        return 6;
    case PM_ScrollBarExtent:
        return 10;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

QSize QuartzStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *w) const
{
    QSize sz = QProxyStyle::sizeFromContents(ct, opt, contentsSize, w);
    if (ct == CT_PushButton) {
        sz.setHeight(qMax(sz.height(), static_cast<int>(StyleMetrics::PushButtonHeight)));
    }
    return sz;
}

QRect QuartzStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *widget) const
{
    if (cc == CC_TitleBar) {
        int y = (StyleMetrics::StandardTitleBarHeight - StyleMetrics::TrafficLightButtonSize) / 2;
        int d = StyleMetrics::TrafficLightButtonSize;
        int step = StyleMetrics::TrafficLightButtonSize + StyleMetrics::TrafficLightSpacing;
        int startX = StyleMetrics::TrafficLightLeftMargin;

        switch (sc) {
        case SC_TitleBarCloseButton:
            return QRect(startX, y, d, d);
        case SC_TitleBarMinButton:
            return QRect(startX + step, y, d, d);
        case SC_TitleBarMaxButton:
            return QRect(startX + 2 * step, y, d, d);
        default:
            break;
        }
    }
    return QProxyStyle::subControlRect(cc, opt, sc, widget);
}

} // namespace Quartz::Style
