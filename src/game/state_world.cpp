#include "main.h"

#include "entities.h"
#include "map.h"

struct Player : Tickable, Renderable, SolidPhysicsRect, Camera
{
    IMP_STANDALONE_COMPONENT(Game)

    Input::ButtonList control_left = {{Input::a, Input::left}};
    Input::ButtonList control_right = {{Input::d, Input::right}};
    Input::ButtonList control_jump = {{Input::j, Input::space, Input::x}};

    ivec2 CameraPos() const override
    {
        return Pos();
    }

    irect2 PhysicsRelativeHitbox() const override
    {
        return ivec2().centered_rect_size(ivec2(14,24));
    }

    void Tick() override
    {
        constexpr float
            walk_acc   = 0.3f,
            walk_speed = 2.f,
            walk_dec   = 0.3f,
            jump_speed = 3.2f,
            short_jump_drag = 0.94f;

        int hc = control_right.down() - control_left.down();
        if (hc)
            clamp_var_abs(vel.x += hc * walk_acc, walk_speed);
        else if (abs(vel.x) > walk_dec)
            vel.x -= sign(vel.x) * walk_dec;
        else
            vel.x = 0;

        if (ground && control_jump.pressed())
            vel.y = -jump_speed;
        if (!ground && !control_jump.down() && vel.y < 0)
            vel.y *= short_jump_drag;
    }

    void Render() const override
    {
        ivec2 pixel_pos = Pos() - game.get<Camera>()->CameraPos();
        if (pixel_pos.abs()(any) > screen_size / 2 + tile_size * 2)
            return;
        r.iquad(pixel_pos, "player"_image).center();
    }
};

struct Box : Tickable, Renderable, SolidPhysicsRect
{
    irect2 PhysicsRelativeHitbox() const override
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
            vel.x *= 0.95f;
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

            game.create<Player>().SetPos(iround(map.points.GetSinglePoint("player")));

            map.points.ForEachPointNamed("box", [](fvec2 pos)
            {
                auto &box = game.create<Box>();
                box.SetPos(iround(pos));
            });
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
                ivec2 remaining_move_sub;
                std::pair<int, float> initiative{};
                ivec2 new_pos_sub;

                std::vector<OtherSolid> overlaps;
            };

            struct ImpulseEntry
            {
                Physics *target = nullptr;

                std::vector<OtherSolid> overlaps;
            };

            const auto &bvh_tree = game.get<BvhTree>()->tree_sub;

            // Update `vel_lag` and collect physics objects.
            // Also apply gravity.
            std::vector<Entry> remaining_targets;
            remaining_targets.reserve(game.get<AllPhysics>().size());
            for (auto &e : game.get<AllPhysics>())
            {
                Physics &ph = e.get<Physics>();
                ph.vel.y += gravity;
                ivec2 move_sub = round_with_compensation(ph.vel * subpixel_resolution, ph.vel_lag);
                ph.vel_lag = next_value_towards(ph.vel_lag, 0);

                remaining_targets.push_back({.target = &ph, .remaining_move_sub = move_sub, .initiative = {move_sub.abs().max(), ph.vel.max()}, .new_pos_sub = ph.PosSub()});

                int self_index = -1;
                if (auto solid = e.get_opt<Solid>())
                    self_index = solid->GetBvhTreeIndex();

                bvh_tree.CollideAabb((ph.PosSub() + ph.PhysicsRelativeHitboxSub()).expand_dir(move_sub).expand(1), [&](int index)
                {
                    if (index != self_index)
                    {
                        auto &e = game.get(bvh_tree.GetNodeUserData(index));
                        remaining_targets.back().overlaps.push_back({.s = &e.get<Solid>(), .p = e.get_opt<Physics>()});
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
            std::erase_if(remaining_targets, [](const Entry &e){return !e.remaining_move_sub;});

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
                        ivec2 step_sub = sign(entry.remaining_move_sub.only_component(axis));
                        if (!step_sub)
                            continue;

                        bool collides = false;
                        for (const auto &overlap : entry.overlaps)
                        {
                            if (overlap.s->IsSolidAtRectSub(entry.new_pos_sub + entry.target->PhysicsRelativeHitboxSub() + step_sub))
                            {
                                collides = true;
                                break;
                            }
                        }

                        if (!collides)
                        {
                            had_progress = true;
                            entry.new_pos_sub += step_sub;
                            entry.remaining_move_sub -= step_sub;
                        }
                    }

                    // Erase entities that ran out of speed.
                    if (entry.remaining_move_sub)
                    {
                        if (pos != i)
                            remaining_targets[pos] = std::move(remaining_targets[i]);
                        pos++;
                    }
                    else
                    {
                        entry.target->SetPosSub(entry.new_pos_sub);
                    }
                }
                remaining_targets.resize(pos);
            }
            for (auto &entry : remaining_targets)
            {
                entry.target->SetPosSub(entry.new_pos_sub);
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
                        if (other.s->IsSolidAtRectSub(entry.target->PosSub() + entry.target->PhysicsRelativeHitboxSub() + dir))
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
