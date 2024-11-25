#ifndef OFFLINE_ANALYSIS_H
#define OFFLINE_ANALYSIS_H

#include <unordered_set>
#include <vector>
#include <list>
#include <string>
#include <queue>
#include "transaction/transaction.h"
#include "graph/graph.h"
// #include "transitive_closure.h"

using InstallMap = std::unordered_map<uint64_t, std::vector<uint32_t>>;

class Vbox
{
    Vbox(const std::string &root);

public:
private:
    TransactionManager trx_manager_;
    std::vector<Vertex> vertices_;
    std::vector<Edge> edges_;
    std::unordered_map<uint32_t, uint32_t> tid2index_;

    size_t n_;
};

// class Verifier
// {
// public:
//     Verifier();
//     bool run();

// private:
//     bool solveConstraints();
//     void pruneConstraints();
//     void buildDependencyGraph();
//     void check_reads(const vector<Read *> &reads);
//     void initializeTransitiveClosure();
//     void generate_item_constraint();
//     void generate_predicate_constraint();
//     void identifyKnownDependencies();
//     void generatePredicateConstraints();

//     void mergeConstraints(Constraint &constraint);
//     void executePruningStep(std::unordered_map<Vertex, std::unordered_set<Vertex>> &prunedEdges);
//     void optimizePruning();
//     void pruning();

//     void pruneNormalConstrantsFirst(std::queue<TEdge> &edgeQueue);
//     void prunePredicateConstrantsFirst(std::queue<TEdge> &edgeQueue);

//     // Data structures
//     // std::vector<Vertex> transactionHistory;
//     // TxnMap &transactionMap;
//     // TransitiveClosure transitiveClosure;
//     // VerifyOptions &options;
//     // std::unordered_set<Constraint *> activeConstraints;
//     // std::unordered_set<PredicateConstraint *> activePredConstraints;
//     // std::list<Constraint> allConstraints;
//     // std::list<PredicateConstraint> allPredConstraints;
//     // InstallMap installMap;
//     // PredicateMap predMap;
//     // WriteMap writeOperations;
//     // ReadMap readOperations;
//     // std::vector<Read *> readOperationsList;
//     // DirectionMap directionMap;
//     // PredDirectionMap dDMap;
//     // PredDirectionMap dUMap;

//     // std::unordered_map<Vertex, std::unordered_set<Vertex>> knownDependencies;
//     // int totalConstraints = 0;
//     // int pqtimes = 0;
//     // bool collectEdges;
//     // int numVertices;
// private:
//     std::vector<Vertex> vertices_;
//     std::vector<Edge> edges_;
//     std::unordered_map<uint32_t, uint32_t> tid2index_;
//     InstallMap install;
//     std::unordered_set<ItemConstraint> item_cst_;
//     DirectionMap direction_;
//     size_t n_;
// };

#endif // OFFLINE_ANALYSIS_H
