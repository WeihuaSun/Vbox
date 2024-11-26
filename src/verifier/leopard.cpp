#include "leopard.h"
#include "exception/isolation_exceptions.h"
using DSG::Edge;
using namespace std;

size_t ReadManager::size() const { return reads_.size(); }
void ReadManager::insert(const Read *read) { reads_.insert(read); }
void ReadManager::remove(const Read *read)
{
    auto it = reads_.find(read);
    if (it != reads_.end())
    {
        reads_.erase(it);
    }
}
const Read *ReadManager::min() const
{
    if (!reads_.empty())
    {
        return *reads_.begin();
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Leopard::Leopard(const VerifyOptions &options)
{
    trx_manager_.load(options.log);
    int i = 0;
    for (const unique_ptr<Transaction> &trx : trx_manager_.transactions())
    {
        vertices_.emplace_back(trx.get(), i);
        i++;
    }
    n_ = i;
}

bool Leopard::run()
{
    try
    {
        vector<uint32_t> actives_vectices;
        for (uint32_t i = 0; i < n_; ++i)
        {
            ww_edges_.clear();
            wr_edges_.clear();
            rw_edges_.clear();
            garbage_.clear();

            Vertex &v = vertices_[i];
            Transaction *trx = v.transaction();
            for (const unique_ptr<Operator> &op : trx->operators())
            {
                switch (op->type())
                {
                case OperatorType::READ:
                {
                    Read *read = static_cast<Read *>(op.get());
                    reads_[i].push_back(read);
                    min_reads_[read->key()].insert(read);
                }
                break;
                case OperatorType::WRITE:
                {
                    Write *write = static_cast<Write *>(op.get());
                    v.set_write(write->key(), write);
                }
                break;
                default:
                    break;
                }
            }

            auto it = actives_vectices.begin();
            while (it != actives_vectices.end())
            {
                Vertex &u = vertices_[*it];
                if (u.end() <= v.start())
                {
                    it = actives_vectices.erase(it);
                    consistent_read(u.index());
                }
                else
                {
                    ++it;
                }
            }
            actives_vectices.push_back(i);
            sort_write(i);
            first_updater_win(i);
            ssi_certifier();
            garbage_collection();
            //v.clear();
        }
        for (uint32_t j : actives_vectices)
        {
            consistent_read(j);
        }
        return true;
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
        return false;
    }
}

vector<uint32_t> Leopard::candidate(Read *read, Vertex &r_trx)
{
    uint64_t key = read->key();
    vector<uint32_t> candidates;
    Vertex *pivot = nullptr;
    const vector<uint32_t> &key_installer = active_install_[key];
    for (auto it = key_installer.rbegin(); it != key_installer.rend(); ++it)
    {
        Vertex &w_trx = vertices_[*it];
        if (w_trx.end() <= r_trx.start())
        {
            if (pivot == nullptr)
            {
                pivot = &w_trx;
                candidates.push_back(*it);
            }
            else
            {
                if (w_trx.end() <= pivot->start())
                {
                    candidates.push_back(*it);
                }
            }
        }
        else
        {
            candidates.push_back(*it);
        }
    }
    return candidates;
}

void Leopard::consistent_read(uint32_t j)
{
    const vector<Read *> reads = reads_.at(j);
    for (Read *read : reads)
    {
        uint64_t key = read->key();
        if (read->from_oid() != 0)
        {
            vector<uint32_t> candidates = candidate(read, vertices_[j]);
            bool find = false;
            for (uint32_t i : candidates)
            {
                Vertex &w_trx = vertices_[i];
                Write *write = w_trx.writes().at(key);
                if (read->from_oid() == write->oid())
                {
                    w_trx.set_read(key, j);
                    wr_edges_.push_back(::Edge(i, j));
                    find = true;
                }
            }
            if (find == false)
            {
                throw InconsistentReads(read);
            }
        }
        min_reads_[key].remove(read);
    }
}

void Leopard::sort_write(uint32_t j)
{
    Vertex &v = vertices_[j];
    for (auto &write : v.writes())
    {
        uint64_t key = write.first;
        Write *v_write = write.second;

        auto active_it = active_install_[key].begin();
        while (active_it != active_install_[key].end())
        {
            uint32_t i = *active_it;
            Vertex &u = vertices_[i];

            if (replacement_time_[key][i] <= v.start())
            {
                auto it = min_reads_.find(key);
                if (it == min_reads_.end() || min_reads_[key].size() == 0 || replacement_time_[key][i] <= min_reads_[key].min()->start())
                {
                    garbage_[key].insert(i);
                    ++active_it;
                    for (auto it = version_order_[key].begin(); it != version_order_[key].end(); ++it)
                    {
                        if (*it == i)
                        {
                            auto nextIt = std::next(it);
                            for (uint32_t k : u.reads().at(key))
                            {
                                rw_edges_.emplace_back(k, *nextIt);
                            }
                            version_order_[key].erase(it);
                            break;
                        }
                    }
                }
                else
                {
                    ++active_it;
                }
            }
            else if (u.end() <= v.start())
            {
                ++active_it;
                replacement_time_[key][i] = min(v.end(), replacement_time_[key][i]);
            }
            else
            {
                ++active_it;
            }
        }
        replacement_time_[key][j] = UINT64_MAX;
        active_install_[key].push_back(j);
    }
}

void Leopard::garbage_collection()
{
    for (auto &entry : garbage_)
    {
        unordered_set<uint32_t> &garbage = entry.second;
        for (uint32_t i : garbage)
        {
            anti_in_.erase(i);
            anti_out_.erase(i);
        }
    }
}

void Leopard::first_updater_win(uint32_t j)
{
    Vertex &v = vertices_[j];
    for (auto &install : v.writes())
    {
        uint64_t key = install.first;
        Write *v_write = install.second;
        auto it = version_order_[key].begin();
        auto pos = version_order_[key].begin();

        while (it != version_order_[key].end())
        {
            uint32_t i = *it;
            Vertex u = vertices_[i];
            Write *u_write = u.writes().at(key);

            uint64_t v_begin_end = v_write->end();
            uint64_t v_commit_start = v.transaction()->operators().back()->start();
            uint64_t u_begin_end = u_write->end();
            uint64_t u_commit_start = u.transaction()->operators().back()->start();
            if (u_commit_start <= v_begin_end)
            {
                pos = ++it;
                ww_edges_.emplace_back(i, j);
            }
            else if (v_commit_start <= u_begin_end)
            {
                ++it;
            }
            else
            {
                throw ConcurrentWrite(u, v);
            }
        }
        version_order_[key].insert(pos, j);
    }
}

void Leopard::to_certifier(const vector<::Edge> &edges)
{
    for (const ::Edge &e : edges)
    {
        if (vertices_[e.to()].end() <= vertices_[e.from()].start())
        {
            throw TOCertifierFailed(vertices_[e.from()], vertices_[e.to()]);
        }
    }
}

void Leopard::ssi_certifier()
{
    for (const ::Edge &e : rw_edges_)
    {
        uint32_t from = e.from();
        uint32_t to = e.to();
        if (from == to || vertices_[from].end() <= vertices_[to].start())
        {
            return;
        }
        if (anti_out_.find(to) != anti_out_.end())
        {
            throw SSICertifierFailed(vertices_[anti_out_[to]], vertices_[from], vertices_[to]);
        }
        else
        {
            anti_in_[to] = from;
        }
        if (anti_in_.find(from) != anti_in_.end())
        {
            throw SSICertifierFailed(vertices_[anti_in_[from]], vertices_[from], vertices_[to]);
        }
        else
        {
            anti_out_[from] = to;
        }
    }
}