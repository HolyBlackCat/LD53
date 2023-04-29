#include "main.h"

#include "entities.h"
#include "map.h"

struct Parcel : Tickable, Renderable, SolidPhysicsRect, Game::LinkOne<"carried_by">
{
    IMP_STANDALONE_COMPONENT(Game)

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
    std::optional<irect2> SolidSimpleRectHitbox() const override
    {
        return !game.has_link<"carried_by">(*this) ? SolidPhysicsRect::SolidSimpleRectHitbox() : std::nullopt;
    }

    void Tick() override
    {
        if (ground)
            vel.x *= 0.85f;
    }

    void Render() const override
    {
        ivec2 pixel_pos = Pos() - game.get<Camera>()->CameraPos();
        if (pixel_pos.abs()(any) > screen_size / 2 + tile_size * 2)
            return;
        r.iquad(pixel_pos, "parcel"_image).center();
    }
};

struct Player : Tickable, Renderable, SolidPhysicsRect, Camera, Game::LinkOne<"carries">
{
    IMP_STANDALONE_COMPONENT(Game)

    Input::ButtonList control_left = {{Input::a, Input::left}};
    Input::ButtonList control_right = {{Input::d, Input::right}};
    Input::ButtonList control_jump = {{Input::j, Input::x, Input::space}};
    Input::ButtonList control_act = {{Input::k, Input::z}};

    bool facing_left = false;
    int walking_time = -1;

    bool NowCarrying() const
    {
        return game.has_link<"carries">(*this);
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

    bool CheckCollisionWithSolidEntity(ivec2 self_pos, const Solid &s) const override
    {
        // Ignore our own box.
        if (NowCarrying() && &s == &game.get_link<"carries">(*this).get<Solid>())
            return false;
        return s.IsSolidAtRect(self_pos + SolidSimpleRectHitbox().value() - Pos());
    }

    void Tick() override
    {
        constexpr float
            walk_acc            = 0.3f,
            walk_speed          = 2.f,
            walk_speed_carrying = 1.f,
            walk_dec            = 0.3f,
            jump_speed          = 3.2f,
            jump_speed_carrying = 2.1f,
            short_jump_drag     = 0.94f;

        constexpr ivec2
            parcel_pickup_extra_hitbox_size(8);

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
                    auto &parcel = game.get_link<"carries">(*this);
                    if (!parcel.get<Physics>().CheckCollisionWithWorld({}, {}))
                        game.unlink<"carries">(*this);
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

    void Render() const override
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
};

struct Box : Tickable, Renderable, SolidPhysicsRect
{
    irect2 PhysicsRoughRelativeHitbox() const override
    {
        return ivec2().centered_rect_size(ivec2(12));
    }

    float PhysicsMass() const override
    {
        return 4;
    }

    void Tick() override
    {
        if (ground)
            vel.x *= 0.85f;
    }

    void Render() const override
    {
        ivec2 pixel_pos = Pos() - game.get<Camera>()->CameraPos();
        if (pixel_pos.abs()(any) > screen_size / 2 + tile_size * 2)
            return;
        r.iquad(pixel_pos, "boxes"_image with(= _.a.rect_size(tile_size))).center();
    }
};

namespace States
{
    STRUCT( World EXTENDS StateBase )
    {
        MEMBERS()

        void Init() override
        {
            // Entities.
            game = nullptr;

            // Configure the audio.
            float audio_distance = screen_size.x * 3;
            Audio::ListenerPosition(fvec3(0, 0, -audio_distance));
            Audio::ListenerOrientation(fvec3(0,0,1), fvec3(0,-1,0));
            Audio::Source::DefaultRefDistance(audio_distance);

            game.create<BvhTree>();
            auto &map = game.create<Map>();
            map.Load(Program::ExeDir() + "assets/map.json");


            map.points.ForEachPointNamed("box", [](fvec2 pos)
            {
                auto &box = game.create<Box>();
                box.SetPos(iround(pos));
            });

            if (auto parcel_pos = map.points.GetSinglePointOpt("parcel"))
            {
                game.create<Parcel>().SetPos(iround(*parcel_pos) with(.y += 4));
            }
            game.create<Player>().SetPos(iround(map.points.GetSinglePoint("player")));
        }

        void TickPhysics()
        {
            constexpr float gravity = 0.08f;

            struct OtherSolid
            {
                Solid *s = nullptr;
                Physics *p = nullptr; // Can be null.
            };

            struct Entry
            {
                Physics *target = nullptr;
                ivec2 remaining_move;
                std::pair<int, float> initiative{};
                ivec2 new_pos;

                std::vector<OtherSolid> overlaps;
            };

            struct ImpulseEntry
            {
                Physics *target = nullptr;

                std::vector<OtherSolid> overlaps;
            };

            const auto &bvh_tree = game.get<BvhTree>()->tree;

            // Update `vel_lag` and collect physics objects.
            // Also apply gravity.
            std::vector<Entry> remaining_targets;
            remaining_targets.reserve(game.get<AllPhysics>().size());
            for (auto &e : game.get<AllPhysics>())
            {
                Physics &ph = e.get<Physics>();
                if (!ph.PhysicsEnabled())
                    continue;
                ph.vel.y += gravity;
                ivec2 move = round_with_compensation(ph.vel, ph.vel_lag);
                ph.vel_lag = next_value_towards(ph.vel_lag, 0);

                remaining_targets.push_back({.target = &ph, .remaining_move = move, .initiative = {move.abs().max(), ph.vel.max()}, .new_pos = ph.Pos()});

                int self_index = -1;
                if (auto solid = e.get_opt<Solid>())
                    self_index = solid->GetBvhTreeIndex();

                bvh_tree.CollideAabb((ph.Pos() + ph.PhysicsRoughRelativeHitbox()).expand_dir(move).expand(1), [&](int index)
                {
                    if (index != self_index)
                    {
                        auto &e = game.get(bvh_tree.GetNodeUserData(index));

                        OtherSolid other{.s = &e.get<Solid>(), .p = e.get_opt<Physics>()};
                        if (other.p && !other.p->PhysicsEnabled())
                            other.p = nullptr;
                        remaining_targets.back().overlaps.push_back(other);
                    }
                    return false;
                });
            }
            // Sort objects by initiative.
            std::stable_sort(remaining_targets.begin(), remaining_targets.end(), [](const Entry &a, const Entry &b)
            {
                return a.initiative > b.initiative;
            });

            // Collect impulse targets.
            std::vector<ImpulseEntry> impulse_targets;
            impulse_targets.reserve(game.get<AllPhysics>().size());
            for (const auto &entry : remaining_targets)
            {
                impulse_targets.push_back({.target = entry.target, .overlaps = entry.overlaps});
            }

            // Strip non-moving objects from targets.
            std::erase_if(remaining_targets, [](const Entry &e){return !e.remaining_move;});

            // Move objects.
            bool had_progress = true;
            while (had_progress)
            {
                had_progress = false;

                std::size_t pos = 0;
                for (std::size_t i = 0; i < remaining_targets.size(); i++)
                {
                    Entry &entry = remaining_targets[i];

                    for (int axis = 0; axis < 2; axis++)
                    {
                        ivec2 step = sign(entry.remaining_move.only_component(axis));
                        if (!step)
                            continue;

                        bool collides = false;
                        for (const auto &overlap : entry.overlaps)
                        {
                            if (entry.target->CheckCollisionWithSolidEntity(entry.new_pos + step, *overlap.s))
                            {
                                collides = true;
                                break;
                            }
                        }

                        if (!collides)
                        {
                            had_progress = true;
                            entry.new_pos += step;
                            entry.remaining_move -= step;
                        }
                    }

                    // Erase entities that ran out of speed.
                    if (entry.remaining_move)
                    {
                        if (pos != i)
                            remaining_targets[pos] = std::move(remaining_targets[i]);
                        pos++;
                    }
                    else
                    {
                        entry.target->SetPos(entry.new_pos);
                    }
                }
                remaining_targets.resize(pos);
            }
            for (auto &entry : remaining_targets)
            {
                entry.target->SetPos(entry.new_pos);
            }
            remaining_targets.clear();

            // Transfer impulse.
            auto TransferImpulse = [](Physics &target, int axis, const OtherSolid &other)
            {
                ivec2 dir = sign(target.vel.only_component(axis));
                int axis_sign = sign(dir[axis]);

                if (axis == 1 && axis_sign == 1)
                    target.ground = true;

                if (target.vel[axis] * axis_sign > (other.p ? other.p->vel[axis] * axis_sign : 0))
                {
                    float common_vel = 0;
                    if (other.p)
                    {
                        float mass_sum = target.PhysicsMass() + other.p->PhysicsMass();
                        common_vel = (target.vel[axis] * target.PhysicsMass() + other.p->vel[axis] * other.p->PhysicsMass()) / mass_sum;
                        other.p->vel[axis] = common_vel;

                        other.p->vel_lag = target.vel_lag = (other.p->vel_lag + target.vel_lag) / 2;
                    }
                    target.vel[axis] = common_vel;
                }
            };

            for (ImpulseEntry &entry : impulse_targets) LOOP_NAME(impulse_transfer)
            {
                entry.target->ground = false;

                for (const auto &other : entry.overlaps)
                {
                    for (int axis = 0; axis < 2; axis++)
                    {
                        ivec2 dir = sign(entry.target->vel.only_component(axis));
                        if (entry.target->CheckCollisionWithSolidEntity(entry.target->Pos() + dir, *other.s))
                            TransferImpulse(*entry.target, axis, other);
                    }
                }
            }
        }

        void Tick(std::string &next_state) override
        {
            (void)next_state;

            TickPhysics();

            for (auto &e : game.get<AllTickable>())
                e.get<Tickable>().Tick();
        }

        void Render() const override
        {
            Graphics::SetClearColor(fvec3(0));
            Graphics::Clear();

            r.BindShader();

            for (auto &e : game.get<AllRenderable>())
                e.get<Renderable>().Render();

            r.Finish();
        }
    };
}
