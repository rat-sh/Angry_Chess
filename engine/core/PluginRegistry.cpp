#include "engine/core/PluginRegistry.hpp"
#include <stdexcept>

namespace bge {


// ── Registration ──────────────────────────────────────────────────────────────

void PluginRegistry::registerGame(std::string name, GameRulesFactory f) {
    games_[std::move(name)] = std::move(f);
}
void PluginRegistry::registerTheme(std::string name, ThemeFactory f) {
    themes_[std::move(name)] = std::move(f);
}
void PluginRegistry::registerAI(std::string name, AIPlayerFactory f) {
    ais_[std::move(name)] = std::move(f);
}
void PluginRegistry::registerRenderer(std::string name, RendererFactory f) {
    renderers_[std::move(name)] = std::move(f);
}
void PluginRegistry::registerInputHandler(std::string name, InputHandlerFactory f) {
    inputs_[std::move(name)] = std::move(f);
}

// ── Creation ──────────────────────────────────────────────────────────────────

std::unique_ptr<IGameRules> PluginRegistry::createGame(std::string_view name) const {
    auto it = games_.find(std::string(name));
    if (it == games_.end())
        throw std::runtime_error("Game not registered: " + std::string(name));
    return it->second();
}
std::unique_ptr<ITheme> PluginRegistry::createTheme(std::string_view name) const {
    auto it = themes_.find(std::string(name));
    if (it == themes_.end())
        throw std::runtime_error("Theme not registered: " + std::string(name));
    return it->second();
}
std::unique_ptr<IAIPlayer> PluginRegistry::createAI(std::string_view name) const {
    auto it = ais_.find(std::string(name));
    if (it == ais_.end())
        throw std::runtime_error("AI not registered: " + std::string(name));
    return it->second();
}
std::unique_ptr<IRenderer> PluginRegistry::createRenderer(std::string_view name) const {
    auto it = renderers_.find(std::string(name));
    if (it == renderers_.end())
        throw std::runtime_error("Renderer not registered: " + std::string(name));
    return it->second();
}
std::unique_ptr<IInputHandler> PluginRegistry::createInputHandler(std::string_view name) const {
    auto it = inputs_.find(std::string(name));
    if (it == inputs_.end())
        throw std::runtime_error("InputHandler not registered: " + std::string(name));
    return it->second();
}

// ── Listing ───────────────────────────────────────────────────────────────────

std::vector<std::string> PluginRegistry::listGames() const {
    std::vector<std::string> v;
    for (auto& [k, _] : games_) v.push_back(k);
    return v;
}
std::vector<std::string> PluginRegistry::listThemes() const {
    std::vector<std::string> v;
    for (auto& [k, _] : themes_) v.push_back(k);
    return v;
}
std::vector<std::string> PluginRegistry::listAIs() const {
    std::vector<std::string> v;
    for (auto& [k, _] : ais_) v.push_back(k);
    return v;
}

} // namespace bge
