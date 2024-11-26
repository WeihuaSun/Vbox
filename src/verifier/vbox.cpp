#include "vbox.h"
#include "exception/isolation_exceptions.h"

using namespace std;
using DSG::Edge;

Vbox::Vbox(const VerifyOptions &options)
{
    trx_manager_.load(options.log);
    int i = 0;
    for (const unique_ptr<Transaction> &trx : trx_manager_.transactions())
    {
        vertices_.emplace_back(trx.get(), i);
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
        init();
        generate_item_constraint();
        return true;
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
        return false;
    }
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
        uint64_t key = entry.first;
        vector<uint32_t> &key_installers = entry.second;
        vector<uint32_t> active_vertices;
        unordered_map<uint32_t, uint64_t> replacement_time;
        for (uint32_t i : key_installers)
        {
            Vertex &v = vertices_[i];
            auto active_it = active_vertices.begin();
            while (active_it != active_vertices.end())
            {
                uint32_t j = *active_it;
                Vertex u = vertices_[j];
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
                            if (i != k && i < vertices_[k].right())
                            {
                                edges_.emplace_back(k, i); // item-anti-dependency
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

                    for (const Edge &e : item_cst.alpha_edges())
                    {
                        item_directions_[e] = item_cst.alpha();
                    }
                    for (const Edge &e : item_cst.beta_edges())
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
    for (size_t i = 0; i < n_; ++i)
    {
        Vertex &v = vertices_[i];
        if (v.predicates().empty())
        {
            continue;
        }
        unordered_map<uint64_t, vector<pair<uint32_t, Write *>>> bound_installs; // key->[(trx,write)]
        for (uint32_t j = v.left(); j < v.right(); ++j)
        {
            const auto &trx_installs = vertices_[j].writes();
            for (const auto &install : trx_installs)
            {
                bound_installs[install.first].push_back(make_pair(j, install.second));
            }
        }

        for (Predicate *p : v.predicates())
        {
            for (const auto &entry : bound_installs)
            {
                uint64_t key = entry.first;
                const vector<pair<uint32_t, Write *>> &key_installers = entry.second;

                pred_csts_.emplace_back(make_unique<PredicateConstraint>(i));
                PredicateConstraint &pred_cst = *pred_csts_.back();

                for (size_t m = 0; m < key_installers.size(); ++m)
                {
                    uint32_t j = key_installers[m].first;
                    if (j == i)
                    {
                        continue;
                    }
                    Write *w = key_installers[m].second;

                    if (!p->relevant(w) || p->match(w))
                    {
                        continue;
                    }

                    PredicateDirection *direction = pred_cst.add(j);
                    determined_directions_[Edge(j, i)].insert(direction); // wr

                    // forward
                    for (size_t n = m + 1; n < key_installers.size(); ++n)
                    {
                        uint64_t k = key_installers[n].first;
                        Write *w_ = key_installers[n].second;
                        if (p->match(w_) && p->relevant(w_))
                        {
                            Edge rw(i, k);
                            if (vertices_[j].right() <= k) // ww
                            {
                                direction->insert_determined(i, k); // rw
                                determined_directions_[rw].insert(direction);
                            }
                            else
                            {
                                Edge ww(j, k);
                                direction->insert_undetermined(i, k); // rw
                                undetermined_directions_[rw].insert(direction);
                                re_derivations_[ww].insert(rw);
                            }
                        }
                    }
                    // backward
                    for (int n = m - 1; n >= 0; --n)
                    {
                        uint32_t k = key_installers[n].first;
                        Write *w_ = key_installers[n].second;
                        if (k < vertices_[j].left())
                        {
                            break;
                        }
                        else
                        {
                            Edge rw(i, k);
                            if (p->match(w_) && p->relevant(w_))
                            {
                                Edge ww(j, k);                      // ww
                                direction->insert_undetermined(rw); // rw
                                undetermined_directions_[rw].insert(direction);
                                re_derivations_[ww].insert(rw);
                            }
                        }
                    }
                }

                bool left_candidate = false;

                for (uint32_t j : installs_[key])
                {
                    if (j >= v.left())
                    {
                        break;
                    }
                    Write *w = vertices_[j].writes().at(key);
                    if (!p->match(w) && p->relevant(w))
                    {
                        PredicateDirection *direction = pred_cst.add(0); // from init
                        for (size_t m = 0; m < key_installers.size(); ++m)
                        {
                            uint32_t j = key_installers[m].first;
                            if (j == i)
                            {
                                continue;
                            }
                            Write *w = key_installers[m].second;
                            if (p->match(w) && p->relevant(w))
                            {
                                Edge rw(i, j);
                                direction->insert_determined(i, j); // rw
                                determined_directions_[rw].insert(direction);
                            }
                        }
                        left_candidate = true;
                        break;
                    }
                }
                if (pred_cst.size() == 0)
                { // may read from self update
                    if (v.writes().count(key) != 0 && p->relevant(v.writes().at(key)) && !p->match(v.writes().at(key)))
                        continue;
                    throw SerializableException("predicate aborted reads.");
                }
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
    queue<Edge> edge_queue;
}

bool Vbox::contain_cycle(const unordered_set<Edge> &edges) const
{
    return any_of(edges.begin(), edges.end(),
                  [this](const Edge &e)
                  {
                      return closure_->reach(e.to(), e.from());
                  });
}

void Vbox::prune_item_first(queue<Edge> &edge_queue)
{
    auto item_cst_it = item_csts_.begin();
    while (item_cst_it != item_csts_.end())
    {

        ItemConstraint *item_cst = item_cst_it.base()->get();
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

            for (const Edge &e : accept_edges)
            {
                edge_queue.push(e);
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

void Vbox::prune_pred_first(queue<Edge> &edge_queue)
{

    auto cst_it = pred_csts_.begin();
    while (cst_it != pred_csts_.end())
    {
        PredicateConstraint *cst = cst_it.base()->get();
        for (auto dir_it = cst->directions().begin(); dir_it != cst->directions().end();)
        {
            PredicateDirection *direction = dir_it->second.get();

            // prune undetermined edges
            for (auto e_it = direction->undetermined_edges().begin(); e_it != direction->undetermined_edges().end();)
            {
                const Edge &edge = *e_it;
                Edge &derivation = direction->derivation(edge);

                if (closure_->reach(edge.from(), edge.to()))
                {
                    e_it = direction->undetermined_edges().erase(e_it);

                    direction->remove_derivation(edge);
                    re_derivations_[derivation].erase(edge);
                }
                else if (closure_->reach(edge.to(), edge.from()))
                {
                    e_it = direction->undetermined_edges().erase(e_it);

                    direction->remove_derivation(edge);
                    re_derivations_[derivation].erase(edge);
                }
                else
                {
                    if (closure_->reach(derivation.from(), derivation.to()))
                    {
                        e_it = direction->undetermined_edges().erase(e_it);

                        direction->remove_derivation(edge);
                        re_derivations_[derivation].erase(edge);

                        direction->insert_determined(edge);
                        determined_directions_[edge].insert(direction);

                        for (const Edge &derived_edge : re_derivations_[derivation])
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

                        for (const Edge &derived_edge : re_derivations_[derivation])
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

            // prune determined edges
            for (auto e_it = direction->determined_edges().begin(); e_it != direction->determined_edges().end();)
            {
                const Edge &edge = *e_it;
                if (closure_->reach(edge.from(), edge.to()))
                {
                    e_it = direction->determined_edges().erase(e_it);
                }
                else if (closure_->reach(edge.to(), edge.from()))
                {
                    e_it = direction->determined_edges().erase(e_it);
                    determined_directions_[edge].erase(direction);
                    cst->remove(direction);

                    for (const Edge &e : direction->determined_edges())
                    {
                        determined_directions_[e].erase(direction);
                        if (determined_directions_[e].empty())
                        {
                            determined_directions_.erase(e);
                        }
                    }

                    for (PredicateDirection *rej : determined_directions_.at(edge)) // 包含该边的其他方向
                    {
                        for (const Edge &e : rej->determined_edges())
                        {
                            determined_directions_[e].erase(rej);
                            if (determined_directions_[e].empty())
                            {
                                determined_directions_.erase(e);
                            }
                        }
                        rej->parent()->remove(rej);
                    }
                }
                else
                {
                    ++e_it;
                }
            }
        }

        if (cst->size() == 0) // 剪枝后
        {
            throw SerializableException("prune predicate constraints error.");
        }
        else if (cst->size() == 1)
        {
            PredicateDirection *direction = cst->directions().begin()->second.get();
            for (const Edge &e : direction->determined_edges())
            {
                edge_queue.push(e);
                determined_directions_[e].erase(direction);
                if (determined_directions_[e].empty())
                {
                    determined_directions_.erase(e);
                }
            }
            for (const Edge &e : direction->undetermined_edges())
            {
                Edge derivation = cst->directions().begin()->second->derivation(e);
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
                    for (const Edge &e_ : item_directions_[e]->edges())
                    {
                        item_directions_[e_] = item_directions_[derivation];
                        item_directions_[derivation]->insert(e_.from(), e_.to());
                    }
                    for (const Edge &e_ : item_directions_[e]->adversary()->edges())
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
    queue<Edge> edge_queue;
    prune_item_first(edge_queue);
    prune_pred_first(edge_queue);
    while (!edge_queue.empty())
    {
        Edge e = edge_queue.front();
        edge_queue.pop();
        if (closure_->reach(e.to(), e.from()))
        {
            throw SerializableException("prune");
        }
        vector<Edge> change = closure_->insert(e);

        for (const Edge &accept : change)
        {
            Edge reject = Edge(accept.to(), accept.from());
            // prune item constraint
            auto item_dir_it = item_directions_.find(reject);
            if (item_dir_it != item_directions_.end())
            {
                for (const Edge &acc : item_dir_it->second->adversary()->edges())
                {
                    edge_queue.push(acc);
                    item_directions_.erase(item_dir_it);
                }
                for (const Edge &rej : item_dir_it->second->edges())
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
                    for (const Edge &e : rej->determined_edges())
                    {
                        determined_directions_[e].erase(rej);
                        if (determined_directions_[e].empty())
                        {
                            determined_directions_.erase(e);
                        }
                    }
                    rej->parent()->remove(rej);
                    if (rej->parent()->size() == 0)
                    {
                        throw SerializableException("prune predicate constraints error.");
                    }
                    if (rej->parent()->size() == 1)
                    {
                        PredicateDirection *direction = rej->parent()->directions().begin()->second.get();
                        for (const Edge &e : direction->determined_edges())
                        {
                            edge_queue.push(e);
                            determined_directions_[e].erase(direction);
                            if (determined_directions_[e].empty())
                            {
                                determined_directions_.erase(e);
                            }
                        }
                        for (const Edge &e : direction->undetermined_edges())
                        {
                            Edge derivation = rej->parent()->directions().begin()->second->derivation(e);
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
                                for (const Edge &e_ : item_directions_[e]->edges())
                                {
                                    item_directions_[e_] = item_directions_[derivation];
                                    item_directions_[derivation]->insert(e_.from(), e_.to());
                                }
                                for (const Edge &e_ : item_directions_[e]->adversary()->edges())
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
                for (PredicateDirection *rej : pred_udir_it_r->second)
                {
                    rej->remove_undetermined(reject);
                }
            }
            auto pred_udir_it_a = undetermined_directions_.find(accept);
            if (pred_udir_it_a != undetermined_directions_.end())
            {
                for (PredicateDirection *d : pred_udir_it_a->second)
                {
                    d->remove_undetermined(accept);
                    d->insert_determined(accept);
                    determined_directions_[accept].insert(d);
                }
                undetermined_directions_.erase(accept);
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
                Vertex from = vertices_[tid2index_[read->from_tid()]];
                from.set_read(read->key(), i);
                edges_.emplace_back(from.index(), i); // wr
            }
            break;
            case OperatorType::WRITE:
            {
                Write *write = static_cast<Write *>(op.get());
                installs_[write->key()].push_back(i);
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
    for (size_t i = 0; i < n_; ++i)
    {
        Vertex &v = vertices_[i];
        auto it = active_vertices.begin();
        while (it != active_vertices.end())
        {
            Vertex &u = vertices_[*it];
            if (u.end() <= v.start())
            {
                it = active_vertices.erase(it);
                u.set_right(i);
            }
            else
            {
                ++it;
                v.set_left(u.index());
            }
        }
        active_vertices.push_back(i);
    }
    for (uint32_t i : active_vertices)
    {
        vertices_[i].set_right(vertices_.size());
    }
}
