#pragma once

struct Game : Ent::BasicTag<Game,
    Ent::Mixins::ComponentsAsCategories,
    Ent::Mixins::GlobalEntityLists,
    Ent::Mixins::EntityCallbacks,
    Ent::Mixins::EntityLinks
> {};

extern Game::Controller game;

struct Camera
{
    IMP_STANDALONE_COMPONENT(Game)

    [[nodiscard]] virtual ivec2 CameraPos() const = 0;
};

struct Tickable
{
    IMP_COMPONENT(Game)

    virtual void Tick() = 0;
};
using AllTickable = Game::Category<Ent::OrderedList, Tickable>;

struct Renderable
{
    IMP_COMPONENT(Game)

    virtual void Render() const = 0;
};
using AllRenderable = Game::Category<Ent::OrderedList, Renderable>;

// A global bounding-volume-hierarchy tree for physics objects.
struct BvhTree
{
    IMP_STANDALONE_COMPONENT(Game)

    AabbTree<ivec2, Game::Id> tree;

    BvhTree()
    {
        decltype(tree)::Params params(ivec2(8));
        params.velocity_margin_factor = ivec2(4);
        // params.balance_threshold = 2;
        tree = params;
    }
};

struct StoredInBvhTree
{
    IMP_COMPONENT(Game)

    virtual ~StoredInBvhTree() = default;

  private:
    struct State
    {
        int tree_index = -1;
        irect2 aabb;
    };
    std::optional<State> state;

  protected:
    void SetVolumeRect(irect2 rect, ivec2 vel = {})
    {
        if (state)
            game.get<BvhTree>()->tree.ModifyNode(state->tree_index, rect, vel);
        else
            state.emplace(State{.tree_index = game.get<BvhTree>()->tree.AddNode(rect, dynamic_cast<Game::Entity &>(*this).id()), .aabb = rect});
    }
    void ResetVolumeRect()
    {
        if (state)
        {
            if (auto &tree = game.get<BvhTree>())
                tree->tree.RemoveNode(state->tree_index);
            state.reset();
        }
    }

  public:
    // Automatically removes the entity from the tree.
    void _deinit(Game::Controller &, Game::Entity &)
    {
        ResetVolumeRect();
    }

    [[nodiscard]] const std::optional<irect2> VolumeRect() const
    {
        if (state)
            return state->aabb;
        else
            return {};
    }

    [[nodiscard]] int GetBvhTreeIndex() const {return state ? state->tree_index : -1;}
};

struct Solid : StoredInBvhTree
{
    IMP_COMPONENT(Game)

    [[nodiscard]] virtual bool IsSolidAtPoint(ivec2 point) const = 0;
    [[nodiscard]] virtual bool IsSolidAtRect(irect2 rect) const = 0;
};

struct Physics
{
    IMP_COMPONENT(Game)

  private:
    ivec2 pos;

  public:
    fvec2 vel;
    fvec2 vel_lag;

    bool ground = false;

    virtual void SetPos(ivec2 new_pos) {pos = new_pos;}
    [[nodiscard]] ivec2 Pos() const {return pos;}

    [[nodiscard]] virtual bool PhysicsEnabled() const {return true;}

    // The approximate hitbox (for the AABB tree), relative to `pos`.
    [[nodiscard]] virtual irect2 PhysicsRoughRelativeHitbox() const = 0;

    // Matters when transfering momentum between entities.
    virtual float PhysicsMass() const {return 1;}

    // Check collision with `s`.
    [[nodiscard]] virtual bool CheckCollisionWithSolidEntity(ivec2 self_pos, const Solid &s) const {return s.IsSolidAtRect(self_pos + PhysicsRoughRelativeHitbox());}

    [[nodiscard]] bool CheckCollisionWithWorld(std::optional<ivec2> self_pos_override, std::optional<irect2> hitbox_override) const
    {
        irect2 hitbox = (self_pos_override ? *self_pos_override : Pos()) + (hitbox_override ? *hitbox_override : PhysicsRoughRelativeHitbox());
        const auto &tree = game.get<BvhTree>()->tree;
        return tree.CollideAabb(hitbox, [&](int index)
        {
            auto &s = game.get(tree.GetNodeUserData(index)).get<Solid>();
            return s.IsSolidAtRect(hitbox);
        });
    }
};
using AllPhysics = Game::Category<Ent::OrderedList, Physics>;

struct SolidRect : Solid
{
    IMP_COMPONENT(Game)

    virtual std::optional<irect2> SolidSimpleRectHitbox() const = 0;

    bool IsSolidAtPoint(ivec2 point) const override final
    {
        auto r = SolidSimpleRectHitbox();
        return r && r->contains(point);
    }
    bool IsSolidAtRect(irect2 rect) const override final
    {
        auto r = SolidSimpleRectHitbox();
        return r && r->touches(rect);
    }
};

struct SolidPhysicsRect : Physics, SolidRect
{
    IMP_COMPONENT(Game)

    void SetPos(ivec2 new_pos) override
    {
        Physics::SetPos(new_pos);
        SetVolumeRect(Pos() + PhysicsRoughRelativeHitbox());
    }

    std::optional<irect2> SolidSimpleRectHitbox() const override
    {
        return Pos() + PhysicsRoughRelativeHitbox();
    }
};
