#include "solver.h"
#include "graph/graph.h"

using namespace std;
using DSG::Edge;

VboxSolver::VboxSolver()
{
}

void VboxSolver::formulate(vector<unique_ptr<ItemConstraint>> &item_csts,
                           vector<unique_ptr<PredicateConstraint>> &pred_csts)
{
    for (const unique_ptr<ItemConstraint> &cst : item_csts)
    {
        int var = newVar(true, true);
        vars_.emplace_back(cst.get(), var);
    }

    for (const unique_ptr<PredicateConstraint> &cst : pred_csts)
    {
        vector<int> tmp_vars;
        for (auto it = cst->directions().begin(); it != cst->directions().end(); ++it)
        {
            PredicateDirection *direction = it->second.get();
            int var1 = newVar(true, true); // edge set var
            tmp_vars.push_back(var1);
            vars_.emplace_back(direction, var1);

            for (const ::Edge &e : direction->undetermined_edges())
            {
                int var2 = newVar(true, true);
                ItemDirection *derivation = item_directions_[direction->derivation(e)];
                int var0 = cst_from_var_[derivation->parent()];
                bool sign0 = (derivation->parent()->alpha() == derivation);
                //(A or B)<->C equals to (not A or not B or C) and (not C or A) and (not C or B)
                addClause(mkLit(var2, false), mkLit(var1, true), mkLit(var0, sign0)); //(not A or not B or C)
            }
        }
        // Only one of B1, B2, B3.. is assigned True
        vec<Lit> ps; //(B1 or B2 or B3...)
        for (size_t i = 0; i < vars_.size(); ++i)
        {
            ps.push(mkLit(tmp_vars[i], false));
            for (size_t j = i + 1; j < tmp_vars.size(); ++j)
            {
                addClause(mkLit(tmp_vars[i], true), mkLit(tmp_vars[j], true)); // not Bi or not Bj
            }
        }
        addClause(ps);
    }
}

size_t VboxSolver::decision_level() const { return v_trail_lim_.size(); }

void VboxSolver::v_propagate(unordered_set<ConstraintVar *> &reason)
{
    int sat_trail_size;
    size_t vbox_trail_size;
    do
    {
        sat_trail_size = trail.size();
        vbox_trail_size = v_trail_.size();

        Monosat::CRef sat_conflict = propagate();
        if (sat_conflict != Monosat::CRef_Undef)
        {
            sat_calc_reason(reason, sat_conflict);
            return;
        }
        else
        {
            for (int i = sat_trail_size; i < trail.size(); ++i)
            {
                Monosat::Lit p = trail[i];
                int v = var(p);
                ConstraintVar &var = vars_[v];
                var.set_assign(assigns[v] == l_true);
                var.set_level(decision_level());
                var.set_reason(nullptr);
                v_trail_.push_back(&var);
                unassigned_.erase(&var);
            }
        }

        while (v_head_ < v_trail_.size())
        {
            ConstraintVar *var = v_trail_[v_head_++];
            unordered_set<::Edge> accept;
            if (var->type() == 0)
            {
                accept = var->assign() ? var->constraint()->alpha_edges() : var->constraint()->beta_edges();
            }
            else if (var->type() == 1)
            {
                accept = var->direction()->determined_edges();
            }
            else
            {
                accept = var->edges();
            }
            for (const ::Edge &e : accept)
            {
                if (closure_->reach(e.to(), e.from()))
                {
                    // need assign?
                    reason.insert(var);
                    v_calc_reason(reason, &e);
                    return;
                }
                const auto &changes = closure_->insert(e);
                for (const ::Edge &changedEdge : changes)
                {
                    ::Edge reject(changedEdge.to(), changedEdge.from());
                    record_[decision_level()].push_back(changedEdge);

                    auto item_it = item_directions_.find(reject);
                    if (item_it != item_directions_.end())
                    {
                        ItemConstraint *parent = item_it->second->parent();
                        ConstraintVar &p_var = vars_[cst_from_var_[parent]];
                        p_var.set_assign(item_it->second == parent->beta());
                        p_var.set_level(decision_level());
                        p_var.set_reason(&(item_it->first));
                        uncheckedEnqueue(Monosat::mkLit(p_var.var(), p_var.assign()));
                        v_trail_.push_back(&p_var);
                        unassigned_.erase(&p_var);
                    }

                    auto dDIt = determined_directions_.find(reject);
                    if (dDIt != determined_directions_.end())
                    {
                        unordered_set<PredicateDirection *> &directions = dDIt->second;
                        for (PredicateDirection *direction : directions)
                        {
                            ConstraintVar p_var = vars_[dir_from_var_[direction]];
                            p_var.set_assign(false);
                            p_var.set_level(decision_level());
                            p_var.set_reason(&(dDIt->first));
                        }
                    }
                }
            }
        }

    } while (sat_trail_size != trail.size() && vbox_trail_size != v_trail_.size());
}

int VboxSolver::v_analyze(unordered_set<ConstraintVar *> &reason, vector<ConstraintVar *> &learned)
{
    int back_level = 0;
    int count = 0;
    ConstraintVar *conflict;
    learned.push_back(nullptr);
    unordered_set<ConstraintVar *> seen;
    int index = v_trail_.size() - 1;
    do
    {
        if (reason.empty())
        {
            if (conflict->reason() == nullptr)
            {
                sat_calc_reason(reason, conflict->var());
                reason.erase(conflict);
            }
            else
            {
                v_calc_reason(reason, conflict->reason());
            }
        }
        for (ConstraintVar *var : reason)
        {
            if (seen.find(var) == seen.end())
            {
                seen.insert(var);
                if (var->level() == (int)decision_level())
                {
                    count++;
                }
                else if (var->level() > 0)
                {
                    learned.push_back(var);
                    back_level = max(back_level, var->level());
                }
            }
        }
        while (seen.find(v_trail_[index--]) == seen.end())
        {
            conflict = v_trail_[index + 1];
            count--;
        }
        reason.clear();
    } while (count > 0);
    learned[0] = conflict;
    return back_level;
}

void VboxSolver::sat_calc_reason(unordered_set<ConstraintVar *> &reason, Monosat::CRef conflict)
{
    Monosat::Clause &c = ca[conflict];
    for (int i = 0; i < c.size(); i++)
    {
        auto p = c[i];
        reason.insert(&vars_[var(p)]);
    }
}

void VboxSolver::v_calc_reason(unordered_set<ConstraintVar *> &reason, const ::Edge *reason_edge)
{
    vector<const ::Edge *> path = closure_->path(reason_edge->from(), reason_edge->to());
    for (const ::Edge *e : path)
    {
        auto it = item_directions_.find(*e);
        if (it != item_directions_.end())
        {
            reason.insert(&vars_[cst_from_var_[it->second->parent()]]);
        }
    }
}

void VboxSolver::v_backtrace(int bk_level)
{
    if (decisionLevel() > bk_level)
    {
        for (int c = v_trail_.size() - 1; c >= this->v_trail_lim_[bk_level]; c--)
        {
            ConstraintVar *var = v_trail_.back();
            var->set_assign(true);
            var->set_level(-1);
            var->set_reason(nullptr);
            v_trail_.pop_back();
            unassigned_.insert(var);
            assigns[var->var()] = l_undef;
            trail.pop();
            qhead--;
        }
        for (int c = decisionLevel() - 1; c >= bk_level; c--)
        {
            v_trail_lim_.pop_back();
            auto &record = record_.back();
            record_.pop_back();
            closure_->backtrace(record);
        }
    }
}

bool VboxSolver::check()
{
    ConstraintVar *var = nullptr;
    unordered_set<ConstraintVar *> reason;
    while (!unassigned_.empty())
    {
        reason.clear();
        v_propagate(reason);
        if (reason.size() > 0)
        {
            if (decision_level() == 0)
            {
                return false;
            }
            else
            {
                vector<ConstraintVar *> learned;
                int bk_level = v_analyze(reason, learned);
                Monosat::vec<Monosat::Lit> sat_clause;
                for (ConstraintVar *var : learned)
                {
                    sat_clause.push(Monosat::mkLit(var->var(), var->assign()));
                }
                addClause(sat_clause);
                v_backtrace(bk_level);
            }
        }
        else
        {
            v_trail_lim_.push_back(v_trail_.size());
            var = *unassigned_.begin();
            unassigned_.erase(unassigned_.begin());
            var->set_assign(true);
            var->set_level(decision_level());
            v_trail_.push_back(var);
        }
    }
    return true;
}

MiniSolver::MiniSolver() { sat_solver_ = newSolver(); }

void MiniSolver::formulate(const vector<unique_ptr<ItemConstraint>> &item_csts, const vector<::Edge> &edges)
{
    vector<Var> edge_variables;

    for (size_t i = 0; i < item_csts.size(); ++i)
    {
        edge_variables.push_back(sat_solver_->newVar());
    }

    unordered_map<int, unordered_map<int, Var>> topo_order;

    auto getTopoOrderVar = [&](int u, int v) -> Var
    {
        if (topo_order[u].find(v) == topo_order[u].end())
        {
            topo_order[u][v] = sat_solver_->newVar();
        }
        return topo_order[u][v];
    };

    for (const ::Edge &e : edges)
    {

        Var var_from = getTopoOrderVar(e.from(), e.to());
        Var var_to = getTopoOrderVar(e.from(), e.to());

        sat_solver_->addClause(mkLit(var_from, false));
        sat_solver_->addClause(mkLit(var_to, true));
    }
    int i = 0;
    for (const unique_ptr<ItemConstraint> &cst : item_csts)
    {
        for (const ::Edge &e : cst->alpha_edges())
        {
            Var var_from = getTopoOrderVar(e.from(), e.to());
            Var var_to = getTopoOrderVar(e.to(), e.from());

            sat_solver_->addClause(mkLit(var_from, false), mkLit(edge_variables[i], true)); // 选择 alpha 边集时，from 在 to 之前
            sat_solver_->addClause(mkLit(var_to, true), mkLit(edge_variables[i], true));    // 选择 alpha 边集时，to 不在 from 之前
        }

        for (const auto &e : cst->beta_edges())
        {

            Var var_from = getTopoOrderVar(e.from(), e.to());
            Var var_to = getTopoOrderVar(e.to(), e.from());

            sat_solver_->addClause(mkLit(var_from, false), mkLit(edge_variables[i], false));
            sat_solver_->addClause(mkLit(var_to, true), mkLit(edge_variables[i], false));
        }

        i++;
    }
}

bool MiniSolver::check()
{
    return sat_solver_->solve();
}

void MiniSolver::clear() { sat_solver_ = newSolver(); }

/////////////////////////////////////////////////////////////////////////////////////////////////

MonoSolver::MonoSolver()
{
    sat_solver_ = newSolver();
    graph_solver_ = newGraph(sat_solver_);
}

void MonoSolver::formulate(size_t n, const vector<unique_ptr<ItemConstraint>> &item_csts, const vector<::Edge> &edges)
{
    graph_solver_->newNodes(n);

    // formulate known edges
    for (const ::Edge &e : edges)
    {
        int lit = newEdge(sat_solver_, graph_solver_, e.from(), e.to(), 1);
        addClause(sat_solver_, &lit, 1);
    }

    // formulate item constraints
    for (const unique_ptr<ItemConstraint> &cst : item_csts)
    {
        int var_alpha = newVar(sat_solver_);
        int var_beta = newVar(sat_solver_);

        int a[2] = {varToLit(var_alpha, true), 0};
        int b[2] = {varToLit(var_beta, true), 0};
        for (const ::Edge &e : cst->alpha_edges())
        {
            int lit = newEdge(sat_solver_, graph_solver_, e.from(), e.to(), 1);
            a[1] = lit;
            addClause(sat_solver_, a, 2);
        }

        for (const ::Edge &e : cst->beta_edges())
        {
            int lit = newEdge(sat_solver_, graph_solver_, e.from(), e.to(), 1);
            b[1] = lit;
            addClause(sat_solver_, b, 2);
        }

        int n[2] = {varToLit(var_alpha, true), varToLit(var_beta, true)};
        int p[2] = {varToLit(var_alpha, false), varToLit(var_beta, false)};
        addClause(sat_solver_, n, 2);
        addClause(sat_solver_, p, 2);
    }

    // formulate acyclic
    acyclic_directed(sat_solver_, graph_solver_);
}

bool MonoSolver::check()
{
    return solve(sat_solver_);
}

void MonoSolver::clear()
{
    sat_solver_ = newSolver();
    graph_solver_ = newGraph(sat_solver_);
}

//////////////////////////////////////////////////////////////////////////////////////////
