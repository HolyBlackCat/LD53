#pragma once

#include "entities.h"

constexpr int tile_size = 12;

enum class Tile
{
    air,
    wall,
    bridge,
    _count [[maybe_unused]],
};

struct Cell
{
    Tile mid = Tile::air;
    unsigned char random{};
};

struct Map : Renderable, Solid
{
    IMP_STANDALONE_COMPONENT(Game)

    Array2D<Cell, int> cells;

    Tiled::PointLayer points;

    void Load(Stream::Input source);

    void Render() const override;
    bool IsSolidAtPoint(ivec2 point) const override;
    bool IsSolidAtRect(irect2 rect) const override;
};

struct TileInfo
{
    Tile tile{};
    bool solid = false;

    struct DrawParams
    {
        const Map &map;
        ivec2 tile_pos;
        const Cell &cell;
        Tile Cell::*layer{};
        ivec2 screen_pos;
        unsigned char random{};
        std::function<void(Render::Quad_t &&)> modify_quad;
    };

    std::function<void(const DrawParams &params)> draw;
};

[[nodiscard]] const TileInfo &GetTileInfo(Tile tile);
