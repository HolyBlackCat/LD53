#include "player.h"

bool Parcel::CheckCollisionWithSolidEntity(ivec2 self_pos, const Solid &s) const
{
    if (skipping_player_collisions)
    {
        if (&s == &*game.get<Player>())
            return false;
    }
    return s.IsSolidAtRect(self_pos + PhysicsRoughRelativeHitbox());
}

void Parcel::Tick()
{
    if (ground)
        vel.x *= 0.85f;

    if (skipping_player_collisions)
    {
        // Temporarily reset to make `CheckCollisionWithSolidEntity` do its thing.
        skipping_player_collisions = false;
        if (CheckCollisionWithSolidEntity(Pos(), *game.get<Player>()))
            skipping_player_collisions = true;
    }
}

void Parcel::Render() const
{
    ivec2 pixel_pos = Pos() - game.get<Camera>()->CameraPos();
    if (pixel_pos.abs()(any) > screen_size / 2 + tile_size * 2)
        return;
    r.iquad(pixel_pos, "parcel"_image).center();
}

bool Player::CheckCollisionWithSolidEntity(ivec2 self_pos, const Solid &s) const
{
    // Ignore our own parcel.
    if (NowCarrying() && &s == &game.get_link<"carries">(*this).get<Solid>())
        return false;
    // Ignore parcel while it still overlaps us after being thrown.
    if (!NowCarrying())
    {
        if (auto &p = game.get<Parcel>(); p && p->skipping_player_collisions && &*p == &s)
            return false;
    }
    return s.IsSolidAtRect(self_pos + SolidSimpleRectHitbox().value() - Pos());
}

void Player::Tick()
{
    constexpr float
        walk_acc            = 0.3f,
        walk_speed          = 2.f,
        walk_speed_carrying = 1.f,
        walk_dec            = 0.3f,
        jump_speed          = 3.2f,
        jump_speed_carrying = 2.1f,
        short_jump_drag     = 0.94f;

    constexpr int
        // When the parcel is thrown down, it's moved down by this offset to make it intersect with the player, to make it pass through.
        parcel_throw_offset_down = 2;

    constexpr ivec2
        parcel_pickup_extra_hitbox_size(8);

    constexpr fvec2
        parcel_throw_vel     (3.f ,-0.5f),
        parcel_throw_vel_up  (0   ,-3.f),
        parcel_throw_vel_down(0   , 0.5f);

    { // Walk.
        int hc = control_right.down() - control_left.down();
        if (hc)
            clamp_var_abs(vel.x += hc * walk_acc, (NowCarrying() ? walk_speed_carrying : walk_speed));
        else if (abs(vel.x) > walk_dec)
            vel.x -= sign(vel.x) * walk_dec;
        else
            vel.x = 0;

        { // Update walk animation.
            if (hc)
                facing_left = hc < 0;
            if (abs(vel.x) > 0.6f)
                walking_time++;
            else
                walking_time = -1;
        }
    }

    { // Jump.
        if (ground && control_jump.pressed())
            vel.y = -(NowCarrying() ? jump_speed_carrying : jump_speed);
        if (!ground && !control_jump.down() && vel.y < 0)
            vel.y *= short_jump_drag;
    }

    { // Picking up and dropping the parcel.
        if (control_act.pressed())
        {
            if (!NowCarrying())
            {
                auto &parcel = game.get<Parcel>();
                if (parcel && (Pos() + HitboxWithoutParcel()).expand(parcel_pickup_extra_hitbox_size).touches(parcel->Pos() + parcel->PhysicsRoughRelativeHitbox()))
                {
                    game.link<"carries", "carried_by">(*this, *parcel);
                }
            }
            else
            {
                int throw_control = control_down.down() - control_up.down();

                auto &parcel = game.get_link<"carries">(*this);
                auto &parcel_ph = parcel.get<Physics>();

                ivec2 desired_parsel_pos = parcel_ph.Pos() with(.y += throw_control > 0 ? parcel_throw_offset_down : 0);

                if (!parcel_ph.CheckCollisionWithWorld(desired_parsel_pos, {}, [&](const Game::Entity &e){return e.get_opt<Player>() != this;}))
                {
                    parcel_ph.SetPos(desired_parsel_pos);
                    if (throw_control > 0)
                        parcel.get<Parcel>().skipping_player_collisions = true;
                    game.unlink<"carries">(*this);

                    fvec2 throw_vel = throw_control < 0 ? parcel_throw_vel_up : throw_control > 0 ? parcel_throw_vel_down : parcel_throw_vel;
                    parcel_ph.vel = vel + throw_vel with(.x *= (facing_left ? -1 : 1));
                }
            }
        }
    }

    { // Updating parcel position to match ours.
        if (NowCarrying())
        {
            game.get_link<"carries">(*this).get<Physics>().SetPos(Pos() with(.y -= 24));
        }
    }
}

void Player::Render() const
{
    constexpr ivec2 sprite_size(32);

    ivec2 pixel_pos = Pos() - game.get<Camera>()->CameraPos();
    if (pixel_pos.abs()(any) > screen_size / 2 + sprite_size)
        return;

    int variant = 0;
    int frame = 0;

    if (!ground)
    {
        variant = 2;
        frame = vel.y < -0.5f ? 0 : vel.y < 0.5f ? 1 : 2;
    }
    else if (walking_time >= 0)
    {
        variant = 1;
        frame = walking_time / 12 % 5;
    }
    else
    {
        variant = 0;
    }

    r.iquad(pixel_pos with(.y -= 4), "player"_image with(= _.a + (ivec2(frame, variant) * sprite_size).rect_size(sprite_size))).center().flip_x(facing_left);
}
