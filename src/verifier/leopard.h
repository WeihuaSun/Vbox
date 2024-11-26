#ifndef LEOPARD_H
#define LEOPARD_H

#include <set>
#include <list>
#include "transaction/transaction.h"
#include "graph/graph.h"
#include "options.h"

struct ReadComparator
{
    bool operator()(const Read *lhs, const Read *rhs) const
    {
        return lhs->start() < rhs->start();
    }
};

class ReadManager
{
public:
    ReadManager() = default;
    size_t size() const;
    void insert(const Read *read);
    void remove(const Read *read);
    const Read *min() const;

private:
    std::set<const Read *, ReadComparator> reads_;
};

class Leopard
{

public:
    Leopard(const VerifyOptions &options);
    bool run();

private:
    void consistent_read(uint32_t i);
    std::vector<uint32_t> candidate(Read *read, Vertex &r_trx_);
    void sort_write(uint32_t i);
    void first_updater_win(uint32_t i);
    void ssi_certifier(const std::vector<DSG::Edge> &edges);
    void to_certifier(const std::vector<DSG::Edge> &edges);
    void garbage_collection();

private:
    TransactionManager trx_manager_;
    std::vector<Vertex> vertices_;
    size_t n_;
    std::unordered_map<uint32_t, std::vector<Read *>> reads_;
    std::unordered_map<uint32_t, uint32_t> anti_in_;
    std::unordered_map<uint32_t, uint32_t> anti_out_;
    std::unordered_map<uint64_t, std::vector<uint32_t>> active_install_;
    std::vector<DSG::Edge> ww_edges_;
    std::vector<DSG::Edge> wr_edges_;
    std::vector<DSG::Edge> rw_edges_;
    std::unordered_map<uint64_t, ReadManager> min_reads_;
    std::unordered_map<uint64_t, std::list<uint32_t>> version_order_;
    std::unordered_map<uint64_t, std::unordered_map<uint32_t, uint64_t>> replacement_time_;
    std::unordered_map<uint64_t, std::unordered_set<uint32_t>> garbage_;
};

#endif