#pragma once

#include <kwin/effect/offscreeneffect.h>
#include <kwin/effect/effecthandler.h>
#include <kwin/opengl/glshader.h>
#include <kwin/opengl/glshadermanager.h>
#include "stagemanagereffect.h"

namespace Quartz::Compositor {

class StageManagerKWinPlugin : public KWin::OffscreenEffect {
    Q_OBJECT

public:
    explicit StageManagerKWinPlugin();
    ~StageManagerKWinPlugin() override;

    static bool supported();

    void reconfigure(ReconfigureFlags flags) override;
    void apply(KWin::EffectWindow *window, int mask, KWin::WindowPaintData &data, KWin::WindowQuadList &quads) override;

protected:
    bool drawWindow(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, KWin::EffectWindow *window, int mask, const KWin::Region &deviceRegion, KWin::WindowPaintData &data) override;

private:
    void initShader();

    StageManagerEffect m_core;
    std::unique_ptr<KWin::GLShader> m_squircleShader;
};

} // namespace Quartz::Compositor
