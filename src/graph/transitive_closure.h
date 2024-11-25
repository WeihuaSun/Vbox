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
    virtual const bool reach(uint32_t from, uint32_t to) const = 0;
    virtual const Edge *parent(uint32_t from, uint32_t to) const = 0;
    virtual void set_parent(uint32_t from, uint32_t to, const Edge *parent) = 0;
    virtual size_t size() const = 0;
    virtual size_t capacity() const = 0;
};

class StandardMatrix : public ReachabilityMatrix
{
public:
    StandardMatrix(size_t n);
    const bool reach(uint32_t from, uint32_t to) const override;
    void set_reach(uint32_t from, uint32_t to, bool is_reachable) override;
    const Edge *parent(uint32_t from, uint32_t to) const override;
    void set_parent(uint32_t from, uint32_t to, const Edge *parent) override;
    size_t size() const override;
    size_t capacity() const override;

private:
    size_t n_;
    std::vector<std::vector<bool>> reach_;
    std::vector<std::vector<const Edge *>> parent_;
};

#include <unordered_map>

class HashMatrix : public ReachabilityMatrix
{
public:
    HashMatrix(size_t n);
    const bool reach(uint32_t from, uint32_t to) const override;
    void set_reach(uint32_t from, uint32_t to, bool is_reachable) override;
    const Edge *parent(uint32_t from, uint32_t to) const override;
    void set_parent(uint32_t from, uint32_t to, const Edge *parent) override;
    size_t size() const override;
    size_t capacity() const override;

private:
    size_t n_;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, bool>> reach_;
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, const Edge *>> parent_;
};

// class CSRMatrix : public ReachabilityMatrix
// {
// public:
//     CSRMatrix(size_t n);
//     const bool reach(uint32_t from, uint32_t to) const override;
//     void set_reach(uint32_t from, uint32_t to, bool is_reachable) override;
//     size_t size() const override;
//     size_t capacity() const override;

//     void set_reach(uint32_t from, uint32_t to, bool is_reachable) override
//     {
//         if (from >= row_ptr_.size() - 1)
//         {
//             throw std::out_of_range("Row index out of bounds.");
//         }
//         int start = row_ptr_[from];
//         int numValues = row_ptr_[from + 1] - start;

//         for (int i = start; i < start + numValues; ++i)
//         {
//             if (i == to)
//             {
//                 values_[i] = is_reachable;
//                 return;
//             }
//         }
//         // No match found, expand matrix
//         checkAndExpand(size_);
//         values_[size_++] = is_reachable;
//         row_ptr_[from + 1] = size_;
//     }

//     bool reach(uint32_t from, uint32_t to) const override
//     {
//         if (from >= row_ptr_.size() - 1 || to >= row_ptr_.size() - 1)
//         {
//             return false;
//         }
//         int start = row_ptr_[from];
//         int end = row_ptr_[from + 1];

//         for (int i = start; i < end; ++i)
//         {
//             if (i == to)
//             {
//                 return values_[i];
//             }
//         }
//         return false;
//     }

// private:
//     void preAllocate(int n, int k)
//     {
//         row_ptr_.resize(n + 1, 0);
//         reach_.resize(n * k, false);

//     }
//     void checkAndExpand(size_t requiredSize)
//     {
//         if (reach_.size() < requiredSize)
//         {
//             size_t newSize = std::max(reach_.size() * 2, requiredSize);
//             reach_.resize(newSize, false);
//             parent_.resize(newSize, nullptr);
//         }
//     }

// private:
//     size_t n_;
//     size_t d_ = 10;
//     std::vector<bool> reach_;
//     std::vector<Edge *> parent_;
//     std::vector<uint32_t> row_ptr_;
//     std::vector<uint32_t> start_;
//     std::vector<uint32_t> end_;
// };

class TransitiveClosure
{
public:
    TransitiveClosure(const std::vector<Vertex> &vertices, const VerifyOptions &options);
    bool reach(uint32_t from, uint32_t to);
    Edge parent(uint32_t from, uint32_t to);
    void set_reach(uint32_t from, uint32_t to, bool is_reachable);
    void set_parent(uint32_t from, uint32_t to, const Edge *parent);
    std::vector<Edge> insert(const Edge &e);

    void backtrace(const std::vector<Edge> &edges);
    bool dfs(uint32_t i, std::vector<State> &states, std::queue<uint32_t> &rev_topo_order);
    bool dfs_opt(uint32_t i, std::vector<State> &states, std::queue<uint32_t> &rev_topo_order, uint32_t *visited);

private:
    void warshall(const std::vector<Edge> &edges);
    std::vector<Edge> warshall(const Edge &edge);

    void italino(const std::vector<Edge> &edges);
    std::vector<Edge> italino(const Edge &edge);

    void italino_opt(const std::vector<Edge> &edges);
    std::vector<Edge> italino_opt(const Edge &edge);

    void prudom(const std::vector<Edge> edges);
    void prudom_opt(const std::vector<Edge> &edges);

private:
    std::vector<Vertex> vertices_;
    VerifyOptions options_;
    size_t n_;
    std::unique_ptr<ReachabilityMatrix> matrix_;
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> adjacency_;
    bool solve;
};

class Descendant
{
public:
    Descendant();
    void merge(Descendant &other);

private:
    uint32_t i;
    uint32_t d;
    std::vector<uint32_t> s;
};