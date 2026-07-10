#pragma once
#include "engine/core/PluginRegistry.hpp"

namespace angry_chess {

/// Plugin entry point.
/// Call install() once at program startup; it registers all factories
/// into the shared PluginRegistry. No engine code is modified.
class AngryChessPlugin {
public:
    static void install(bge::PluginRegistry& registry);
};

} // namespace angry_chess
