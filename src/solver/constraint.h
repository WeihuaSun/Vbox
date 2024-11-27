#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "graph/graph.h"

class ItemConstraint;
class PredicateConstraint;

class ItemDirection
{
public:
    ItemDirection(ItemConstraint *parent);

    void insert(uint32_t s, uint32_t t);

    ItemConstraint *parent() const;
    ItemDirection *adversary() const;
    const std::unordered_set<DSG::Edge> &edges() const;

private:
    ItemConstraint *parent_;
    ItemDirection *adversary_;
    std::unordered_set<DSG::Edge> edges_;
    friend class ItemConstraint;
};

class ItemConstraint
{
public:
    ItemConstraint(uint32_t i, uint32_t j);

    const std::unordered_set<DSG::Edge> &alpha_edges() const;
    const std::unordered_set<DSG::Edge> &beta_edges() const;

    ItemDirection *alpha() const;
    ItemDirection *beta() const;

    void insert_alpha(uint32_t s, uint32_t t);
    void insert_beta(uint32_t s, uint32_t t);

    void insert_alpha(const std::unordered_set<DSG::Edge> &edges);
    void insert_beta(const std::unordered_set<DSG::Edge> &edges);

    bool removed() const;
    void set_removed();

    uint32_t i() const{return i_;}
    uint32_t j() const{return j_;}

private:
    uint32_t i_, j_;
    std::unique_ptr<ItemDirection> alpha_;
    std::unique_ptr<ItemDirection> beta_;
    int variable_;
    bool remove_ = false;
};

class PredicateDirection
{
public:
    PredicateDirection(uint32_t v, PredicateConstraint *parent);
    void insert_determined(uint32_t s, uint32_t t);
    void insert_determined(const DSG::Edge &e);
    void insert_undetermined(uint32_t s, uint32_t t);
    void insert_undetermined(const DSG::Edge &e);

    void remove_determined(const DSG::Edge &e);
    void remove_undetermined(const DSG::Edge &e);

    uint32_t v() const;

    PredicateConstraint *parent();
    std::unordered_set<DSG::Edge> &determined_edges();
    std::unordered_set<DSG::Edge> &undetermined_edges();
    DSG::Edge &derivation(const DSG::Edge &e);
    void remove_derivation(const DSG::Edge &e);

private:
    uint32_t v_;
    PredicateConstraint *parent_;
    std::unordered_set<DSG::Edge> determined_edges_;
    std::unordered_set<DSG::Edge> undetermined_edges_;
    std::unordered_map<DSG::Edge, DSG::Edge> derivation_;
    std::unordered_map<DSG::Edge, DSG::Edge> from_derivation_;
};

class PredicateConstraint
{
public:
    PredicateConstraint(uint32_t u);
    PredicateDirection *add(uint32_t v);
    void remove(PredicateDirection *d);
    size_t size() const;
    std::unordered_map<uint32_t, std::unique_ptr<PredicateDirection>> &directions();

private:
    int size_ = 0;
    uint32_t u_; // predicate read trx
    std::unordered_map<uint32_t, std::unique_ptr<PredicateDirection>> directions_;
};

class ConstraintVar
{
public:
    ConstraintVar(ItemConstraint *item_cst, int var);
    ConstraintVar(PredicateDirection *direction, int var);
    ConstraintVar(DSG::Edge e, int var);

    int type() const;
    
    ItemConstraint *constraint() const;
    PredicateDirection *direction() const;
    const std::unordered_set<DSG::Edge> &edges() const;

    int var() const;
    int level() const;
    const DSG::Edge *reason() const;
    bool assign() const;

    // 修改函数
    void set_level(int level);
    void set_reason(const DSG::Edge *reason);
    void set_assign(bool assign);

private:
    int type_;
    ItemConstraint *item_cst_ = nullptr;
    PredicateDirection *direction_ = nullptr;
    std::unordered_set<DSG::Edge> undetermined_edges_;
    int var_ = 0;
    int level_ = 0;
    const DSG::Edge *reason_ = nullptr;
    bool assign_ = false;
};

#endif