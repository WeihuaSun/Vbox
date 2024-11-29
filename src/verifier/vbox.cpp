#include <chrono>

#include "vbox.h"
#include "exception/isolation_exceptions.h"
#include "solver/solver.h"
using namespace std;

void check_edges(unordered_set<DSG::Edge> &edges)
{

    unordered_map<uint32_t, unordered_set<uint32_t>> adjacency;
    for (const DSG::Edge &e : edges)
    {
        adjacency[e.from()].insert(e.to());
    }

    for (auto &entry : adjacency)
    {
        uint32_t s = entry.first;
        for (uint32_t t : entry.second)
        {
            auto it = adjacency[t].find(s);
            if (it != adjacency[t].end())
            {
                cout << s << "\n"
                     << t << endl;
            }
        }
    }
}

Vbox::Vbox(const VerifyOptions &options) : options_(options)
{
    trx_manager_.load(options.log);
    int i = 0;
    for (const unique_ptr<Transaction> &trx : trx_manager_.transactions())
    {
        vertices_.emplace_back(trx.get(), i, i, UINT32_MAX);
        tid2index_[trx->tid()] = i;
        i++;
    }
    n_ = i;
    closure_ = new TransitiveClosure(vertices_, options_);
}

bool Vbox::run()
{
    try
    {
        std::cout << "====================== Execution Statistics ======================\n";
        init();
        generate_item_constraint();
        generate_pred_constraint();
        check_edges(edges_);

        size_t origin_item_cst_num = item_csts_.size();
        size_t origin_pred_cst_num = pred_csts_.size();

        std::cout << "Initial Constraints: \n";
        std::cout << "  Item Constraints:         " << origin_item_cst_num << "\n";
        std::cout << "  Predicate Constraints:    " << origin_pred_cst_num << "\n";
        std::cout << "\n";

        auto construct_start = chrono::high_resolution_clock::now();
        construct_closure();
        auto construct_end = chrono::high_resolution_clock::now();

        auto construct_time = chrono::duration_cast<chrono::microseconds>(construct_end - construct_start).count();
        std::cout << "Closure Construction: \n";
        std::cout << "  Time Taken:               " << construct_time << " us\n";
        std::cout << "\n";

        auto prune_start = chrono::high_resolution_clock::now();
        prune_constraint();
        auto prune_end = chrono::high_resolution_clock::now();

        size_t pruned_item_cst_num = item_csts_.size();
        size_t pruned_pred_cst_num = pred_csts_.size();

        auto prune_time = chrono::duration_cast<chrono::microseconds>(prune_end - prune_start).count();

        std::cout << "Constraint Pruning: \n";
        std::cout << "  Time Taken:               " << prune_time << " us\n";
        std::cout << "  Pruned Item Constraints:  " << pruned_item_cst_num << "\n";
        std::cout << "  Pruned Predicate Constraints: " << pruned_pred_cst_num << "\n";
        std::cout << "\n";

        auto solve_start = chrono::high_resolution_clock::now();
        solve_constraint();
        auto solve_end = chrono::high_resolution_clock::now();
        auto solve_time = chrono::duration_cast<chrono::microseconds>(solve_end - solve_start).count();

        std::cout << "Constraint Solving: \n";
        std::cout << "  Time Taken:               " << solve_time << " us\n";
        std::cout << "\n";

        std::cout << "===============================================================\n";

        return true;
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
        return false;
    }
}

void Vbox::solve_constraint()
{
    bool satisfiable = true;
    if (options_.sat == "monosat")
    {
        cout << "monosat" << endl;
        MonoSolver solver;
        solver.formulate(n_, item_csts_, edges_);
        satisfiable = solver.check();
    }
    else if (options_.sat == "minisat")
    {
        cout << "minisat" << endl;
        MiniSolver solver;
        solver.formulate(item_csts_, edges_);
        satisfiable = solver.check();
    }
    else if (options_.sat == "vboxsat")
    {
        cout << "vboxsat" << endl;
        VboxSolver solver(closure_, vertices_, item_directions_, determined_directions_);
        solver.formulate(item_csts_, pred_csts_);
        satisfiable = solver.check();
    }
    if (!satisfiable)
    {
        throw SerializableException("unsatisfiable.");
    }
}

void Vbox::construct_closure()
{
    closure_->create();
    closure_->construct(edges_);
    // edges_.clear();
}

void Vbox::merge_item_constraint(ItemConstraint &cst)
{
    unordered_set<ItemDirection *> equal_alpha, equal_beta;
    for (const auto &e : cst.alpha_edges())
    {
        auto it = item_directions_.find(e);
        if (it != item_directions_.end())
        {
            ItemDirection *d = it->second;
            equal_alpha.insert(d);
        }
    }

    for (const auto &e : cst.beta_edges())
    {
        auto it = item_directions_.find(e);
        if (it != item_directions_.end())
        {
            ItemDirection *d = it->second;
            equal_beta.insert(d);
            if (equal_alpha.find(d) != equal_alpha.end())
            {
                throw ISException("Merge excption.");
            }
        }
    }

    if (!equal_alpha.empty() || !equal_beta.empty())
    {
        for (ItemDirection *direction : equal_alpha)
        {
            cst.insert_alpha(direction->edges());
            cst.insert_beta(direction->adversary()->edges());
            direction->parent()->set_removed();
        }

        for (ItemDirection *direction : equal_beta)
        {
            cst.insert_alpha(direction->adversary()->edges());
            cst.insert_beta(direction->edges());
            direction->parent()->set_removed();
        }
    }
}

void Vbox::generate_item_constraint()
{
    for (auto &entry : installs_)
    {
        total_item_cst_num_ += ((installs_.size() + 1) * (installs_.size()) / 2);
        uint64_t key = entry.first;
        set<uint32_t> &key_installers = entry.second;
        vector<uint32_t> active_vertices;
        unordered_map<uint32_t, uint64_t> replacement_time;

        active_vertices.push_back(0);
        replacement_time[0] = UINT64_MAX;

        for (uint32_t i : key_installers)
        {
            Vertex &v = vertices_[i];
            auto active_it = active_vertices.begin();
            while (active_it != active_vertices.end())
            {
                uint32_t j = *active_it;
                Vertex &u = vertices_[j];
                if (replacement_time[u.index()] <= v.start())
                {
                    active_it = active_vertices.erase(active_it);
                }
                else if (u.end() <= v.start()) // item-write-dependency (u -> v)
                {
                    ++active_it;
                    if (u.reads().count(key) > 0)
                    {
                        const unordered_set<uint32_t> &read_from_u = u.reads().at(key);
                        for (uint32_t k : read_from_u) // item-read-depends on u with respect to key
                        {
                            //&& i < vertices_[k].right()
                            if (i != k)
                            {
                                edges_.emplace(k, i); // item-anti-dependency
                            }
                        }
                    }
                    replacement_time[u.index()] = min(replacement_time[u.index()], v.end());
                }
                else // overlap in time
                {
                    ++active_it;
                    item_csts_.emplace_back(make_unique<ItemConstraint>(j, i));
                    ItemConstraint &item_cst = *item_csts_.back();
                    //(u -> v)
                    if (u.reads().count(key) > 0)
                    {
                        const unordered_set<uint32_t> &read_from_u = u.reads().at(key);
                        for (uint32_t k : read_from_u)
                        {
                            if (i != k && i < vertices_[k].right())
                            {
                                item_cst.insert_alpha(k, i);
                            }
                        }
                    }
                    //(v -> u)
                    if (v.reads().count(key) > 0)
                    {
                        const unordered_set<uint32_t> &read_from_v = v.reads().at(key);
                        for (uint32_t k : read_from_v)
                        {
                            if (j != k && j < vertices_[k].right())
                            {
                                item_cst.insert_beta(k, j);
                            }
                        }
                    }

                    if (options_.merge)
                    {
                        merge_item_constraint(item_cst);
                    }

                    for (const DSG::Edge &e : item_cst.alpha_edges())
                    {
                        item_directions_[e] = item_cst.alpha();
                    }
                    for (const DSG::Edge &e : item_cst.beta_edges())
                    {
                        item_directions_[e] = item_cst.beta();
                    }
                }
            }
            replacement_time[i] = UINT64_MAX;
            active_vertices.push_back(i);
        }
    }
}

void Vbox::generate_pred_constraint()
{
    // i:write_trx_prev, j:pred_read_trx, k:write_trx_next
    // vertex: u,v,w

    for (size_t j = 0; j < n_; ++j)
    {
        Vertex &v = vertices_[j];
        if (v.predicates().empty())
        {
            continue;
        }
        unordered_map<uint64_t, vector<pair<uint32_t, Write *>>> bound_installs; // key->[(trx,write)]
        for (uint32_t p = v.left(); p < v.right(); ++p)
        {
            const auto &trx_installs = vertices_[p].writes();
            for (const auto &install : trx_installs)
            {
                bound_installs[install.first].push_back(make_pair(p, install.second));
            }
        }
        UnitedPredicate u_pred;
        for (Predicate *p : v.predicates())
        {
            u_pred.add(p);
        }

        for (const auto &entry : bound_installs)
        {
            uint64_t key = entry.first;
            if (u_pred.cover(key))
            {
                continue;
            }
            const vector<pair<uint32_t, Write *>> &key_installers = entry.second;

            pred_csts_.emplace_back(make_unique<PredicateConstraint>(j));
            PredicateConstraint &pred_cst = *pred_csts_.back();

            for (size_t m = 0; m < key_installers.size(); ++m)
            {
                uint32_t i = key_installers[m].first;
                if (i == j)
                {
                    continue;
                }
                Write *write = key_installers[m].second;

                if (!u_pred.relevant(write) || u_pred.match(write))
                {
                    continue;
                }

                PredicateDirection *direction = pred_cst.add(i);
                determined_directions_[DSG::Edge(i, j)].insert(direction); // wr

                // forward
                for (size_t n = m + 1; n < key_installers.size(); ++n)
                {
                    uint32_t k = key_installers[n].first;
                    Write *write_ = key_installers[n].second;
                    if (u_pred.match(write_) && u_pred.relevant(write_))
                    {
                        DSG::Edge rw(j, k);
                        if (vertices_[i].right() <= k) // i ->ww-> k
                        {
                            direction->insert_determined(j, k); // rw
                            determined_directions_[rw].insert(direction);
                        }
                        else
                        {
                            DSG::Edge ww(i, k);
                            direction->insert_undetermined(j, k); // rw
                            undetermined_directions_[rw].insert(direction);
                            re_derivations_[ww].insert(rw);
                        }
                    }
                }
                // backward
                for (int n = m - 1; n >= 0; --n)
                {
                    uint32_t k = key_installers[n].first;
                    Write *write_ = key_installers[n].second;
                    if (k < vertices_[i].left())
                    {
                        break;
                    }
                    else
                    {
                        DSG::Edge rw(j, k);
                        if (u_pred.match(write_) && u_pred.relevant(write_))
                        {
                            DSG::Edge ww(i, k);                 // ww
                            direction->insert_undetermined(rw); // rw
                            undetermined_directions_[rw].insert(direction);
                            re_derivations_[ww].insert(rw);
                        }
                    }
                }
            }
            for (uint32_t i : installs_[key])
            {
                if (i >= v.left())
                {
                    break;
                }
                Write *write_ = vertices_[i].writes().at(key);
                if (!u_pred.match(write_) && u_pred.relevant(write_))
                {
                    PredicateDirection *direction = pred_cst.add(0);
                    // determined_directions_[DSG::Edge(0, j)].insert(direction); // wr
                    for (size_t m = 0; m < key_installers.size(); ++m)
                    {
                        uint32_t k = key_installers[m].first;
                        if (k == j || vertices_[j].right() <= k)
                        {
                            continue;
                        }
                        Write *write__ = key_installers[m].second;
                        if (u_pred.match(write__) && u_pred.relevant(write__))
                        {
                            DSG::Edge rw(j, k);
                            direction->insert_determined(j, k); // rw
                            determined_directions_[rw].insert(direction);
                        }
                    }
                    break;
                }
            }
            if (pred_cst.size() == 0)
            {
                pred_csts_.pop_back();
            }
            else if (pred_cst.size() == 1)
            {
                PredicateDirection *d = pred_cst.directions().begin()->second.get();
                for (const auto &e : d->determined_edges())
                {
                    edges_.insert(e);
                    determined_directions_[e].erase(d);
                    if (determined_directions_[e].empty())
                    {
                        determined_directions_.erase(e);
                    }
                }

                for (const DSG::Edge &e : d->undetermined_edges())
                {
                    DSG::Edge derivation = pred_cst.directions().begin()->second->derivation(e);
                    undetermined_directions_[e].erase(d);
                    if (undetermined_directions_[e].empty())
                    {
                        undetermined_directions_.erase(e);
                    }
                    re_derivations_[derivation].erase(e);

                    if (item_directions_.count(e) == 0)
                    {
                        item_directions_[e] = item_directions_[derivation];
                        item_directions_[e]->insert(e.from(), e.to());
                    }
                    else // merge
                    {
                        for (const DSG::Edge &e_ : item_directions_[e]->edges())
                        {
                            item_directions_[e_] = item_directions_[derivation];
                            item_directions_[derivation]->insert(e_.from(), e_.to());
                        }
                        for (const DSG::Edge &e_ : item_directions_[e]->adversary()->edges())
                        {
                            item_directions_[e_] = item_directions_[derivation]->adversary();
                            item_directions_[derivation]->adversary()->insert(e_.from(), e_.to());
                        }
                    }
                }
                pred_csts_.pop_back();
            }
        }
    }
}

void Vbox::prune_constraint()
{
    if (options_.prune == "prune")
    {
        prune();
    }
    else if (options_.prune == "prune_opt")
    {
        prune_opt();
    }
}

void Vbox::prune()
{
    queue<DSG::Edge> edge_queue;
}

bool Vbox::contain_cycle(const unordered_set<DSG::Edge> &edges) const
{
    return any_of(edges.begin(), edges.end(),
                  [this](const DSG::Edge &e)
                  {
                      return closure_->reach(e.to(), e.from());
                  });
}

void Vbox::prune_item_first(unordered_set<DSG::Edge> &edge_queue)
{
    auto item_cst_it = item_csts_.begin();
    while (item_cst_it != item_csts_.end())
    {
        ItemConstraint *item_cst = item_cst_it->get();
        if (item_cst->removed())
        {
            item_cst_it = item_csts_.erase(item_cst_it);
            continue;
        }
        bool cycle_alpha = contain_cycle(item_cst->alpha_edges());
        bool cycle_beta = contain_cycle(item_cst->beta_edges());
        if (cycle_alpha && cycle_beta)
        {
            throw SerializableException("prune: both alpha and beta contain cycles");
        }
        if (cycle_alpha || cycle_beta)
        {
            auto &accept_edges = cycle_alpha ? item_cst->beta_edges() : item_cst->alpha_edges();
            auto &reject_edges = cycle_alpha ? item_cst->alpha_edges() : item_cst->beta_edges();

            for (const DSG::Edge &e : accept_edges)
            {
                edge_queue.insert(e);
                if (options_.collect)
                {
                    edges_.insert(e);
                }
                item_directions_.erase(e);
            }
            for (const auto &e : reject_edges)
            {
                item_directions_.erase(e);
            }

            item_cst_it = item_csts_.erase(item_cst_it);
        }
        else
        {
            ++item_cst_it;
        }
    }
}

void Vbox::prune_pred_first(unordered_set<DSG::Edge> &edge_queue)
{
    auto cst_it = pred_csts_.begin();
    while (cst_it != pred_csts_.end())
    {
        PredicateConstraint *cst = cst_it->get();
        for (auto dir_it = cst->directions().begin(); dir_it != cst->directions().end();)
        {
            PredicateDirection *direction = dir_it->second.get();
            for (auto e_it = direction->undetermined_edges().begin(); e_it != direction->undetermined_edges().end();)
            {
                const DSG::Edge &edge = *e_it;
                DSG::Edge &derivation = direction->derivation(edge);

                if (closure_->reach(edge.from(), edge.to()))
                {
                    e_it = direction->undetermined_edges().erase(e_it);

                    direction->remove_derivation(edge);
                    re_derivations_[derivation].erase(edge);
                    undetermined_directions_[edge].erase(direction);
                }
                else if (closure_->reach(edge.to(), edge.from()))
                {
                    e_it = direction->undetermined_edges().erase(e_it);

                    direction->remove_derivation(edge);
                    re_derivations_[derivation].erase(edge);
                    undetermined_directions_[edge].erase(direction);
                }
                else
                {
                    if (closure_->reach(derivation.from(), derivation.to()))
                    {
                        e_it = direction->undetermined_edges().erase(e_it);

                        direction->remove_derivation(edge);
                        re_derivations_[derivation].erase(edge);
                        undetermined_directions_[edge].erase(direction);

                        direction->insert_determined(edge);
                        determined_directions_[edge].insert(direction);

                        for (const DSG::Edge &derived_edge : re_derivations_[derivation])
                        {
                            for (PredicateDirection *direction_ : undetermined_directions_[derived_edge])
                            {
                                direction_->remove_undetermined(derived_edge);

                                direction_->insert_determined(derived_edge);
                                determined_directions_[derived_edge].insert(direction_);
                            }
                            undetermined_directions_.erase(derived_edge);
                        }
                        re_derivations_.erase(derivation);
                    }
                    else if (closure_->reach(derivation.from(), derivation.to()))
                    {
                        e_it = direction->undetermined_edges().erase(e_it);
                        direction->remove_derivation(edge);
                        re_derivations_[derivation].erase(edge);
                        undetermined_directions_[edge].erase(direction);

                        for (const DSG::Edge &derived_edge : re_derivations_[derivation])
                        {
                            for (PredicateDirection *direction_ : undetermined_directions_[derived_edge])
                            {
                                direction_->remove_undetermined(derived_edge);
                            }
                            undetermined_directions_.erase(derived_edge);
                        }
                        re_derivations_.erase(derivation);
                    }
                    else
                    {
                        ++e_it;
                    }
                }
            }

            bool reject_dir = false;
            // prune determined edges
            for (auto e_it = direction->determined_edges().begin(); e_it != direction->determined_edges().end();)
            {
                const DSG::Edge &edge = *e_it;
                if (closure_->reach(edge.from(), edge.to()))
                {

                    determined_directions_[edge].erase(direction);
                    if (determined_directions_[edge].empty())
                    {
                        determined_directions_.erase(edge);
                    }
                    e_it = direction->determined_edges().erase(e_it);
                }
                else if (closure_->reach(edge.to(), edge.from()))
                {

                    determined_directions_[edge].erase(direction);
                    if (determined_directions_[edge].empty())
                    {
                        determined_directions_.erase(edge);
                    }
                    reject_dir = true;
                    e_it = direction->determined_edges().erase(e_it);
                }
                else
                {
                    ++e_it;
                }
            }
            if (reject_dir)
            {
                for (const auto &edge : direction->determined_edges())
                {
                    determined_directions_[edge].erase(direction);
                    if (determined_directions_[edge].empty())
                    {
                        determined_directions_.erase(edge);
                    }
                }
                dir_it = cst->directions().erase(dir_it);
            }
            else
            {
                ++dir_it;
            }
        }

        if (cst->size() == 0) // 剪枝后
        {
            throw SerializableException("prune predicate constraints error.");
            // cst_it = pred_csts_.erase(cst_it);
        }
        else if (cst->size() == 1)
        {
            PredicateDirection *direction = cst->directions().begin()->second.get();
            for (const DSG::Edge &e : direction->determined_edges())
            {
                edge_queue.insert(e);
                if (options_.collect)
                {
                    edges_.insert(e);
                }
                determined_directions_[e].erase(direction);
                if (determined_directions_[e].empty())
                {
                    determined_directions_.erase(e);
                }
            }
            for (const DSG::Edge &e : direction->undetermined_edges())
            {
                DSG::Edge derivation = cst->directions().begin()->second->derivation(e);
                undetermined_directions_[e].erase(direction);
                if (undetermined_directions_[e].empty())
                {
                    undetermined_directions_.erase(e);
                }
                re_derivations_[derivation].erase(e);

                if (item_directions_.count(e) == 0)
                {
                    item_directions_[e] = item_directions_[derivation];
                    item_directions_[e]->insert(e.from(), e.to());
                }
                else // merge
                {
                    for (const DSG::Edge &e_ : item_directions_[e]->edges())
                    {
                        item_directions_[e_] = item_directions_[derivation];
                        item_directions_[derivation]->insert(e_.from(), e_.to());
                    }
                    for (const DSG::Edge &e_ : item_directions_[e]->adversary()->edges())
                    {
                        item_directions_[e_] = item_directions_[derivation]->adversary();
                        item_directions_[derivation]->adversary()->insert(e_.from(), e_.to());
                    }
                }
            }
            cst_it = pred_csts_.erase(cst_it);
        }
        else
        {
            ++cst_it;
        }
    }
}

void Vbox::prune_opt()
{
    unordered_set<DSG::Edge> edge_queue;
    prune_item_first(edge_queue);
    prune_pred_first(edge_queue);
    while (!edge_queue.empty())
    {
        DSG::Edge e = *edge_queue.begin();
        edge_queue.erase(e);
        if (closure_->reach(e.to(), e.from()))
        {
            throw SerializableException("prune");
        }
        vector<DSG::Edge> change = closure_->insert(e);

        for (const DSG::Edge &accept : change)
        {
            DSG::Edge reject = DSG::Edge(accept.to(), accept.from());
            // prune item constraint
            auto item_dir_it = item_directions_.find(reject);
            if (item_dir_it != item_directions_.end())
            {
                for (const DSG::Edge &acc : item_dir_it->second->adversary()->edges())
                {
                    edge_queue.insert(acc);
                    if (options_.collect)
                    {
                        edges_.insert(e);
                    }
                    item_directions_.erase(item_dir_it);
                }
                for (const DSG::Edge &rej : item_dir_it->second->edges())
                {
                    item_directions_.erase(rej);
                }
            }
            // prune predicate determined edge
            auto pred_ddir_it = determined_directions_.find(reject);
            if (pred_ddir_it != determined_directions_.end())
            {
                for (PredicateDirection *rej : pred_ddir_it->second)
                {
                    for (const DSG::Edge &e : rej->determined_edges())
                    {
                        determined_directions_[e].erase(rej);
                        if (determined_directions_[e].empty())
                        {
                            determined_directions_.erase(e);
                        }
                    }
                    PredicateConstraint *parent = rej->parent();
                    parent->remove(rej);
                    if (parent->size() == 0)
                    {
                        throw SerializableException("prune predicate constraints error.");
                    }
                    if (parent->size() == 1)
                    {
                        PredicateDirection *direction = parent->directions().begin()->second.get();
                        for (const DSG::Edge &e : direction->determined_edges())
                        {
                            edge_queue.insert(e);
                            if (options_.collect)
                            {
                                edges_.insert(e);
                            }
                            determined_directions_[e].erase(direction);
                            if (determined_directions_[e].empty())
                            {
                                determined_directions_.erase(e);
                            }
                        }
                        for (const DSG::Edge &e : direction->undetermined_edges())
                        {
                            DSG::Edge derivation = rej->parent()->directions().begin()->second->derivation(e);
                            undetermined_directions_[e].erase(direction);
                            if (undetermined_directions_[e].empty())
                            {
                                undetermined_directions_.erase(e);
                            }
                            re_derivations_[derivation].erase(e);

                            if (item_directions_.count(e) == 0)
                            {
                                item_directions_[e] = item_directions_[derivation];
                                item_directions_[e]->insert(e.from(), e.to());
                            }
                            else
                            {
                                for (const DSG::Edge &e_ : item_directions_[e]->edges())
                                {
                                    item_directions_[e_] = item_directions_[derivation];
                                    item_directions_[derivation]->insert(e_.from(), e_.to());
                                }
                                for (const DSG::Edge &e_ : item_directions_[e]->adversary()->edges())
                                {
                                    item_directions_[e_] = item_directions_[derivation]->adversary();
                                    item_directions_[derivation]->adversary()->insert(e_.from(), e_.to());
                                }
                            }
                        }
                    }
                }
            }
            // prune predicate undetermined edge
            auto pred_udir_it_r = undetermined_directions_.find(reject);
            if (pred_udir_it_r != undetermined_directions_.end())
            {
                for (PredicateDirection *d : pred_udir_it_r->second)
                {
                    d->remove_undetermined(reject);
                    re_derivations_[d->derivation(reject)].erase(reject);
                }
                undetermined_directions_.erase(reject);
            }
            auto pred_udir_it_a = undetermined_directions_.find(accept);
            if (pred_udir_it_a != undetermined_directions_.end())
            {
                for (PredicateDirection *d : pred_udir_it_a->second)
                {
                    d->remove_undetermined(accept);
                    d->insert_determined(accept);
                    re_derivations_[d->derivation(accept)].erase(accept);
                    determined_directions_[accept].insert(d);
                }
                undetermined_directions_.erase(accept);
            }

            auto re_der_it_r = re_derivations_.find(reject);
            if (re_der_it_r != re_derivations_.end())
            {
                for (const DSG::Edge edge : re_derivations_.at(reject))
                {
                    for (PredicateDirection *d : undetermined_directions_[edge])
                    {
                        d->remove_undetermined(reject);
                    }
                    undetermined_directions_.erase(edge);
                }
                re_derivations_.erase(re_der_it_r);
            }

            auto re_der_it_a = re_derivations_.find(accept);
            if (re_der_it_a != re_derivations_.end())
            {
                for (const DSG::Edge edge : re_derivations_.at(accept))
                {
                    for (PredicateDirection *d : undetermined_directions_[edge])
                    {
                        d->remove_undetermined(accept);
                        d->insert_determined(accept);
                        determined_directions_[accept].insert(d);
                    }
                    undetermined_directions_.erase(edge);
                }
                re_derivations_.erase(re_der_it_r);
            }
        }
    }
}

void Vbox::check_read(vector<Read *> &reads)
{
    for (Read *read : reads)
    {
        if (read->from_tid() != 0)
        {
            if (tid2index_.count(read->from_tid()) == 0)
            {
                throw AbortedReads(read);
            }
            auto &installs = vertices_[tid2index_[read->from_tid()]].writes();
            if (installs.count(read->key()) == 0 || installs.at(read->key())->oid() != read->from_oid())
            {
                throw IntermediateReads(vertices_[tid2index_[read->from_tid()]].transaction(), read);
            }
        }
    }
}

void Vbox::init()
{
    vector<Read *> reads;
    for (size_t i = 0; i < n_; ++i)
    {
        Vertex &v = vertices_[i];
        Transaction *trx = v.transaction();
        for (const unique_ptr<Operator> &op : trx->operators())
        {
            switch (op->type())
            {
            case OperatorType::READ:
            {
                Read *read = static_cast<Read *>(op.get());
                reads.push_back(read);
                // assert(tid2index_.count(read->from_tid())>0);
                Vertex &from = vertices_[tid2index_[read->from_tid()]];
                from.set_read(read->key(), i);
                edges_.emplace(from.index(), i); // wr
            }
            break;
            case OperatorType::WRITE:
            {
                Write *write = static_cast<Write *>(op.get());
                installs_[write->key()].insert(i);
                v.set_write(write->key(), write);
            }
            break;
            case OperatorType::PREDICATE:
            {
                Predicate *predicate = static_cast<Predicate *>(op.get());
                v.set_predicate(predicate);
            }
            break;
            default:
                break;
            }
        }
    }
    check_read(reads);
    reads.clear();
    vector<uint32_t> active_vertices;
    for (size_t j = 0; j < n_; ++j)
    {
        Vertex &v = vertices_[j];
        auto it = active_vertices.begin();
        while (it != active_vertices.end())
        {
            Vertex &u = vertices_[*it];
            if (u.end() <= v.start())
            {
                it = active_vertices.erase(it);
                u.set_right(j);
                if (options_.collect)
                {
                    edges_.emplace(u.index(), v.index());
                }
            }
            else
            {
                ++it;
                v.set_left(u.index());
            }
        }
        active_vertices.push_back(j);
    }
    for (uint32_t i : active_vertices)
    {
        vertices_[i].set_right(n_);
    }
}
