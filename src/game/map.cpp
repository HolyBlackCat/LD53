#include "map.h"

#include "game/main.h"

void Map::Load(Stream::Input source)
{
    Json json(source.ReadToMemory().string(), 32);

    points = Tiled::LoadPointLayer(Tiled::FindLayer(json.GetView(), "points"));

    auto layer_mid = Tiled::LoadTileLayer(Tiled::FindLayer(json.GetView(), "mid"));

    cells.resize(ivec2(layer_mid.size()));

    for (ivec2 pos : vector_range(cells.bounds()))
    {
        Tile tile_mid = Tile(layer_mid.safe_nonthrowing_at(pos));
        if (tile_mid < Tile{} || tile_mid >= Tile::_count)
            throw std::runtime_error(FMT("Invalid tile_mid index: {}", int(tile_mid)));

        Cell &target_cell = cells.safe_nonthrowing_at(pos);

        target_cell.mid = tile_mid;
        target_cell.random = ra.i <= 255;
    }

    SetVolumeRect(ivec2().rect_size(cells.size() * tile_size));
}

void Map::Render() const
{
    ivec2 camera_pos = game.get<Camera>()->CameraPos();

    ivec2 corner_a = cells.bounds().clamp(div_ex(camera_pos - screen_size / 2, tile_size));
    ivec2 corner_b = cells.bounds().clamp(div_ex(camera_pos + screen_size / 2, tile_size));

    for (ivec2 tile_pos : corner_a <= vector_range <= corner_b)
    {
        const Cell &cell = cells.safe_nonthrowing_at(tile_pos);
        if (const auto &draw_func = GetTileInfo(cell.mid).draw)
        {
            draw_func(TileInfo::DrawParams{
                .map = *this,
                .tile_pos = tile_pos,
                .cell = cell,
                .layer = &Cell::mid,
                .screen_pos = tile_pos * tile_size - camera_pos,
                .random = cell.random,
                .modify_quad = [](Render::Quad_t &&){},
            });
        }
    }
}

bool Map::IsSolidAtPoint(ivec2 point) const
{
    ivec2 tile_pos = div_ex(point, tile_size);
    if (!cells.pos_in_range(tile_pos))
        return false;
    return GetTileInfo(cells.safe_nonthrowing_at(tile_pos).mid).solid;
}

bool Map::IsSolidAtRect(irect2 rect) const
{
    return Math::for_each_cuboid_point(rect.a, prev_value(rect.b), ivec2(tile_size), nullptr, [this](ivec2 point) -> bool
    {
        return IsSolidAtPoint(point);
    });
}

const TileInfo &GetTileInfo(Tile tile)
{
    if (tile < Tile{} || tile >= Tile::_count)
        throw std::runtime_error("Invalid tile enum.");

    static const auto data = []{
        static auto ShouldMerge = [](Tile self, Tile other) -> bool
        {
            return self == other;
        };

        static auto DrawSimple = [](ivec2 tex_pos)
        {
            return [tex_pos](const TileInfo::DrawParams &params)
            {
                params.modify_quad(r.iquad(params.screen_pos, "tiles"_image with(= (_.a + tex_pos * tile_size).rect_size(tile_size))));
            };
        };

        static auto DrawTiled = [](ivec2 tex_pos)
        {
            return [tex_pos](const TileInfo::DrawParams &params)
            {
                int merge_mask = 0;
                for (int i = 0; i < 8; i++)
                {
                    ivec2 other_tile_pos = params.tile_pos + ivec2::dir8(i);
                    if (!params.map.cells.pos_in_range(other_tile_pos))
                        continue;
                    if (ShouldMerge(params.cell.*params.layer, params.map.cells.safe_nonthrowing_at(other_tile_pos).*params.layer))
                        merge_mask |= 1 << i;
                }

                ivec2 variant;
                if      ((merge_mask & 0b11111111) == 0b11111111) /*##*/ variant = ivec2(1 + params.random % 2, 1);
                else if ((merge_mask & 0b11111101) == 0b11111101) /*|\*/ variant = ivec2(5, 2);
                else if ((merge_mask & 0b11110111) == 0b11110111) /*\|*/ variant = ivec2(4, 2);
                else if ((merge_mask & 0b11011111) == 0b11011111) /*/|*/ variant = ivec2(4, 1);
                else if ((merge_mask & 0b01111111) == 0b01111111) /*|\*/ variant = ivec2(5, 1);
                else if ((merge_mask & 0b00011111) == 0b00011111) /*''*/ variant = ivec2(1 + params.random % 2, 0);
                else if ((merge_mask & 0b01111100) == 0b01111100) /* |*/ variant = ivec2(3, 1);
                else if ((merge_mask & 0b11110001) == 0b11110001) /*..*/ variant = ivec2(1 + params.random % 2, 2);
                else if ((merge_mask & 0b11000111) == 0b11000111) /*| */ variant = ivec2(0, 1);
                else if ((merge_mask & 0b00000111) == 0b00000111) /* /*/ variant = ivec2(0, 0);
                else if ((merge_mask & 0b00011100) == 0b00011100) /*\ */ variant = ivec2(3, 0);
                else if ((merge_mask & 0b01110000) == 0b01110000) /*/ */ variant = ivec2(3, 2);
                else if ((merge_mask & 0b11000001) == 0b11000001) /* \*/ variant = ivec2(0, 2);
                else                                                     variant = ivec2(4 + params.random % 2, 0);

                params.modify_quad(r.iquad(params.screen_pos, "tiles"_image with(= (_.a + (tex_pos + variant) * tile_size).rect_size(tile_size))));
            };
        };

        std::array<TileInfo, int(Tile::_count)> data = {{
            {.tile = Tile::air   , .solid = false, .draw = nullptr               },
            {.tile = Tile::wall  , .solid = true , .draw = DrawTiled(ivec2(0, 1)) },
            {.tile = Tile::bridge, .solid = true , .draw = DrawSimple(ivec2(0, 0))},
        }};

        for (int i = 0; i < int(Tile::_count); i++)
        {
            if (data[i].tile != Tile(i))
                Program::HardError(FMT("Bad tile info at index {}.", i));
        }
        return data;
    }();

    return data[int(tile)];
}
