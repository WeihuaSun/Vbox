#include "constraint.h"

using namespace std;
using DSG::Edge;

ItemDirection::ItemDirection(ItemConstraint *parent) : parent_(parent) {}
void ItemDirection::insert(uint32_t s, uint32_t t) { edges_.emplace(s, t); }

ItemConstraint *ItemDirection::parent() const { return parent_; }
ItemDirection *ItemDirection::adversary() const { return adversary_; }
const unordered_set<::Edge> &ItemDirection::edges() const { return edges_; }

////////////////////////////////////////////////////////////////////////////////

ItemConstraint ::ItemConstraint(uint32_t i, uint32_t j) : i_(i), j_(j)
{
    alpha_ = make_unique<ItemDirection>(this);
    beta_ = make_unique<ItemDirection>(this);
    alpha_->adversary_ = beta_.get();
    beta_->adversary_ = alpha_.get();
    alpha_->insert(i, j);
    beta_->insert(j, i);
}

ItemDirection *ItemConstraint::alpha() const { return alpha_.get(); }
ItemDirection *ItemConstraint::beta() const { return beta_.get(); }

const std::unordered_set<::Edge> &ItemConstraint::alpha_edges() const { return alpha_->edges(); }
const std::unordered_set<::Edge> &ItemConstraint::beta_edges() const { return beta_->edges(); }

void ItemConstraint::insert_alpha(uint32_t s, uint32_t t) { alpha_->insert(s, t); }
void ItemConstraint::insert_beta(uint32_t s, uint32_t t) { beta_->insert(s, t); }

void ItemConstraint::insert_alpha(const std::unordered_set<::Edge> &edges)
{
    for (const ::Edge &e : edges)
    {
        alpha_->insert(e.from(), e.to());
    }
}
void ItemConstraint::insert_beta(const std::unordered_set<::Edge> &edges)
{
    for (const ::Edge &e : edges)
    {
        beta_->insert(e.from(), e.to());
    }
}

bool ItemConstraint::removed() const { return remove_; }
void ItemConstraint::set_removed() { remove_ = true; }

///////////////////////////////////////////////////////////////////////////////////

PredicateDirection::PredicateDirection(uint32_t v, PredicateConstraint *parent) : v_(v), parent_(parent) {}

void PredicateDirection::insert_determined(uint32_t s, uint32_t t)
{
    determined_edges_.emplace(s, t);
}

void PredicateDirection::insert_determined(const ::Edge &e)
{
    determined_edges_.emplace(e);
}

void PredicateDirection::remove_determined(const ::Edge &e)
{
    determined_edges_.erase(e);
}

void PredicateDirection::remove_undetermined(const ::Edge &e)
{
    undetermined_edges_.erase(e);
    derivation_.erase(e);
}

void PredicateDirection::insert_undetermined(uint32_t s, uint32_t t)
{
    undetermined_edges_.emplace(s, t);
    derivation_[::Edge(s, t)] = ::Edge(v_, t);
}

void PredicateDirection::insert_undetermined(const ::Edge &e)
{
    undetermined_edges_.emplace(e.from(), e.to());
    derivation_[e] = ::Edge(v_, e.to());
}

uint32_t PredicateDirection::v() const { return v_; }

PredicateConstraint *PredicateDirection::parent() { return parent_; }
std::unordered_set<::Edge> &PredicateDirection::determined_edges() { return determined_edges_; }
std::unordered_set<::Edge> &PredicateDirection::undetermined_edges() { return undetermined_edges_; }
::Edge &PredicateDirection::derivation(const ::Edge &e) { return derivation_.at(e); }
void PredicateDirection::remove_derivation(const ::Edge &e) { derivation_.erase(e); }

////////////////////////////////////////////////////////////////////////////////////

PredicateConstraint::PredicateConstraint(uint32_t u) : u_(u) {}
PredicateDirection *PredicateConstraint::add(uint32_t v)
{
    directions_[v] = std::make_unique<PredicateDirection>(v, this);
    if (v != 0)
    {
        directions_[v]->insert_determined(v, u_); // determined write dependency
    }
    return directions_[v].get();
}
void PredicateConstraint::remove(PredicateDirection *d)
{
    directions_.erase(d->v());
}

size_t PredicateConstraint::size() const
{
    return directions_.size();
}
unordered_map<uint32_t, unique_ptr<PredicateDirection>> &PredicateConstraint::directions() { return directions_; }

/////////////////////////////////////////////////////////////////

ConstraintVar::ConstraintVar(ItemConstraint *item_cst, int var) : type_(0), item_cst_(item_cst), var_(var) {}
ConstraintVar::ConstraintVar(PredicateDirection *direction, int var) : type_(1), direction_(direction), var_(var) {}
ConstraintVar::ConstraintVar(::Edge e, int var) : type_(2), var_(var) { undetermined_edges_.emplace(e); }

int ConstraintVar::type() const { return type_; }
ItemConstraint *ConstraintVar::constraint() const { return item_cst_; }
PredicateDirection *ConstraintVar::direction() const { return direction_; }
const std::unordered_set<DSG::Edge> &ConstraintVar::edges() const { return undetermined_edges_; }
int ConstraintVar::level() const { return level_; }
int ConstraintVar::var() const { return var_; }
const DSG::Edge *ConstraintVar::reason() const { return reason_; }
bool ConstraintVar::assign() const { return assign_; }

void ConstraintVar::set_level(int level) { level_ = level; }
void ConstraintVar::set_reason(const DSG::Edge *reason) { reason_ = reason; }
void ConstraintVar::set_assign(bool assign) { assign_ = assign; }
