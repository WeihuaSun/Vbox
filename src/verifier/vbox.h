#ifndef VBOX_H
#define VBOX_H

#include "transaction/transaction.h"
#include "graph/graph.h"
#include "graph/transitive_closure.h"
#include "options.h"
#include "solver/constraint.h"

#include <vector>
#include <unordered_map>
#include <queue>
#include <set>

class Vbox
{

public:
    Vbox(const VerifyOptions &options);
    bool run();

private:
    void init();
    void check_read(std::vector<Read *> &reads);
    void generate_item_constraint();
    void merge_item_constraint(ItemConstraint &cst);
    void generate_pred_constraint();
    void construct_closure();

    void prune_constraint();
    void prune();
    void prune_opt();
    void prune_item_first(std::queue<DSG::Edge> &edges);
    void prune_pred_first(std::queue<DSG::Edge> &edges);
    bool contain_cycle(const std::unordered_set<DSG::Edge> &edges) const;
    void solve_constraint();

private:
    TransactionManager trx_manager_;
    std::vector<Vertex> vertices_;
    std::unordered_set<DSG::Edge> edges_;
    std::unordered_map<uint32_t, uint32_t> tid2index_;
    std::unordered_map<uint64_t, std::set<uint32_t>> installs_;
    std::vector<std::unique_ptr<ItemConstraint>> item_csts_;
    std::vector<std::unique_ptr<PredicateConstraint>> pred_csts_;

    std::unordered_map<DSG::Edge, ItemDirection *> item_directions_;
    std::unordered_map<DSG::Edge, std::unordered_set<PredicateDirection *>> determined_directions_;
    std::unordered_map<DSG::Edge, std::unordered_set<PredicateDirection *>> undetermined_directions_;
    std::unordered_map<DSG::Edge, std::unordered_set<DSG::Edge>> re_derivations_;

    TransitiveClosure* closure_;
    VerifyOptions options_;
    size_t n_;
    size_t total_item_cst_num_ = 0;
};

#endif