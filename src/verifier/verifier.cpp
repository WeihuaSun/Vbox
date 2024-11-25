// #include <cassert>
// #include <chrono>
// #include <list>

// #include "verifier.h"
// #include "transaction.h"
// #include "transitive_closure.h"
// #include "solver/constraint.h"
// using namespace std;

// Verifier::Verifier() {};

// void Verifier::mergeConstraints(ItemConstraint &cst)
// {
//     unordered_set<Direction *> equal_alpha, equal_beta;
//     for (const auto &e : cst.alpha_edges())
//     {
//         auto it = direction_.find(e);
//         if (it != direction_.end())
//         {
//             Direction *d = it->second;
//             equal_alpha.insert(d);
//         }
//     }

//     for (const auto &e : cst.beta_edges())
//     {
//         auto it = direction_.find(e);
//         if (it != direction_.end())
//         {
//             Direction *d = it->second;
//             equal_beta.insert(d);
//             if (equal_alpha.find(d) != equal_alpha.end())
//             {
//                 // exception
//             }
//         }
//     }

//     if (!equal_alpha.empty() || !equal_beta.empty())
//     {
//         for (Direction *direction : equal_alpha)
//         {
//             cst.insert_alpha(direction->edges);
//             cst.insert_beta(direction->adversary->edges);
//             item_cst_.erase(direction->parent);
//         }

//         for (Direction *direction : equal_beta)
//         {
//             cst.insert_alpha(direction->adversary->edges);
//             cst.insert_beta(direction->edges);
//             item_cst_.erase(direction->parent);
//         }
//     }
// }

// void Verifier::generate_item_constraint()
// {
//     vector<uint32_t> active_vertices;
//     unordered_map<uint32_t, uint64_t> replacement_time;
//     for (auto &entry : install)
//     {
//         uint64_t key = entry.first;
//         vector<uint32_t> key_installers = entry.second;
//         replacement_time.clear();
//         active_vertices.clear();

//         for (uint32_t i : key_installers)
//         {
//             Vertex v = vertices_[i];
//             auto active_it = active_vertices.begin();
//             while (active_it != active_vertices.end())
//             {
//                 Vertex u = vertices_[*active_it];
//                 if (replacement_time[u.index()] <= v.start())
//                 {
//                     active_it = active_vertices.erase(active_it);
//                 }
//                 else if (u.end() <= v.start()) // item-write-dependency (u -> v)
//                 {
//                     ++active_it;
//                     if (u.reads().count(key) > 0)
//                     {
//                         const unordered_set<uint32_t> &read_from_u = u.reads().at(key);
//                         for (uint32_t j : read_from_u)
//                         {
//                             Vertex r = vertices_[j]; // item-read-depends on u with respect to key ( u -> r)
//                             if (i != j && i <= r.right())
//                             {
//                                 edges_.emplace_back(r, v); // item-anti-dependency ( r -> v )
//                             }
//                         }
//                     }
//                     replacement_time[u.index()] = min(replacement_time[u.index()], v.end());
//                 }
//                 else // overlap in time
//                 {
//                     ++active_it;
//                     ItemConstraint item_cst(u.index(), v.index());

//                     //(u -> v)
//                     if (u.reads().count(key) > 0)
//                     {
//                         const unordered_set<uint32_t> &read_from_u = u.reads().at(key);
//                         for (uint32_t j : read_from_u)
//                         {
//                             Vertex r = vertices_[j];
//                             if (i != j && i <= r.right())
//                             {
//                                 edges_.emplace_back(r, v);
//                             }
//                         }
//                     }
//                     //(v -> u)
//                     if (v.reads().count(key) > 0)
//                     {
//                         const unordered_set<uint32_t> &read_from_v = v.reads().at(key);
//                         for (uint32_t j : read_from_v)
//                         {
//                             Vertex r = vertices_[j];
//                             if (i != j && i <= r.right())
//                             {
//                                 edges_.emplace_back(r, u);
//                             }
//                         }
//                     }
//                 }
//                 replacement_time[i] = UINT64_MAX;
//                 active_vertices.push_back(i);
//             }
//         }
//     }
// }

// void Verifier::generate_predicate_constraint()
// {
//     for (size_t i = 0; i < n_; ++i)
//     {
//         Vertex &v = vertices_[i];
//         if (v.predicates().empty())
//         {
//             continue;
//         }
//         for (Predicate *p : v.predicates())
//         {
//             uint64_t key;
//             vector<uint32_t> key_installers;
//             PredicateConstraint cst(i);
//             for (size_t m = 0; m < key_installers.size(); ++m)
//             {
//                 uint32_t j = key_installers[m];
//                 Vertex &u = vertices_[j];
//                 if(!p->match())

//             }
//         }
//     }

//     for (const auto &predicateEntry : predMap)
//     {
//         Vertex readTxn = predicateEntry.first;
//         vector<Predicate *> predicates = predicateEntry.second;
//         for (size_t i = readTxn->leftIndex; i < readTxn->rightIndex; ++i)
//         {
//             Vertex curTxn = transactionHistory[i];
//             Install &install = installMap[curTxn->tid];
//             for (const auto &installEntry : install)
//             {
//                 writeOpMap[installEntry.first].push_back(curTxn);
//             }
//         }

//         for (Predicate *predicate : predicates)
//         {
//             for (const auto &entry : writeOpMap)
//             {
//                 uint64_t key = entry.first;
//                 vector<Vertex> installList = entry.second;

//                 allPredConstraints.emplace_back(readTxn);
//                 PredicateConstraint &predCst = allPredConstraints.back();

//                 for (size_t i = 0; i < installList.size(); ++i)
//                 {
//                     Vertex curTxn = installList[i];
//                     if (curTxn != readTxn)
//                     {
//                         Write *curWrite = installMap[curTxn->tid][key];
//                         if (!predicate->match(curWrite) && predicate->relevant(curWrite))
//                         {
//                             PredicateDirection *direction = predCst.add(curTxn);
//                             TEdge wr(curTxn, readTxn);
//                             direction->insertDEdge(wr); // wr
//                             dDMap[wr].insert(direction);

//                             // forward
//                             for (size_t j = i + 1; j < installList.size(); ++j)
//                             {
//                                 Vertex otherTxn = installList[j];
//                                 Write *otherWrite = installMap[otherTxn->tid][key];
//                                 if (predicate->match(otherWrite) && predicate->relevant(otherWrite))
//                                 {
//                                     TEdge rw(readTxn, otherTxn);
//                                     if (otherTxn->timeIndex >= curTxn->rightIndex)
//                                     {
//                                         direction->insertDEdge(rw); // rw
//                                         dDMap[rw].insert(direction);
//                                     }
//                                     else
//                                     {
//                                         TEdge ww(curTxn, otherTxn);                        // ww
//                                         direction->insertUEdge(curTxn, readTxn, otherTxn); // rw
//                                         dUMap[ww].insert(direction);
//                                     }
//                                 }
//                             }
//                             // backward
//                             for (int j = i - 1; j >= 0; --j)
//                             {
//                                 Vertex otherTxn = installList[j];
//                                 if (otherTxn->timeIndex < curTxn->leftIndex)
//                                 {
//                                     break;
//                                 }
//                                 else
//                                 {
//                                     Write *otherWrite = installMap[otherTxn->tid][key];
//                                     if (predicate->match(otherWrite) && predicate->relevant(otherWrite))
//                                     {
//                                         TEdge ww(curTxn, otherTxn);                        // ww
//                                         direction->insertUEdge(curTxn, readTxn, otherTxn); // rw
//                                         dUMap[ww].insert(direction);
//                                     }
//                                 }
//                             }
//                         }
//                     }
//                 }

//                 if (predCst.size == 0)
//                 {
//                     for (size_t i = 0; i < installList.size(); ++i)
//                     {
//                         Vertex curTxn = installList[i];
//                         Write *curWrite = installMap[curTxn->tid][key];
//                         if (predicate->match(curWrite) && predicate->relevant(curWrite))
//                         {
//                             knownDependencies[readTxn].insert(curTxn); // rw
//                         }
//                     }
//                 }
//                 else
//                 {
//                     activePredConstraints.insert(&predCst);
//                 }
//                 // else
//                 // {   // need?往前找，看看有没有mismatch的版本,需要把插入事务也包括进来
//                 //     PredicateDirection *direction = predCst.add(initTxn);
//                 //     activePredConstraints.insert(&predCst);
//                 //     for (size_t i = 0; i < installList.size(); ++i)
//                 //     {
//                 //         Vertex curTxn = installList[i];
//                 //         Write *curWrite = installMap[curTxn->tid][key];
//                 //         if (predicate->match(curWrite) && predicate->relevant(curWrite))
//                 //         {
//                 //             TEdge rw(readTxn, curTxn);
//                 //             direction->insertDEdge(rw); // rw
//                 //             dDMap[rw].insert(direction);
//                 //         }
//                 //     }
//                 // }
//             }
//         }
//     }
// }

// void Verifier::buildDependencyGraph()
// {
//     for (size_t i = 0; i < n_; ++i)
//     {
//         Vertex &v = vertices_[i];
//         Transaction *trx = v.transaction();
//         for (const unique_ptr<Operator> &op : trx->operators())
//         {
//             switch (op->type())
//             {
//             case OperatorType::READ:
//             {
//                 Read *read = static_cast<Read *>(op.get());
//                 Vertex from = vertices_[tid2index_[read->from_tid()]];
//                 from.set_read(read->key(), i);
//                 edges_.emplace_back(from.index(), v);
//             }
//             break;
//             case OperatorType::WRITE:
//             {
//                 Write *write = static_cast<Write *>(op.get());
//                 v.set_write(write->key(), write);
//             }
//             break;
//             case OperatorType::PREDICATE:
//             {
//                 Predicate *predicate = static_cast<Predicate *>(op.get());
//                 v.set_predicate(predicate);
//             }
//             break;
//             default:
//                 break;
//             }
//         }
//     }

//     vector<Vertex> active_vertices;
//     for (size_t i = 0; i < n_; ++i)
//     {
//         Vertex &v = vertices_[i];
//         auto it = active_vertices.begin();
//         while (it != active_vertices.end())
//         {
//             Vertex &u = *it;
//             if (u.end() <= v.start())
//             {
//                 it = active_vertices.erase(it);
//                 u.set_right(i);
//             }
//             else
//             {
//                 ++it;
//                 v.set_left(u.index());
//             }
//         }
//         active_vertices.push_back(v);
//     }
//     for (Vertex &v : active_vertices)
//     {
//         v.set_right(vertices_.size());
//     }
// }

// bool Verifier::run()
// {
//     try
//     {
//         buildDependencyGraph();
//         check_reads();
//         identifyKnownDependencies();
//         generateConstraints();
//         generatePredicateConstraints();
//         initializeTransitiveClosure();
//         pruning();
//         solveConstraints();
//         return true;
//     }
//     catch (exception &e)
//     {
//         cout << e.what() << endl;
//         return false;
//     }
// }

// bool OfflineAnalyzer::solveConstraints()
// {
//     if (options.satStrategy == "monosat")
//     {
//         MonoSolver mSolver = MonoSolver();
//         return mSolver.monoSolve(transactionHistory, knownDependencies, activeConstraints);
//     }
//     else if (options.satStrategy == "minisat")
//     {
//         MiSATSolver miSolver = MiSATSolver();
//         return miSolver.miSolve(transactionHistory, knownDependencies, activeConstraints);
//     }
//     else if (options.satStrategy == "graphsat")
//     {
//         GraphSolver gSolver(activeConstraints, activePredConstraints, transitiveClosure, directionMap, dDMap, dUMap);
//         return gSolver.graphSolve();
//     }
//     return true;
// }

// void OfflineAnalyzer::pruning()
// {
//     if (options.pruningStrategy == "no")
//     {
//         // Do nothing
//     }
//     else if (options.pruningStrategy == "pruneOptimize")
//     {
//         optimizePruning();
//     }
//     else if (options.pruningStrategy == "prune")
//     {
//         pruneConstraints();
//     }
// }

// void OfflineAnalyzer::executePruningStep(unordered_map<Vertex, unordered_set<Vertex>> &prunedEdges)
// {
//     prunedEdges.clear();
//     auto cstIt = activeConstraints.begin();
//     unordered_set<TEdge, EdgeHash, EdgeEqual> accept;
//     unordered_set<TEdge, EdgeHash, EdgeEqual> reject;

//     while (cstIt != activeConstraints.end())
//     {
//         Constraint *c = *cstIt;
//         bool cycleAlpha = false;
//         bool cycleBeta = false;

//         accept.clear();
//         reject.clear();

//         // Check for cycles in alpha edges
//         for (const auto &e : c->alpha->edges)
//         {
//             if (transitiveClosure.isReach(e.second, e.first))
//             {
//                 pqtimes++;
//                 cycleAlpha = true;
//                 break; // Early exit if cycle detected
//             }
//         }

//         // Check for cycles in beta edges
//         for (const auto &e : c->beta->edges)
//         {
//             if (transitiveClosure.isReach(e.second, e.first))
//             {
//                 pqtimes++;
//                 cycleBeta = true;
//                 break; // Early exit if cycle detected
//             }
//         }

//         if (cycleAlpha && cycleBeta)
//         {
//             throw SerializableException("prune: both alpha and beta contain cycles"); // Improved exception message
//         }
//         else if (cycleAlpha)
//         {
//             accept = move(c->beta->edges);
//             reject = move(c->alpha->edges);
//             cstIt = activeConstraints.erase(cstIt);
//         }
//         else if (cycleBeta)
//         {
//             accept = move(c->alpha->edges);
//             reject = move(c->beta->edges);
//             cstIt = activeConstraints.erase(cstIt);
//         }
//         else
//         {
//             ++cstIt;
//             continue;
//         }

//         // Move accepted edges to prunedEdges and erase from directionMap
//         for (const auto &e : accept)
//         {
//             prunedEdges[e.first].insert(e.second);
//             if (collectEdges)
//             {
//                 knownDependencies[e.first].insert(e.second);
//             }
//             directionMap.erase(e);
//         }

//         // Only erase rejected edges from directionMap
//         for (const auto &e : reject)
//         {
//             directionMap.erase(e);
//         }
//     }
// }

// void OfflineAnalyzer::pruneConstraints()
// {
//     unordered_map<Vertex, unordered_set<Vertex>> prunedEdges;
//     do
//     {
//         executePruningStep(prunedEdges);
//         // cout << prunedEdges.size() << endl;
//         // transitiveClosure.construct(prunedEdges, "warshall");
//         // transitiveClosure.construct(prunedEdges, options.tcUpdateStrategy);

//     } while (!prunedEdges.empty());
// }

// void OfflineAnalyzer::pruneNormalConstrantsFirst(queue<TEdge> &edgeQueue)
// {
//     auto cstIt = activeConstraints.begin();
//     while (cstIt != activeConstraints.end())
//     {
//         Constraint *c = *cstIt;
//         bool cycleAlpha = false;
//         bool cycleBeta = false;

//         unordered_set<TEdge, EdgeHash, EdgeEqual> &alphaEdges = c->alpha->edges;
//         unordered_set<TEdge, EdgeHash, EdgeEqual> &betaEdges = c->beta->edges;
//         unordered_set<TEdge, EdgeHash, EdgeEqual> *accept = nullptr;
//         unordered_set<TEdge, EdgeHash, EdgeEqual> *reject = nullptr;

//         for (const auto &e : alphaEdges)
//         {
//             if (transitiveClosure.isReach(e.second, e.first))
//             {
//                 cycleAlpha = true;
//                 break;
//             }
//         }

//         for (const auto &e : betaEdges)
//         {
//             if (transitiveClosure.isReach(e.second, e.first))
//             {
//                 cycleBeta = true;
//                 break;
//             }
//         }

//         if (cycleAlpha && cycleBeta)
//         {
//             throw SerializableException("prune: both alpha and beta contain cycles");
//         }
//         else if (cycleAlpha)
//         {
//             accept = &betaEdges;
//             reject = &alphaEdges;
//             cstIt = activeConstraints.erase(cstIt);
//         }
//         else if (cycleBeta)
//         {
//             accept = &alphaEdges;
//             reject = &betaEdges;
//             cstIt = activeConstraints.erase(cstIt);
//         }
//         else
//         {
//             ++cstIt;
//             continue;
//         }

//         for (const auto &e : *accept)
//         {
//             edgeQueue.push(e);
//             if (collectEdges)
//             {
//                 knownDependencies[e.first].insert(e.second);
//             }
//             directionMap.erase(e);
//         }

//         for (const auto &e : *reject)
//         {
//             directionMap.erase(e);
//         }
//     }
// }

// void OfflineAnalyzer::prunePredicateConstrantsFirst(std::queue<TEdge> &edgeQueue)
// {
//     auto pcstIt = activePredConstraints.begin();
//     while (pcstIt != activePredConstraints.end())
//     {
//         PredicateConstraint *pc = *pcstIt;
//         vector<TEdge> rejectEdges;
//         vector<TEdge> acceptEdges;
//         for (auto it = pc->directions.begin(); it != pc->directions.end();)
//         {
//             PredicateDirection *direction = it->second.get();
//             rejectEdges.clear();
//             acceptEdges.clear();
//             for (const auto &e : direction->dEdges)
//             {
//                 if (transitiveClosure.isReach(e.second, e.first))
//                 {
//                     rejectEdges.push_back(e);
//                     break;
//                 }
//             }
//             if (!rejectEdges.empty())
//             {
//                 TEdge rejectEdge = rejectEdges.back();
//                 rejectEdges.pop_back();
//                 for (const auto &e : direction->dEdges)
//                 {
//                     unordered_set<PredicateDirection *> &pds = dDMap[e];
//                     if (e == rejectEdge)
//                     {
//                         for (PredicateDirection *rejectPD : pds)
//                         {
//                             rejectPD->parent->directions.erase(rejectPD->n);
//                         }
//                         dDMap.erase(e);
//                     }
//                     else
//                     {
//                         pds.erase(direction);
//                     }
//                 }
//                 it = pc->directions.erase(it);
//             }
//             else
//             {
//                 for (auto edgeIt = direction->uEdges.begin(); edgeIt != direction->uEdges.end();)
//                 {
//                     auto &e = *edgeIt;
//                     if (transitiveClosure.isReach(e.second, e.first))
//                     {
//                         rejectEdges.push_back(e);
//                         edgeIt = direction->uEdges.erase(edgeIt);
//                     }
//                     else if (transitiveClosure.isReach(e.first, e.second))
//                     {
//                         direction->dEdges.emplace(e.first, e.second);
//                         dDMap[e].insert(direction);
//                         edgeIt = direction->uEdges.erase(edgeIt);
//                     }
//                     else
//                     {
//                         ++edgeIt;
//                     }
//                 }
//                 for (const auto &e : rejectEdges)
//                 {
//                     unordered_set<PredicateDirection *> &pds = dUMap[e];
//                     for (PredicateDirection *upd : pds)
//                     {
//                         upd->uEdges.erase(e);
//                     }
//                 }
//                 for (const auto &e : acceptEdges)
//                 {
//                     unordered_set<PredicateDirection *> &pds = dUMap[e];
//                     for (PredicateDirection *upd : pds)
//                     {
//                         upd->uEdges.erase(e);
//                         upd->insertDEdge(e.first, e.second);
//                     }
//                 }

//                 ++it;
//             }
//         }
//         if (pc->size == 0)
//         {
//             throw SerializableException("prune predicate constraints error.");
//         }
//         else if (pc->size == 1)
//         {
//             PredicateDirection *onlyDirection = pc->directions.begin()->second.get();
//             for (const auto &e : onlyDirection->dEdges)
//             {
//                 edgeQueue.push(e);
//             }
//             for (const auto &e : onlyDirection->uEdges)
//             {
//                 TEdge &core = onlyDirection->uEdgeCore[e];
//                 if (directionMap.find(e) == directionMap.end())
//                 {
//                     directionMap[e] = directionMap[core];
//                     directionMap[core]->insertEdge(e.first, e.second);
//                 }
//                 else
//                 {
//                     // merge e->core
//                     for (auto const &e : directionMap[e]->edges)
//                     {
//                         directionMap[e] = directionMap[core];
//                         directionMap[core]->insertEdge(e.first, e.second);
//                     }
//                     for (auto const &e : directionMap[e]->adversary->edges)
//                     {
//                         directionMap[e] = directionMap[core]->adversary;
//                         directionMap[core]->adversary->insertEdge(e.first, e.second);
//                     }
//                 }
//             }
//             pcstIt = activePredConstraints.erase(pcstIt);
//         }
//         else
//         {
//             ++pcstIt;
//         }
//     }
// }

// void OfflineAnalyzer::optimizePruning()
// {
//     queue<TEdge> edgeQueue;
//     pruneNormalConstrantsFirst(edgeQueue);
//     prunePredicateConstrantsFirst(edgeQueue);
//     while (!edgeQueue.empty())
//     {
//         TEdge e = edgeQueue.front();
//         edgeQueue.pop();
//         Vertex u = e.first;
//         Vertex v = e.second;
//         if (transitiveClosure.isReach(v, u))
//         {
//             throw SerializableException("prune: both alpha and beta contain cycles");
//         }
//         auto changes = transitiveClosure.insertEdge(&e);
//         for (const TEdge &changeEdge : changes)
//         {
//             TEdge rejectEdge(changeEdge.second, changeEdge.first);
//             auto dIt = directionMap.find(rejectEdge);
//             if (dIt != directionMap.end())
//             {
//                 activeConstraints.erase(dIt->second->parent);
//                 for (const TEdge &accEdge : dIt->second->adversary->edges)
//                 {
//                     edgeQueue.push(accEdge);
//                     if (collectEdges)
//                     {
//                         knownDependencies[accEdge.first].insert(accEdge.second);
//                     }
//                     directionMap.erase(accEdge);
//                 }
//                 for (const TEdge &rejEdge : dIt->second->edges)
//                 {
//                     directionMap.erase(rejEdge);
//                 }
//             }
//             auto dDIt = dDMap.find(rejectEdge);
//             if (dDIt != dDMap.end())
//             {
//                 unordered_set<PredicateDirection *> rejectDirections = dDIt->second;
//                 for (PredicateDirection *rejectDirection : rejectDirections)
//                 {
//                     for (const auto &e : rejectDirection->dEdges)
//                     {
//                         dDMap[e].erase(rejectDirection);
//                         // if (dDMap[e].empty())
//                         // {
//                         //     dDMap.erase(e);
//                         // }
//                     }
//                     rejectDirection->parent->directions.erase(rejectDirection->n);
//                 }
//                 dDMap.erase(dDIt);
//             }
//             auto dUIt1 = dUMap.find(rejectEdge);
//             if (dUIt1 != dUMap.end())
//             {
//                 unordered_set<PredicateDirection *> rejectDirections = dUIt1->second;
//                 for (PredicateDirection *rejectDirection : rejectDirections)
//                 {
//                     rejectDirection->uEdges.erase(rejectEdge);
//                     rejectDirection->uEdgeCore.erase(rejectEdge);
//                 }
//                 dUMap.erase(dUIt1);
//             }
//             auto dUIt2 = dUMap.find(changeEdge);
//             if (dUIt2 != dUMap.end())
//             {
//                 unordered_set<PredicateDirection *> acceptDirections = dUIt2->second;
//                 for (PredicateDirection *acceptDirection : acceptDirections)
//                 {
//                     acceptDirection->uEdges.erase(changeEdge);
//                     acceptDirection->uEdgeCore.erase(changeEdge);
//                     acceptDirection->dEdges.insert(changeEdge);
//                     dDMap[changeEdge].insert(acceptDirection);
//                 }
//                 dUMap.erase(dUIt2);
//             }
//         }
//     }
// }

// void OfflineAnalyzer::initializeTransitiveClosure()
// {
//     transitiveClosure.constructStage();
//     transitiveClosure.construct(knownDependencies);
// }



// void OfflineAnalyzer::identifyKnownDependencies() // wr
// {
//     for (const auto &entry1 : readOperations)
//     {
//         auto txn = transactionMap[entry1.first].get();
//         for (const auto &entry2 : entry1.second)
//         {
//             for (const auto &entry3 : entry2.second)
//             {
//                 knownDependencies[txn].insert(entry3);
//             }
//         }
//     }
// }

// void Verifier::check_reads(const vector<Read *> &reads)
// {
//     for (Read *read : reads)
//     {
//         if (read->from_tid() != 0)
//         {
//             if (tid2index_.count(read->from_tid() == 0))
//             {
//                 // abort reads
//             }
//             if (vertices_[tid2index_[read->from_tid()]].writes().at(read->key())->oid() != read->from_oid())
//             {
//                 // intermediate reads
//             }
//         }
//     }
// }

// void OfflineAnalyzer::mergeConstraints(Constraint &c)
// {
//     unordered_set<Direction *> equal_alpha, equal_beta;

//     // Check directionMap for intersections with alpha and beta edges
//     for (const auto &e : c.alpha->edges)
//     {
//         auto it = directionMap.find(e);
//         if (it != directionMap.end())
//         {
//             Direction *direction = it->second;
//             if (equal_beta.find(direction) != equal_beta.end())
//             {
//                 throw SerializableException("Conflict detected between alpha and beta");
//             }
//             equal_alpha.insert(direction);
//         }
//     }

//     for (const auto &e : c.beta->edges)
//     {
//         auto it = directionMap.find(e);
//         if (it != directionMap.end())
//         {
//             Direction *direction = it->second;
//             if (equal_alpha.find(direction) != equal_alpha.end())
//             {
//                 throw SerializableException("Conflict detected between alpha and beta");
//             }
//             equal_beta.insert(direction);
//         }
//     }

//     if (!equal_alpha.empty() || !equal_beta.empty())
//     {
//         // Merge directions from equal_alpha
//         for (Direction *direction : equal_alpha)
//         {
//             c.alpha->edges.insert(direction->edges.begin(), direction->edges.end());
//             c.beta->edges.insert(direction->adversary->edges.begin(), direction->adversary->edges.end());
//             activeConstraints.erase(direction->parent);
//         }

//         // Merge directions from equal_beta
//         for (Direction *direction : equal_beta)
//         {
//             c.beta->edges.insert(direction->edges.begin(), direction->edges.end());
//             c.alpha->edges.insert(direction->adversary->edges.begin(), direction->adversary->edges.end());
//             activeConstraints.erase(direction->parent);
//         }
//     }
// }
