#include <bitset>
#include "transitive_closure.h"
#include "exception/isolation_exceptions.h"

using namespace std;
using DSG::Edge;
StandardMatrix::StandardMatrix(size_t n)
    : n_(n), reach_(n, vector<bool>(n, false)), parent_(n, vector<const Edge *>(n, nullptr)) {}
bool StandardMatrix::reach(uint32_t from, uint32_t to) const { return reach_[from][to]; }
void StandardMatrix::set_reach(uint32_t from, uint32_t to, bool is_reachable) { reach_[from][to] = is_reachable; }
const Edge *StandardMatrix::parent(uint32_t from, uint32_t to) const { return parent_[from][to]; }
void StandardMatrix::set_parent(uint32_t from, uint32_t to, const Edge *parent) { parent_[from][to] = parent; }
size_t StandardMatrix::size() const { return n_; }
size_t StandardMatrix::capacity() const { return n_ * n_ * (sizeof(bool) + sizeof(Edge *)); }

HashMatrix::HashMatrix(size_t n) : n_(n) {}
bool HashMatrix::reach(uint32_t from, uint32_t to) const
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

CSRMatrix::CSRMatrix(const vector<Vertex> &vertices) : vertices_(vertices), n_(vertices.size())
{
    row_ptr_.resize(n_ + 1, 0);
    reach_.resize(n_ * d_, false);
    parent_.resize(n_ * d_, nullptr);
    size_t size = 0;
    for (const Vertex &v : vertices)
    {
        row_ptr_[v.index()] = size;
        size += v.right() - v.left();
    }
    assert(size <= n_ * d_);
}

bool CSRMatrix::reach(uint32_t from, uint32_t to) const
{
    if (vertices_[from].end() <= vertices_[to].start())
    {
        return true;
    }
    else if (vertices_[to].end() <= vertices_[from].start())
    {
        return false;
    }
    int row_start = row_ptr_[from];
    int offset = to - vertices_[from].left();
    return reach_[row_start + offset];
}

void CSRMatrix::set_reach(uint32_t from, uint32_t to, bool is_reachable)
{
    int row_start = row_ptr_[from];
    int offset = to - vertices_[from].left();
    reach_[row_start + offset] = is_reachable;
}

const Edge *CSRMatrix::parent(uint32_t from, uint32_t to) const
{
    if (vertices_[from].end() <= vertices_[to].start())
    {
        return nullptr;
    }
    else if (vertices_[to].end() <= vertices_[from].start())
    {
        return nullptr;
    }
    int row_start = row_ptr_[from];
    int offset = to - vertices_[from].left();
    return parent_[row_start + offset];
}

void CSRMatrix::set_parent(uint32_t from, uint32_t to, const Edge *parent)
{
    int row_start = row_ptr_[from];
    int offset = to - vertices_[from].left();
    parent_[row_start + offset] = parent;
}

size_t CSRMatrix::size() const { return n_; }
size_t CSRMatrix::capacity() const { return sizeof(reach_) + sizeof(parent_) + sizeof(row_ptr_); }
//////////////////////////////////////////////////////////////////////////////////////////
TransitiveClosure::TransitiveClosure(const vector<Vertex> &vertices, const VerifyOptions &options) : vertices_(vertices), options_(options), n_(vertices.size())
{
}
void TransitiveClosure::create()
{
    if (options_.compact)
    {
        matrix_ = make_unique<CSRMatrix>(vertices_);
    }
    else
    {
        matrix_ = make_unique<StandardMatrix>(n_);
    }
}

bool TransitiveClosure::reach(uint32_t from, uint32_t to) const { return matrix_->reach(from, to); }
const ::Edge *TransitiveClosure::parent(uint32_t from, uint32_t to) const { return matrix_->parent(from, to); }
void TransitiveClosure::set_reach(uint32_t from, uint32_t to, bool is_reachable) { matrix_->set_reach(from, to, is_reachable); }
void TransitiveClosure::set_parent(uint32_t from, uint32_t to, const ::Edge *parent) { matrix_->set_parent(from, to, parent); }
vector<const ::Edge *> TransitiveClosure::path(uint32_t from, uint32_t to) const
{
    vector<const ::Edge *> total_path;
    if (from == to)
    {
        return total_path;
    }
    const ::Edge *e = parent(from, to);
    vector<const ::Edge *> left_path = path(from, e->from());
    vector<const ::Edge *> right_path = path(e->to(), to);
    // Combine the left path, the current edge, and the right path
    total_path.insert(total_path.end(), left_path.begin(), left_path.end());
    total_path.push_back(e);
    total_path.insert(total_path.end(), right_path.begin(), right_path.end());
    return total_path;
}

vector<Edge> TransitiveClosure::insert(const Edge &e)
{
    vector<Edge> r;
    switch (options_.update_t)
    {
    case Updater::U_WARSHALL:
        return warshall(e);
    case Updater::U_ITALINO:
        return italino(e);
    case Updater::U_ITALINO_OPT:
        return italino_opt(e);
    default:
        break;
    }
    return r;
}

void TransitiveClosure::construct(const vector<::Edge> &edges)
{
    cout<<int(options_.construct_t)<<endl;
    switch (options_.construct_t)
    {
    case Constructor::C_WARSHALL:
        warshall(edges);
        break;
    case Constructor::C_ITALINO:
        italino(edges);
        break;
    case Constructor::C_ITALINO_OPT:
        italino_opt(edges);
        break;
    case Constructor::C_PURDOM:
        prudom(edges);
        break;
    case Constructor::C_PURDOM_OPT:
        prudom_opt(edges);
        break;
    default:
        break;
    }
}

void TransitiveClosure::warshall(const vector<Edge> &edges)
{
    cout<<"warshall"<<endl;
    for (const Edge &e : edges)
    {
        if (!reach(e.from(), e.to()))
        {
            set_reach(e.from(), e.to(), true);
        }
    }
    for (size_t k = 0; k < n_; k++)
    {
        for (size_t i = 0; i < n_; i++)
        {
            for (size_t j = 0; j < n_; j++)
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
    for (size_t k = 0; k < n_; k++)
    {
        for (size_t i = 0; i < n_; i++)
        {
            for (size_t j = 0; j < n_; j++)
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
                        if (solve_)
                        {
                            set_parent(i, j, &e);
                        }
                    }
                }
            }
        }
    }
    return record;
}

void TransitiveClosure::italino(const vector<Edge> &edges)
{
    for (const Edge &e : edges)
    {
        italino(e);
    }
}

void TransitiveClosure::italino_opt(const vector<Edge> &edges)
{
    for (const Edge &e : edges)
    {
        italino_opt(e);
    }
}

vector<Edge> TransitiveClosure::italino_opt(const Edge &e)
{
    vector<Edge> record;
    const Vertex &s = vertices_[e.from()];
    const Vertex &t = vertices_[e.to()];

    if (!reach(e.from(), e.to()))
    {
        uint32_t u_left = t.left();
        uint32_t u_right = min(s.right(), t.right());
        for (uint32_t i = u_left; i < u_right; ++i)
        {
            if (reach(i, e.from()) && !reach(i, e.to()))
            {
                const Vertex &u = vertices_[i];
                uint32_t v_left = max(max(u.left(), s.left()), t.left());
                uint32_t v_right = min(u.right(), s.right());
                for (uint32_t j = v_left; j < v_right; ++j)
                {
                    if (reach(e.to(), j) && !reach(i, j))
                    {
                        set_reach(i, j, true);
                        record.emplace_back(i, j);
                        if (solve_)
                        {
                            set_parent(i, j, &e);
                        }
                    }
                }
            }
        }
    }
    return record;
}

void TransitiveClosure::prudom(const vector<Edge> &edges)
{
    queue<uint32_t> rev_topo_order;
    vector<State> states(n_, State::UNVISITED);
    unordered_map<uint32_t, unordered_set<uint32_t>> adjacency;

    for (const Edge &e : edges)
    {
        adjacency[e.from()].insert(e.to());
    }

    for (uint32_t i = 0; i < n_; ++i)
    {
        if (states[i] == State::UNVISITED)
        {
            if (dfs(i, states, rev_topo_order, adjacency))
            {
                throw SerializableException("prudom:cycle.");
            }
        }
    }

    unordered_map<uint32_t, bitset<MAX_VERTICES>> descendants;
    while (!rev_topo_order.empty())
    {
        uint32_t i = rev_topo_order.front();
        rev_topo_order.pop();
        auto it = adjacency.find(i);
        if (it != adjacency.end())
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

bool TransitiveClosure::dfs(uint32_t i, vector<State> &states, queue<uint32_t> &rev_topo_order, unordered_map<uint32_t, unordered_set<uint32_t>> &adjacency)
{
    states[i] = State::VISITING;
    auto it = adjacency.find(i);
    if (it != adjacency.end())
    {
        unordered_set<uint32_t> &succs = it->second;
        for (uint32_t s : succs)
        {
            if (states[s] == State::UNVISITED)
            {
                if (dfs(s, states, rev_topo_order, adjacency))
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

bool TransitiveClosure::dfs_opt(uint32_t i, vector<State> &states, queue<uint32_t> &rev_topo_order, uint32_t *visited, unordered_map<uint32_t, unordered_set<uint32_t>> &adjacency)
{
    assert(i < n_);
    states[i] = State::VISITING;
    auto it = adjacency.find(i);
    if (it != adjacency.end())
    {
        unordered_set<uint32_t> &succs = it->second;
        for (uint32_t s : succs)
        {
            assert(s < n_);
            if (states[s] == State::UNVISITED)
            {
                if (dfs_opt(s, states, rev_topo_order, visited, adjacency))
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
            assert(j < n_);
            if (states[j] == State::UNVISITED)
            {
                if (dfs_opt(j, states, rev_topo_order, visited, adjacency))
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
    assert(i < n_);
    states[i] = State::VISITED;
    rev_topo_order.push(i);
    return false;
}

void TransitiveClosure::prudom_opt(const vector<Edge> &edges)
{
    queue<uint32_t> rev_topo_order;
    vector<State> states_map(n_, State::UNVISITED);

    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> adjacency;

    for (const Edge &e : edges)
    {
        adjacency[e.from()].insert(e.to());
    }
    //  topological sort
    uint32_t visited = n_ - 1;
    for (uint32_t i = 0; i < n_; ++i)
    {
        if (states_map[i] == State::UNVISITED)
        {
            if (dfs_opt(i, states_map, rev_topo_order, &visited, adjacency))
            {
                throw SerializableException("prudom_opt:cycle.");
            }
        }
    }
    states_map.clear();
    // merge
    vector<Descendant> descendants;
    descendants.reserve(n_);
    for (uint32_t i = 0; i < n_; ++i)
    {
        descendants.emplace_back(i, vertices_[i].right());
    }

    while (!rev_topo_order.empty())
    {
        uint32_t i = rev_topo_order.front();
        rev_topo_order.pop();
        auto it = adjacency.find(i);
        if (it != adjacency.end())
        {
            unordered_set<uint32_t> &succs = it->second;
            for (uint32_t s : succs)
            {
                descendants[i].merge(descendants[s]);
            }
        }
        if (vertices_[i].right() != n_)
        {
            for (uint32_t j = vertices_[i].right(); j < vertices_[vertices_[i].right()].right(); ++j)
            {
                descendants[i].merge(descendants[j]);
            }
        }
        for (uint32_t j : descendants[i].s())
        {
            set_reach(i, j, true);
        }
        for (uint32_t j = descendants[i].d(); j < vertices_[i].right(); ++j)
        {
            set_reach(i, j, true);
        }
    }
}

void TransitiveClosure::backtrace(const vector<Edge> &edges)
{
    for (const Edge &e : edges)
    {
        set_reach(e.from(), e.to(), false);
        set_parent(e.from(), e.to(), nullptr);
    }
}

/////////////////////////////////////////////////////////////////////////
Descendant::Descendant(uint32_t i, uint32_t d) : i_(i), d_(d) { s_.push_back(i); }

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

const vector<uint32_t> &Descendant::s() const { return s_; }
uint32_t Descendant::d() const { return d_; }