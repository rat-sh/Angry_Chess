#pragma once
#include "engine/renderer/IRenderer.hpp"

namespace bge {

/// No-op renderer — useful for headless AI testing and unit tests.
class NullRenderer final : public IRenderer {
public:
    void beginFrame() override {}
    void drawBoard(const RenderContext&) override {}
    void drawStatus(const RenderContext&) override {}
    void drawPrompt(std::string_view) override {}
    void endFrame() override {}
};

} // namespace bge
