#pragma once
#include "engine/pieces/IPiece.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <stdexcept>

namespace bge {

/// Registry mapping PieceTypeID → PieceDescriptor.
/// Game plugins register their piece types here at startup.
class PieceRegistry {
public:
    void registerPiece(PieceDescriptor desc) {
        pieces_[desc.id] = std::move(desc);
    }

    [[nodiscard]] const PieceDescriptor* find(PieceTypeID id) const {
        auto it = pieces_.find(id);
        return it == pieces_.end() ? nullptr : &it->second;
    }

    [[nodiscard]] bool has(PieceTypeID id) const {
        return pieces_.count(id) > 0;
    }

    [[nodiscard]] std::vector<PieceDescriptor> all() const {
        std::vector<PieceDescriptor> v;
        v.reserve(pieces_.size());
        for (auto& [id, d] : pieces_) v.push_back(d);
        return v;
    }

private:
    std::unordered_map<PieceTypeID, PieceDescriptor> pieces_;
};

} // namespace bge
