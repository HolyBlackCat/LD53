#pragma once

#include "entities.h"
#include "main.h"
#include "map.h"

struct Parcel : Tickable, Renderable, SolidPhysicsRect, Game::LinkOne<"carried_by">
{
    IMP_STANDALONE_COMPONENT(Game)

    // If this is true, won't collide with the player, until we stop overlapping at least once.
    bool skipping_player_collisions = false;

    irect2 PhysicsRoughRelativeHitbox() const override
    {
        return ivec2().centered_rect_size(ivec2(24, 15));
    }

    float PhysicsMass() const override
    {
        return 2;
    }

    // Disable some interactions while we're being carried.
    bool PhysicsEnabled() const override
    {
        return !game.has_link<"carried_by">(*this);
    }

    bool CheckCollisionWithSolidEntity(ivec2 self_pos, const Solid &s) const override;

    void Tick() override;
    void Render() const override;
};

struct Player : Tickable, Renderable, SolidPhysicsRect, Camera, Game::LinkOne<"carries">
{
    IMP_STANDALONE_COMPONENT(Game)

    Input::ButtonList control_up    = {{Input::w, Input::up   }};
    Input::ButtonList control_down  = {{Input::s, Input::down }};
    Input::ButtonList control_left  = {{Input::a, Input::left }};
    Input::ButtonList control_right = {{Input::d, Input::right}};
    Input::ButtonList control_jump  = {{Input::j, Input::x    , Input::space}};
    Input::ButtonList control_act   = {{Input::k, Input::z    }};

    bool facing_left = false;
    int walking_time = -1;

    bool NowCarrying() const
    {
        return game.has_link<"carries">(*this);
    }
    bool NowCarryingOrOverlapping() const
    {
        if (NowCarrying())
            return true;
        if (auto &p = game.get<Parcel>(); p && p->skipping_player_collisions)
            return true;
        return false;
    }

    ivec2 CameraPos() const override
    {
        return Pos();
    }

    irect2 HitboxWithoutParcel() const
    {
        return ivec2(-7,-15).rect_to(ivec2(6, 12));
    }
    irect2 HitboxWithParcel() const
    {
        return ivec2(-7,-29).rect_to(ivec2(6, 12));
    }

    irect2 PhysicsRoughRelativeHitbox() const override
    {
        // We just use the larger hitbox here, since it's only used for the broad phase.
        return HitboxWithParcel();
    }

    std::optional<irect2> SolidSimpleRectHitbox() const override
    {
        return Pos() + (NowCarrying() ? HitboxWithParcel() : HitboxWithoutParcel());
    }

    bool CheckCollisionWithSolidEntity(ivec2 self_pos, const Solid &s) const override;

    void Tick() override;
    void Render() const override;
};
