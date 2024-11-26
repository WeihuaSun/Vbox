#include <vector>
#include <algorithm>
#include <stdexcept>
#include "graph.h"
#include "options.h"

#include <vector>
#include <unordered_map>
#include <stdexcept>

#define MAX_VERTICES 100000

enum State
{
    UNVISITED = 0,
    VISITING = 1,
    VISITED = 2
};

class ReachabilityMatrix
{
public:
    virtual ~ReachabilityMatrix() = default;
    virtual void set_reach(uint32_t from, uint32_t to, bool is_reachable) = 0;
    virtual bool reach(uint32_t from, uint32_t to) const = 0;
    virtual const DSG::Edge *parent(uint32_t from, uint32_t to) const = 0;
    virtual void set_parent(uint32_t from, uint32_t to, const DSG::Edge *parent) = 0;
    virtual size_t size() const = 0;
    virtual size_t capacity() const = 0;
};

class StandardMatrix : public ReachabilityMatrix
{
public:
    StandardMatrix(size_t n);
    bool reach(uint32_t from, uint32_t to) const override;
    void set_reach(uint32_t from, uint32_t to, bool is_reachable) override;
    const DSG::Edge *parent(uint32_t from, uint32_t to) const override;
    void set_parent(uint32_t from, uint32_t to, const DSG::Edge *parent) override;
    size_t size() const override;
    size_t capacity() const override;

private:
    size_t n_;
    std::vector<std::vector<bool>> reach_;
    std::vector<std::vector<const DSG::Edge *>> parent_;
};

class HashMatrix : public ReachabilityMatrix
{
public:
    HashMatrix(size_t n);
    bool reach(uint32_t from, uint32_t to) const override;
    void set_reach(uint32_t from, uint32_t to, bool is_reachable) override;
    const DSG::Edge *parent(uint32_t from, uint32_t to) const override;
    void set_parent(uint32_t from, uint32_t to, const DSG::Edge *parent) override;
    size_t size() const override;
    size_t capacity() const override;

private:
    size_t n_;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, bool>> reach_;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, const DSG::Edge *>> parent_;
};

class CSRMatrix : public ReachabilityMatrix
{
public:
    CSRMatrix(const std::vector<Vertex> &vertices);
    bool reach(uint32_t from, uint32_t to) const override;
    void set_reach(uint32_t from, uint32_t to, bool is_reachable) override;
    const DSG::Edge *parent(uint32_t from, uint32_t to) const override;
    void set_parent(uint32_t from, uint32_t to, const DSG::Edge *parent) override;
    size_t size() const override;
    size_t capacity() const override;

private:
    void checkAndExpand(size_t requiredSize)
    {
        if (reach_.size() < requiredSize)
        {
            size_t newSize = std::max(reach_.size() * 2, requiredSize);
            reach_.resize(newSize, false);
            parent_.resize(newSize, nullptr);
        }
    }

private:
    std::vector<Vertex> vertices_;
    size_t n_;
    size_t d_ = 10;
    std::vector<bool> reach_;
    std::vector<const DSG::Edge *> parent_;
    std::vector<uint32_t> row_ptr_;
};

class TransitiveClosure
{
public:
    TransitiveClosure(const std::vector<Vertex> &vertices, const VerifyOptions &options);
    bool reach(uint32_t from, uint32_t to) const;
    const DSG::Edge *parent(uint32_t from, uint32_t to) const;
    void set_reach(uint32_t from, uint32_t to, bool is_reachable);
    void set_parent(uint32_t from, uint32_t to, const DSG::Edge *parent);
    std::vector<DSG::Edge> insert(const DSG::Edge &e);
    void construct(const std::vector<DSG::Edge> &edges);
    void backtrace(const std::vector<DSG::Edge> &edges);

private:
    void warshall(const std::vector<DSG::Edge> &edges);
    std::vector<DSG::Edge> warshall(const DSG::Edge &edge);

    void italino(const std::vector<DSG::Edge> &edges);
    std::vector<DSG::Edge> italino(const DSG::Edge &edge);

    void italino_opt(const std::vector<DSG::Edge> &edges);
    std::vector<DSG::Edge> italino_opt(const DSG::Edge &edge);

    void prudom(const std::vector<DSG::Edge> &edges);
    void prudom_opt(const std::vector<DSG::Edge> &edges);

    bool dfs(uint32_t i, std::vector<State> &states, std::queue<uint32_t> &rev_topo_order, std::unordered_map<uint32_t, std::unordered_set<uint32_t>> &adjacency);
    bool dfs_opt(uint32_t i, std::vector<State> &states, std::queue<uint32_t> &rev_topo_order, uint32_t *visited, std::unordered_map<uint32_t, std::unordered_set<uint32_t>> &adjacency);

private:
    std::vector<Vertex> vertices_;
    VerifyOptions options_;
    size_t n_;
    std::unique_ptr<ReachabilityMatrix> matrix_;

    bool solve_;
};

class Descendant
{
public:
    Descendant(uint32_t i);
    void merge(const Descendant &other);
    const std::vector<uint32_t> &s() const;
    uint32_t d() const;

private:
    uint32_t i_;
    uint32_t d_;
    std::vector<uint32_t> s_;
};