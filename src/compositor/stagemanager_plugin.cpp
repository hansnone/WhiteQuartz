#include "stagemanager_plugin.h"
#include <QVector2D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QFile>

namespace Quartz::Compositor {

KWIN_EFFECT_FACTORY(StageManagerKWinPlugin, "metadata.json")

StageManagerKWinPlugin::StageManagerKWinPlugin()
    : KWin::OffscreenEffect()
{
    initShader();

    connect(&m_core, &StageManagerEffect::stateChanged, this, [this](bool) {
        if (KWin::effects) {
            KWin::effects->addRepaintFull();
        }
    });
}

StageManagerKWinPlugin::~StageManagerKWinPlugin() = default;

bool StageManagerKWinPlugin::supported()
{
    return KWin::OffscreenEffect::supported() && KWin::ShaderManager::instance();
}

void StageManagerKWinPlugin::reconfigure(ReconfigureFlags flags)
{
    KWin::OffscreenEffect::reconfigure(flags);
    initShader();
}

void StageManagerKWinPlugin::initShader()
{
    if (!KWin::ShaderManager::instance()) {
        return;
    }

    if (QFile::exists(QStringLiteral(":/effects/quartz/shaders/squircle.frag"))) {
        m_squircleShader = KWin::ShaderManager::instance()->generateShaderFromFile(
            KWin::ShaderTrait::MapTexture,
            QString(),
            QStringLiteral(":/effects/quartz/shaders/squircle.frag")
        );
    }
}

void StageManagerKWinPlugin::apply(KWin::EffectWindow *window, int mask, KWin::WindowPaintData &data, KWin::WindowQuadList &quads)
{
    KWin::OffscreenEffect::apply(window, mask, data, quads);

    if (m_core.isEnabled() && window) {
        // Aplica perspectiva 3D lateral (14 grados en eje Y) calculada por StageManagerEffect
        qreal targetX = 24.0;
        qreal targetY = 70.0;
        data.setScale(QVector3D(m_core.scaleFactor(), m_core.scaleFactor(), 1.0));
        data.setRotationAxis(QVector3D(0.0, 1.0, 0.0));
        data.setRotationAngle(m_core.perspectiveAngleY());
        data.translate(targetX - window->frameGeometry().x(), targetY - window->frameGeometry().y(), 0.0);
    }
}


#ifdef KWIN_OFFSCREEN_DRAW_RETURNS_BOOL
bool StageManagerKWinPlugin::drawWindow(const KWin::RenderTarget &renderTarget,
                                       const KWin::RenderViewport &viewport,
                                       KWin::EffectWindow *window,
                                       int mask,
                                       const KWin::Region &deviceRegion,
                                       KWin::WindowPaintData &data)
{
    if (m_squircleShader && window) {
        m_squircleShader->setUniform("uWindowSize", QVector2D(static_cast<float>(window->width()), static_cast<float>(window->height())));
        m_squircleShader->setUniform("uCornerRadius", 11.0f);
        m_squircleShader->setUniform("uPower", 4.5f);
        m_squircleShader->setUniform("uBorderColor", QVector4D(1.0f, 1.0f, 1.0f, 0.15f));

        setShader(window, m_squircleShader.get());
    }

    return KWin::OffscreenEffect::drawWindow(renderTarget, viewport, window, mask, deviceRegion, data);
}
#else
void StageManagerKWinPlugin::drawWindow(const KWin::RenderTarget &renderTarget,
                                       const KWin::RenderViewport &viewport,
                                       KWin::EffectWindow *window,
                                       int mask,
                                       const KWin::Region &deviceRegion,
                                       KWin::WindowPaintData &data)
{
    if (m_squircleShader && window) {
        m_squircleShader->setUniform("uWindowSize", QVector2D(static_cast<float>(window->width()), static_cast<float>(window->height())));
        m_squircleShader->setUniform("uCornerRadius", 11.0f);
        m_squircleShader->setUniform("uPower", 4.5f);
        m_squircleShader->setUniform("uBorderColor", QVector4D(1.0f, 1.0f, 1.0f, 0.15f));

        setShader(window, m_squircleShader.get());
    }

    KWin::OffscreenEffect::drawWindow(renderTarget, viewport, window, mask, deviceRegion, data);
}
#endif

} // namespace Quartz::Compositor

#include "stagemanager_plugin.moc"
