#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_set>
#include <unordered_map>
#include <queue>

#include "transaction/transaction.h"



inline size_t hash_combine(size_t seed, size_t value) { return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2)); }

class Vertex
{
public:
    Vertex(Transaction *trx) : trx_(trx) {}
    Vertex(Transaction *trx, uint32_t index)
        : trx_(trx), index_(index) {}
    Vertex(Transaction *trx, uint32_t index, uint32_t left, uint32_t right)
        : trx_(trx), index_(index), left_(left), right_(right) {}
    ~Vertex() = default;

    uint32_t index() const;
    void set_index(uint32_t index);

    uint32_t left() const;
    void set_left(uint32_t left);

    uint32_t right() const;
    void set_right(uint32_t right);

    uint64_t start() const;
    uint64_t end() const;

    const std::unordered_map<uint64_t, std::unordered_set<uint32_t>> &reads() const;
    void set_read(uint64_t k, uint32_t v);

    const std::unordered_map<uint64_t, Write *> &writes() const;
    void set_write(uint64_t k, Write *w);
    void set_predicate(Predicate *p);
    const std::unordered_set<Predicate *> predicates() const;

    Transaction *transaction() const;

    std::string to_string() const;

private:
    Transaction *trx_;
    uint32_t index_;
    uint32_t left_ = 0;
    uint32_t right_ = UINT32_MAX;
    std::unordered_map<uint64_t, std::unordered_set<uint32_t>> reads_;
    std::unordered_map<uint64_t, Write *> writes_;
    std::unordered_set<Predicate *> predicates_;
};

namespace DSG
{
    class Edge
    {
    public:
        Edge();
        Edge(uint32_t s, uint32_t t);
        ~Edge() = default;

        uint32_t from() const;
        uint32_t to() const;

        bool operator==(const Edge &other) const
        {
            return s_ == other.s_ && t_ == other.t_;
        }

    private:
        uint32_t s_;
        uint32_t t_;
    };
}

namespace std
{
    template <>
    struct hash<DSG::Edge>
    {
        std::size_t operator()(const DSG::Edge &edge) const
        {
            size_t seed = 0;
            seed = hash_combine(seed, std::hash<uint32_t>{}(edge.from()));
            seed = hash_combine(seed, std::hash<uint32_t>{}(edge.to()));
            return seed;
        }
    };
}

class DirectedGraph
{
private:
    std::vector<std::unordered_set<int>> adjList;

public:
    int V;

    DirectedGraph(int vertices) : V(vertices)
    {
        adjList.resize(vertices);
    }

    DirectedGraph() : V(0) {}

    void addNodes(int vertices)
    {
        adjList.resize(vertices);
        V = vertices;
    }

    void addEdge(int u, int v)
    {
        if (u >= V || v >= V)
            return;
        if (u == v)
            return;
        adjList[u].insert(v);
    }

    bool isReachableDFS(int start, int goal)
    {
        if (start >= V || goal >= V)
            return false;
        std::vector<bool> visited(V, false);
        return dfs(start, goal, visited);
    }

    bool isReachableBFS(int start, int goal)
    {
        if (start >= V || goal >= V)
            return false;
        std::vector<bool> visited(V, false);
        std::queue<int> queue;

        visited[start] = true;
        queue.push(start);

        while (!queue.empty())
        {
            int current = queue.front();
            queue.pop();

            if (current == goal)
                return true;

            for (int neighbor : adjList[current])
            {
                if (!visited[neighbor])
                {
                    visited[neighbor] = true;
                    queue.push(neighbor);
                }
            }
        }

        return false;
    }

private:
    bool dfs(int current, int goal, std::vector<bool> &visited)
    {
        visited[current] = true;

        if (current == goal)
            return true;

        for (int neighbor : adjList[current])
        {
            if (!visited[neighbor])
            {
                if (dfs(neighbor, goal, visited))
                    return true;
            }
        }
        return false;
    }
};

#endif