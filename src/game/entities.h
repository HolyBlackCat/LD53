#pragma once

constexpr int subpixel_resolution = 1;
constexpr int subpixel_hitbox_shrinkage = 0;

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

    AabbTree<ivec2, Game::Id> tree_sub;

    BvhTree()
    {
        decltype(tree_sub)::Params params(ivec2(8 * subpixel_resolution));
        params.velocity_margin_factor = ivec2(4);
        // params.balance_threshold = 2;
        tree_sub = params;
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
    void SetVolumeRectSub(irect2 rect, ivec2 vel = {})
    {
        if (state)
            game.get<BvhTree>()->tree_sub.ModifyNode(state->tree_index, rect, vel);
        else
            state.emplace(State{.tree_index = game.get<BvhTree>()->tree_sub.AddNode(rect, dynamic_cast<Game::Entity &>(*this).id()), .aabb = rect});
    }
    void ResetVolumeRect()
    {
        if (state)
        {
            if (auto &tree = game.get<BvhTree>())
                tree->tree_sub.RemoveNode(state->tree_index);
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

    [[nodiscard]] virtual bool IsSolidAtPointSub(ivec2 point) const = 0;
    [[nodiscard]] virtual bool IsSolidAtRectSub(irect2 rect) const = 0;
};

struct Physics
{
    IMP_COMPONENT(Game)

  private:
    ivec2 pos_subpixels;

  public:
    fvec2 vel;
    fvec2 vel_lag;

    bool ground = false;

    virtual void SetPosSub(ivec2 new_pos_subpixels) {pos_subpixels = new_pos_subpixels;}
    void SetPos(ivec2 new_pos) {SetPosSub(new_pos * subpixel_resolution);}
    [[nodiscard]] ivec2 PosSub() const {return pos_subpixels;}
    [[nodiscard]] ivec2 Pos() const {return div_ex(pos_subpixels, subpixel_resolution);}

    // The hitbox, relative to `pos`.
    [[nodiscard]] virtual irect2 PhysicsRelativeHitbox() const = 0;
    [[nodiscard]] irect2 PhysicsRelativeHitboxSub() const {return (PhysicsRelativeHitbox() * subpixel_resolution).shrink(subpixel_hitbox_shrinkage);}

    // Matters when transfering momentum between entities.
    virtual float PhysicsMass() const {return 1;}

    // Whether the physics should respect a specific solid entity.
    [[nodiscard]] virtual bool PhysicsRespectsSolidEntity(const Game::Entity &e) {(void)e; return true;}
};
using AllPhysics = Game::Category<Ent::OrderedList, Physics>;

struct SolidPhysicsRect : Physics, Solid
{
    IMP_COMPONENT(Game)

    void SetPosSub(ivec2 new_pos_subpixels) override
    {
        if (new_pos_subpixels != Pos())
        {
            Physics::SetPosSub(new_pos_subpixels);
            SetVolumeRectSub(PosSub() + PhysicsRelativeHitboxSub());
        }
    }

    bool IsSolidAtPointSub(ivec2 point) const override
    {
        return (PosSub() + PhysicsRelativeHitboxSub()).contains(point);
    }
    bool IsSolidAtRectSub(irect2 rect) const override
    {
        return (PosSub() + PhysicsRelativeHitboxSub()).touches(rect);
    }
};
