
#include "constraint.h"
#include "graph/graph.h"
#include "graph/transitive_closure.h"
#include <monosat/api/Monosat.h>
#include <memory>
#include <vector>

#define l_true (Monosat::lbool((uint8_t)0))
#define l_false (Monosat::lbool((uint8_t)1))
#define l_undef (Monosat::lbool((uint8_t)2))

class VboxSolver : public Monosat::Solver
{
public:
    VboxSolver();
    void formulate(std::vector<std::unique_ptr<ItemConstraint>> &item_csts, std::vector<std::unique_ptr<PredicateConstraint>> &pred_csts);
    bool check();
    void clear();
    void v_propagate(std::unordered_set<ConstraintVar *> &reason);
    int v_analyze(std::unordered_set<ConstraintVar *> &reason, std::vector<ConstraintVar *> &learned);
    size_t decision_level() const;

    void sat_calc_reason(std::unordered_set<ConstraintVar *> &reason, Monosat::CRef conflict);
    void vbox_calc_reason(std::unordered_set<ConstraintVar *> &reason, const DSG::Edge *reason_edge);

    std::vector<ConstraintVar *> v_trail_;
    size_t v_head_=0;
    std::vector<std::vector<DSG::Edge>> record_;

private:
    std::vector<ConstraintVar> vars_;
    std::unordered_set<ConstraintVar *> unassigned_;
    std::unordered_map<ItemConstraint *, int> from_var_;
    std::vector<int> v_trail_lim_;
    TransitiveClosure *closure_;
    std::vector<Vertex> vertices_;
    std::unordered_map<DSG::Edge, ItemDirection *> item_directions_;
    std::unordered_map<DSG::Edge, std::unordered_set<PredicateDirection *>> determined_directions_;
    std::unordered_map<DSG::Edge, std::unordered_set<PredicateDirection *>> undetermined_directions_;
    std::unordered_map<DSG::Edge, std::unordered_set<DSG::Edge>> re_derivations_;
};

class MiniSolver
{
public:
    MiniSolver();
    void formulate(const std::vector<std::unique_ptr<ItemConstraint>> &item_csts, const std::vector<DSG::Edge> &edges);
    bool check();
    void clear();

private:
    SolverPtr sat_solver_;
};

class MonoSolver
{
public:
    MonoSolver();
    void formulate(size_t n, const std::vector<std::unique_ptr<ItemConstraint>> &item_csts, const std::vector<DSG::Edge> &edges);
    bool check();
    void clear();

private:
    SolverPtr sat_solver_;
    GraphTheorySolver_long graph_solver_;
};