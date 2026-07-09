#pragma once
#include "engine/rules/IGameRules.hpp"
#include "engine/renderer/IRenderer.hpp"
#include "engine/ai/IAIPlayer.hpp"
#include "engine/theme/ITheme.hpp"
#include "engine/input/IInputHandler.hpp"
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace bge {

/// Factory function types used by the PluginRegistry.
using GameRulesFactory    = std::function<std::unique_ptr<IGameRules>()>;
using RendererFactory     = std::function<std::unique_ptr<IRenderer>()>;
using AIPlayerFactory     = std::function<std::unique_ptr<IAIPlayer>()>;
using ThemeFactory        = std::function<std::unique_ptr<ITheme>()>;
using InputHandlerFactory = std::function<std::unique_ptr<IInputHandler>()>;

/// Central factory registry.
/// Game plugins call register*() on startup; the engine calls create*() at
/// game-start time.  Adding a new game == writing a plugin — no engine edits.
class PluginRegistry {
public:
    // ── Registration ─────────────────────────────────────────────────────────
    void registerGame        (std::string name, GameRulesFactory    f);
    void registerTheme       (std::string name, ThemeFactory        f);
    void registerAI          (std::string name, AIPlayerFactory     f);
    void registerRenderer    (std::string name, RendererFactory     f);
    void registerInputHandler(std::string name, InputHandlerFactory f);

    // ── Creation ─────────────────────────────────────────────────────────────
    [[nodiscard]] std::unique_ptr<IGameRules>    createGame        (std::string_view name) const;
    [[nodiscard]] std::unique_ptr<ITheme>        createTheme       (std::string_view name) const;
    [[nodiscard]] std::unique_ptr<IAIPlayer>     createAI          (std::string_view name) const;
    [[nodiscard]] std::unique_ptr<IRenderer>     createRenderer    (std::string_view name) const;
    [[nodiscard]] std::unique_ptr<IInputHandler> createInputHandler(std::string_view name) const;

    // ── Listing ───────────────────────────────────────────────────────────────
    [[nodiscard]] std::vector<std::string> listGames()    const;
    [[nodiscard]] std::vector<std::string> listThemes()   const;
    [[nodiscard]] std::vector<std::string> listAIs()      const;

private:
    std::unordered_map<std::string, GameRulesFactory>    games_;
    std::unordered_map<std::string, ThemeFactory>        themes_;
    std::unordered_map<std::string, AIPlayerFactory>     ais_;
    std::unordered_map<std::string, RendererFactory>     renderers_;
    std::unordered_map<std::string, InputHandlerFactory> inputs_;
};

} // namespace bge
