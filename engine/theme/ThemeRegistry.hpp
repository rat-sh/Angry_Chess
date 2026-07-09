#pragma once
#include "engine/theme/ITheme.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace bge {

using ThemeFactory = std::function<std::unique_ptr<ITheme>()>;

/// Maps theme names → ITheme factories.
class ThemeRegistry {
public:
    void registerTheme(std::string name, ThemeFactory f) {
        themes_[std::move(name)] = std::move(f);
    }

    [[nodiscard]] std::unique_ptr<ITheme> create(std::string_view name) const {
        auto it = themes_.find(std::string(name));
        if (it == themes_.end()) return nullptr;
        return it->second();
    }

    [[nodiscard]] bool has(std::string_view name) const {
        return themes_.count(std::string(name)) > 0;
    }

    [[nodiscard]] std::vector<std::string> list() const {
        std::vector<std::string> v;
        v.reserve(themes_.size());
        for (auto& [k, _] : themes_) v.push_back(k);
        return v;
    }

private:
    std::unordered_map<std::string, ThemeFactory> themes_;
};

} // namespace bge
