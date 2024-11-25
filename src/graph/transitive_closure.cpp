#include <bitset>
#include "transitive_closure.h"
#include "exception/isolation_exceptions.h"

using namespace std;
using DSG::Edge;
StandardMatrix::StandardMatrix(size_t n)
    : n_(n), reach_(n, vector<bool>(n, false)), parent_(n, vector<const Edge *>(n, nullptr)) {}

const bool StandardMatrix::reach(uint32_t from, uint32_t to) const { return reach_[from][to]; }
void StandardMatrix::set_reach(uint32_t from, uint32_t to, bool is_reachable) { reach_[from][to] = is_reachable; }
const Edge *StandardMatrix::parent(uint32_t from, uint32_t to) const { return parent_[from][to]; }
void StandardMatrix::set_parent(uint32_t from, uint32_t to, const Edge *parent) { parent_[from][to] = parent; }
size_t StandardMatrix::size() const { return n_; }
size_t StandardMatrix::capacity() const { return n_ * n_ * (sizeof(bool) + sizeof(Edge *)); }

HashMatrix::HashMatrix(size_t n) : n_(n) {}
const bool HashMatrix::reach(uint32_t from, uint32_t to) const
{
    auto from_iter = reach_.find(from);
    if (from_iter != reach_.end())
    {
        auto to_iter = from_iter->second.find(to);
        return to_iter != from_iter->second.end() && to_iter->second;
    }
    return false;
}
void HashMatrix::set_reach(uint32_t from, uint32_t to, bool is_reachable) { reach_[from][to] = is_reachable; }
const Edge *HashMatrix::parent(uint32_t from, uint32_t to) const
{
    auto from_iter = parent_.find(from);
    if (from_iter != parent_.end())
    {
        auto to_iter = from_iter->second.find(to);
        return to_iter != from_iter->second.end() ? to_iter->second : nullptr;
    }
    return nullptr;
}
void HashMatrix::set_parent(uint32_t from, uint32_t to, const Edge *parent) { parent_[from][to] = parent; }
size_t HashMatrix::size() const { return n_; }
size_t HashMatrix::capacity() const { return n_ * n_ * (sizeof(bool) + sizeof(Edge *)); } // to-fix

// CSRMatrix::CSRMatrix(size_t n) : n_(n) {}

// const bool CSRMatrix::reach(uint32_t from, uint32_t to) const
// {
//     int start = row_ptr_[from];
//     int offset = to - start_[from];
//     if (start == 0)
//     {
//         return false;
//     }
//     if (offset >= 0 && offset < (end_[from] - start_[from]))
//     {
//         return reach_[start + offset];
//     }
//     else
//     {
//         throw std::runtime_error("Element not found.");
//     }
// };

// void CSRMatrix::set_reach(uint32_t from, uint32_t to, bool is_reachable)
// {
//     assert(from < row_ptr_.size());
//     int start = row_ptr_[from];
//     int numValues = row_ptr_[from + 1] - start;
//     for (int i = start; i < start + numValues; ++i)
//     {
//         if (i == to)
//         {
//             reach_[i] = is_reachable;
//             return;
//         }
//     }
//     checkAndExpand(size_);
//     reach_[size_++] = is_reachable;
//     row_ptr_[from + 1] = size_;
// }

// size_t CSRMatrix::size() const { return n_; }
//////////////////////////////////////////////////////////////////////////////////////////
TransitiveClosure::TransitiveClosure(const std::vector<Vertex> &vertices, const VerifyOptions &options) : vertices_(vertices), options_(options), n_(vertices.size())
{
}
bool TransitiveClosure::reach(uint32_t from, uint32_t to) { return matrix_->reach(from, to); }

std::vector<Edge> TransitiveClosure::insert(const Edge &e)
{
    switch (options_.update_t)
    {
    case Updater::U_WARSHALL:
        warshall(e);
        break;
    case Updater::U_ITALINO:
        italino(e);
        break;
    case Updater::U_ITALINO_OPT:
        italino_opt(e);
    default:
        break;
    }
}

void TransitiveClosure::warshall(const std::vector<Edge> &edges)
{
    for (const Edge &e : edges)
    {
        if (!reach(e.from(), e.to()))
        {
            set_reach(e.from(), e.to(), true);
        }
    }
    for (int k = 0; k < n_; k++)
    {
        for (int i = 0; i < n_; i++)
        {
            for (int j = 0; j < n_; j++)
            {
                if (reach(i, k) && reach(k, j) && !reach(i, j))
                {
                    set_reach(i, j, true);
                }
            }
        }
    }
}

vector<Edge> TransitiveClosure::warshall(const Edge &e)
{
    vector<Edge> record;
    if (reach(e.from(), e.to()))
    {
        set_reach(e.from(), e.to(), true);
    }
    for (int k = 0; k < n_; k++)
    {
        for (int i = 0; i < n_; i++)
        {
            for (int j = 0; j < n_; j++)
            {
                if (reach(i, k) && reach(k, j) && !reach(i, j))
                {
                    set_reach(i, j, true);
                    record.push_back(Edge(i, j));
                }
            }
        }
    }
    return record;
}

vector<Edge> TransitiveClosure::italino(const Edge &e)
{
    vector<Edge> record;
    if (!reach(e.from(), e.to()))
    {
        for (uint32_t i = 0; i < n_; ++i)
        {
            if (reach(i, e.from()) && !reach(i, e.to()))
            {
                for (uint32_t j = 0; j < n_; ++j)
                {
                    if (reach(e.to(), j) && !reach(i, j))
                    {
                        set_reach(i, j, true);
                        record.emplace_back(i, j);
                        if (solve)
                        {
                            set_parent(i, j, &e);
                        }
                    }
                }
            }
        }
    }
}

void TransitiveClosure::italino(const std::vector<Edge> &edges)
{
    for (const Edge &e : edges)
    {
        italino(e);
    }
}

void TransitiveClosure::italino_opt(const std::vector<Edge> &edges)
{
    for (const Edge &e : edges)
    {
        italino_opt(e);
    }
}

vector<Edge> TransitiveClosure::italino_opt(const Edge &e)
{
    vector<Edge> record;
    Vertex &s = vertices_[e.from()];
    Vertex &t = vertices_[e.to()];

    if (!reach(e.from(), e.to()))
    {
        uint32_t u_left = t.left();
        uint32_t u_right = min(s.right(), t.right());
        for (uint32_t i = u_left; i < u_right; ++i)
        {
            if (reach(i, e.from()) && !reach(i, e.to()))
            {
                Vertex &u = vertices_[i];
                uint32_t v_left = max(max(u.left(), s.left()), t.left());
                uint32_t v_right = min(u.right(), s.right());
                for (uint32_t j = v_left; j < v_right; ++j)
                {
                    if (reach(e.to(), j) && !reach(i, j))
                    {
                        set_reach(i, j, true);
                        record.emplace_back(i, j);
                        if (solve)
                        {
                            set_parent(i, j, &e);
                        }
                    }
                }
            }
        }
    }
}

void TransitiveClosure::prudom(const std::vector<Edge> edges)
{
    queue<uint32_t> rev_topo_order;
    vector<State> states(n_, State::UNVISITED);

    unordered_map<uint32_t, bitset<MAX_VERTICES>> descendants;
    while (!rev_topo_order.empty())
    {
        uint32_t i = rev_topo_order.front();
        rev_topo_order.pop();
        auto it = adjacency_.find(i);
        if (it != adjacency_.end())
        {
            unordered_set<uint32_t> &succs = it->second;
            for (uint32_t s : succs)
            {
                descendants[i] |= descendants[s];
            }
        }

        descendants[i].set(i);

        for (uint32_t j = vertices_[i].left(); j < vertices_[i].right(); ++j)
        {
            if (descendants[i][j])
            {
                set_reach(i, j, true);
            }
        }
    }
}

bool TransitiveClosure::dfs(uint32_t i, vector<State> &states, queue<uint32_t> &rev_topo_order)
{
    states[i] = State::VISITING;
    auto it = adjacency_.find(i);
    if (it != adjacency_.end())
    {
        unordered_set<uint32_t> &succs = it->second;
        for (uint32_t s : succs)
        {
            if (states[s] == State::UNVISITED)
            {
                if (dfs(s, states, rev_topo_order))
                {
                    return true;
                }
            }
            else if (states[s] == State::VISITING)
            {
                return true;
            }
        }
    }
    states[i] = State::VISITED;
    rev_topo_order.push(i);
    return false;
}

bool TransitiveClosure::dfs_opt(uint32_t i, vector<State> &states, queue<uint32_t> &rev_topo_order, uint32_t *visited)
{
    states[i] = State::VISITING;
    auto it = adjacency_.find(i);
    if (it != adjacency_.end())
    {
        unordered_set<uint32_t> &succs = it->second;
        for (uint32_t s : succs)
        {
            if (states[s] == State::UNVISITED)
            {
                if (dfs_opt(s, states, rev_topo_order, visited))
                {
                    return true;
                }
                *visited = min(*visited, vertices_[s].right());
            }
            else if (states[s] == State::VISITING)
            {
                return true;
            }
        }
    }
    if (vertices_[i].right() < *visited)
    {
        for (uint32_t j = vertices_[i].right(); j < *visited; ++j)
        {
            if (states[j] == State::UNVISITED)
            {
                if (dfs_opt(j, states, rev_topo_order, visited))
                {
                    return true;
                }
                *visited = min(*visited, vertices_[j].right());
            }
            else if (states[j] == State::VISITING)
            {
                return true;
            }
        }
    }
    states[i] = State::VISITED;
    rev_topo_order.push(i);
    return false;
}

void TransitiveClosure::prudom_opt(const std::vector<Edge> &edges)
{
    queue<uint32_t> rev_topo_order;
    vector<State> states(n_, State::UNVISITED);
    // topological sort
    uint32_t visited = n_ - 1;
    for (uint32_t i = 0; i < n_; ++i)
    {
        if (states[i] == State::UNVISITED)
        {
            if (dfs_opt(i, states, rev_topo_order, &visited))
            {
                throw SerializableException("prudom_opt:cycle.");
            }
        }
    }
    // merge
    vector<Descendant> descendants;

    while (!rev_topo_order.empty())
    {
        uint32_t i = rev_topo_order.front();
        rev_topo_order.pop();
        auto it = adjacency_.find(i);
        if (it != adjacency_.end())
        {
            unordered_set<uint32_t> &succs = it->second;
            for (uint32_t s : succs)
            {
                descendants[i].merge(descendants[s]);
            }
        }

        for (uint32_t j = vertices_[i].right(); j < vertices_[vertices_[i].right()].right(); ++j)
        {
            descendants[i].merge(descendants[j]);
        }

        for (uint32_t j : descendants[i].s())
        {
            set_reach(i, j, true);
        }
        for (int j = descendants[i].d(); j < vertices_[i].right(); ++j)
        {
            set_reach(i, j, true);
        }
    }
}

void TransitiveClosure::backtrace(const std::vector<Edge> &edges)
{
    for (const Edge &e : edges)
    {
        set_reach(e.from(), e.to(), false);
        set_parent(e.from(), e.to(), nullptr);
    }
}

/////////////////////////////////////////////////////////////////////////
Descendant::Descendant(uint32_t i) : i_(i) { s_.push_back(i); }

void Descendant::merge(const Descendant &other)
{
    d_ = min(other.d_, d_);
    int m = 0;
    int n = 0;
    int q = 0;
    int s1 = s_.size();
    int s2 = other.s_.size();

    vector<uint32_t> new_s(s1 + s2);
    while (m < s1 && n < s2)
    {
        if (s_[m] == other.s_[n])
        {
            if (s_[m] >= d_)
            {
                s1 = m;
                s2 = n;
                break;
            }
            else
            {
                new_s[q] = s_[m];
                m++;
                n++;
            }
        }
        else if (s_[m] < other.s_[m])
        {
            if (s_[m] >= d_)
            {
                s1 = m;
                s2 = n;
                break;
            }
            else
            {
                new_s[q] = s_[m];
                m++;
            }
        }
        else
        {
            if (other.s_[n] >= d_)
            {
                s1 = m;
                s2 = n;
                break;
            }
            else
            {
                new_s[q] = other.s_[n];
                n++;
            }
        }
        q++;
    }

    while (m < s1)
    {
        if (s_[m] >= d_)
        {
            break;
        }
        else
        {
            new_s[q] = s_[m];
            m++;
            q++;
        }
    };
    while (n < s2)
    {
        if (other.s_[n] >= d_)
        {
            break;
        }
        else
        {
            new_s[q] = other.s_[n];
            n++;
            q++;
        }
    };
    new_s.resize(q);
    s_ = new_s;
}

const std::vector<uint32_t> &Descendant::s() const{return s_;}
uint32_t Descendant::d() const{return d_;}