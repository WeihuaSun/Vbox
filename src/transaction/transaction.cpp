#include "transaction.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <dirent.h>
#include <sys/stat.h>
#include <queue>
#include <unordered_set>
using namespace std;

Operator::Operator(uint32_t oid, uint64_t start, uint64_t end)
    : oid_(oid), start_(start), end_(end) {}

uint32_t Operator::oid() const { return oid_; }
uint64_t Operator::start() const { return start_; }
uint64_t Operator::end() const { return end_; }

Read::Read(uint32_t oid, uint64_t start, uint64_t end, uint64_t key, uint32_t from_tid, uint32_t from_oid)
    : Operator(oid, start, end), key_(key), from_tid_(from_tid), from_oid_(from_oid) {}
OperatorType Read::type() const { return OperatorType::READ; }

uint64_t Read::key() const { return key_; }
uint32_t Read::from_tid() const { return from_tid_; }
uint32_t Read::from_oid() const { return from_oid_; }

string Read::to_string() const
{
    ostringstream oss;
    oss << "Read(oid=" << oid_ << ", start=" << start_ << ", end=" << end_ << ", key=" << key_ << ", from_tid=" << from_tid_ << ", from_oid=" << from_oid_ << ")";
    return oss.str();
}

Write::Write(uint32_t oid, uint64_t start, uint64_t end, uint64_t key, uint64_t field, uint32_t value)
    : Operator(oid, start, end), key_(key)
{
    updates_[field] = value;
    fields_.insert(field);
    values_.insert(value);
}
OperatorType Write::type() const { return OperatorType::WRITE; }

uint64_t Write::key() const { return key_; }
unordered_map<uint64_t, uint32_t> Write::updates() { return updates_; }

void Write::combine(Write *other)
{
    for (const auto &update : other->updates())
    {
        uint64_t filed = update.first;
        uint32_t value = update.second;
        if (updates_.count(filed) > 0)
        {
            updates_[filed] = oid_ > other->oid() ? updates_[filed] : value;
        }
        else
        {
            updates_[filed] = value;
        }
    }
    oid_ = max(oid_, other->oid());
}

string Write::to_string() const
{
    ostringstream oss;
    oss << "Write(oid=" << oid_ << ", start=" << start_ << ", end=" << end_ << ", key=" << key_ << ")";
    return oss.str();
}

Begin::Begin(uint32_t oid, uint64_t start, uint64_t end) : Operator(oid, start, end) {}
OperatorType Begin::type() const { return OperatorType::BEGIN; }

string Begin::to_string() const
{
    ostringstream oss;
    oss << "Begin(oid=" << oid_ << ", start=" << start_ << ", end=" << end_ << ")";
    return oss.str();
}

Commit::Commit(uint32_t oid, uint64_t start, uint64_t end) : Operator(oid, start, end) {}
OperatorType Commit::type() const { return OperatorType::COMMIT; }

string Commit::to_string() const
{
    ostringstream oss;
    oss << "Commit(oid=" << oid_ << ", start=" << start_ << ", end=" << end_ << ")";
    return oss.str();
}

Abort::Abort(uint32_t oid, uint64_t start, uint64_t end) : Operator(oid, start, end) {}
OperatorType Abort::type() const { return OperatorType::ABORT; }

string Abort::to_string() const
{
    ostringstream oss;
    oss << "Abort(oid=" << oid_ << ", start=" << start_ << ", end=" << end_ << ")";
    return oss.str();
}

Predicate::Predicate(uint32_t oid, uint64_t start, uint64_t end, uint64_t field, uint32_t left_bound,
                     uint32_t right_bound, unordered_set<uint64_t> keys, unordered_set<uint32_t> from_tids, unordered_set<uint32_t> from_oids)
    : Operator(oid, start, end), field_(field), keys_(keys), from_tids_(from_tids), from_oids_(from_oids), left_bound_(left_bound), right_bound_(right_bound) {}

OperatorType Predicate::type() const { return OperatorType::PREDICATE; }

const unordered_set<uint64_t> &Predicate::keys() const { return keys_; }
const unordered_set<uint32_t> &Predicate::from_tids() const { return from_tids_; }
const unordered_set<uint32_t> &Predicate::from_oids() const { return from_oids_; }

bool Predicate::relevant(Write *write) const
{
    return write->updates().count(field_) > 0;
}

bool Predicate::match(Write *write) const
{
    return relevant(write) && write->updates()[field_] >= left_bound_ && write->updates()[field_] <= right_bound_;
}

bool Predicate::cover(uint64_t key) const
{
    return keys_.count(key) > 0;
}

string Predicate::to_string() const
{
    ostringstream oss;
    oss << "Predicate(oid=" << oid_ << ", start=" << start_ << ", end=" << end_ << ", field=" << field_ << ", left=" << left_bound_ << ", right=" << right_bound_ << ")";
    return oss.str();
}

Transaction::Transaction(uint32_t tid, uint64_t start, uint64_t end)
    : tid_(tid), start_(start), end_(end) {}

void Transaction::append_operator(unique_ptr<Operator> operateor) { operators_.emplace_back(move(operateor)); }
const vector<unique_ptr<Operator>> &Transaction::operators() const { return operators_; }

uint32_t Transaction::tid() const { return tid_; }
uint64_t Transaction::start() const { return start_; }
uint64_t Transaction::end() const { return end_; }

void Transaction::set_start(uint64_t start) { start_ = start; }
void Transaction::set_end(uint64_t end) { end_ = end; }

string Transaction::to_string() const
{
    ostringstream oss;
    oss << "Transaction(tid=" << tid_ << ", start=" << start_ << ", end=" << end_ << ", operators=[\n";
    for (const auto &op : operators_)
    {
        oss << "  " << op->to_string() << "\n";
    }
    oss << "])";
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

TransactionManager::TransactionManager() {}

const vector<unique_ptr<Transaction>> &TransactionManager::transactions() const { return trxs_; }

unique_ptr<Transaction> TransactionManager::init_transaction() { return make_unique<Transaction>(0, 0, 0); }

bool TransactionManager::load(const string &root)
{
    vector<vector<unique_ptr<Transaction>>> sessions;

    DIR *dir = opendir(root.c_str());
    if (!dir)
    {
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        string filename = entry->d_name;
        if (filename == "." || filename == "..")
            continue;

        string full_path = root + "/" + filename;
        struct stat file_info;
        if (stat(full_path.c_str(), &file_info) != 0 || !S_ISREG(file_info.st_mode) || filename.substr(filename.find_last_of('.')) != ".log")
            continue;

        ifstream file(full_path, ios::binary);
        if (!file)
        {
            continue;
        }

        vector<unique_ptr<Transaction>> session;
        Transaction *trx = nullptr;

        char opType;
        unordered_set<uint64_t> keys;
        unordered_set<uint32_t> from_oids, from_tids;

        while (file.read(&opType, 1))
        {
            uint64_t start_time, end_time, key, read_field, write_filed;
            uint32_t tid, oid, from_tid, from_oid, left, right, value, size;

            switch (opType)
            {
            case 'T':
                file.read(reinterpret_cast<char *>(&tid), sizeof(tid));
                file.read(reinterpret_cast<char *>(&start_time), sizeof(start_time));
                file.read(reinterpret_cast<char *>(&end_time), sizeof(end_time));
                session.emplace_back(make_unique<Transaction>(tid, start_time, end_time));
                trx = session.back().get();
                break;
            case 'S':
                file.read(reinterpret_cast<char *>(&oid), sizeof(oid));
                file.read(reinterpret_cast<char *>(&start_time), sizeof(start_time));
                file.read(reinterpret_cast<char *>(&end_time), sizeof(end_time));
                trx->append_operator(make_unique<Begin>(oid, start_time, end_time));
                break;
            case 'C':
                file.read(reinterpret_cast<char *>(&oid), sizeof(oid));
                file.read(reinterpret_cast<char *>(&start_time), sizeof(start_time));
                file.read(reinterpret_cast<char *>(&end_time), sizeof(end_time));
                trx->append_operator(make_unique<Commit>(oid, start_time, end_time));
                if (trx->operators().size() > 1)
                {
                    trx->set_start(trx->operators()[1]->start());
                }
                break;
            case 'A':
                file.read(reinterpret_cast<char *>(&oid), sizeof(oid));
                file.read(reinterpret_cast<char *>(&start_time), sizeof(start_time));
                file.read(reinterpret_cast<char *>(&end_time), sizeof(end_time));
                trx->append_operator(make_unique<Abort>(oid, start_time, end_time));
                if (trx->operators().size() > 1)
                {
                    trx->set_start(trx->operators()[1]->start());
                }
                session.pop_back();
                break;
            case 'W':
                file.read(reinterpret_cast<char *>(&oid), sizeof(oid));
                file.read(reinterpret_cast<char *>(&start_time), sizeof(start_time));
                file.read(reinterpret_cast<char *>(&end_time), sizeof(end_time));
                file.read(reinterpret_cast<char *>(&key), sizeof(key));
                file.read(reinterpret_cast<char *>(&write_filed), sizeof(write_filed));
                file.read(reinterpret_cast<char *>(&value), sizeof(value));
                trx->append_operator(make_unique<Write>(oid, start_time, end_time, key, write_filed, value));
                break;
            case 'R':
                file.read(reinterpret_cast<char *>(&oid), sizeof(oid));
                file.read(reinterpret_cast<char *>(&start_time), sizeof(start_time));
                file.read(reinterpret_cast<char *>(&end_time), sizeof(end_time));
                file.read(reinterpret_cast<char *>(&key), sizeof(key));
                file.read(reinterpret_cast<char *>(&from_tid), sizeof(from_tid));
                file.read(reinterpret_cast<char *>(&from_oid), sizeof(from_oid));
                trx->append_operator(make_unique<Read>(oid, start_time, end_time, key, from_tid, from_oid));
                break;
            case 'P':
                file.read(reinterpret_cast<char *>(&oid), sizeof(oid));
                file.read(reinterpret_cast<char *>(&start_time), sizeof(start_time));
                file.read(reinterpret_cast<char *>(&end_time), sizeof(end_time));
                file.read(reinterpret_cast<char *>(&read_field), sizeof(read_field));
                file.read(reinterpret_cast<char *>(&left), sizeof(left));
                file.read(reinterpret_cast<char *>(&right), sizeof(right));
                file.read(reinterpret_cast<char *>(&size), sizeof(size));

                keys.clear();
                from_tids.clear();
                from_oids.clear();

                keys.reserve(size);
                from_tids.reserve(size);
                from_oids.reserve(size);

                for (size_t i = 0; i < size; ++i)
                {
                    file.read(reinterpret_cast<char *>(&key), sizeof(key));
                    keys.insert(key);
                }
                for (size_t i = 0; i < size; ++i)
                {
                    file.read(reinterpret_cast<char *>(&from_tid), sizeof(from_tid));
                    from_tids.insert(from_tid);
                }
                for (size_t i = 0; i < size; ++i)
                {
                    file.read(reinterpret_cast<char *>(&from_oid), sizeof(from_oid));
                    from_oids.insert(from_oid);
                }
                trx->append_operator(make_unique<Predicate>(oid, start_time, end_time, read_field, left, right, keys, from_tids, from_oids));
                break;
            default:
                throw runtime_error("LOG ERROR: Unknown operation type");
            }
        }
        sessions.emplace_back(move(session));
    }
    closedir(dir);
    trxs_.clear();
    trxs_.emplace_back(move(init_transaction()));
    priority_queue<pair<unique_ptr<Transaction>, int>, vector<pair<Transaction *, int>>, CompareTransactions> minHeap;
    vector<size_t> indices(sessions.size(), 0);

    for (size_t i = 0; i < sessions.size(); ++i)
    {
        if (!sessions[i].empty())
        {
            minHeap.emplace(sessions[i][0].get(), i);
        }
    }
    while (!minHeap.empty())
    {
        int session_index = minHeap.top().second;
        minHeap.pop();
        trxs_.emplace_back(move(sessions[session_index][indices[session_index]]));
        if (++indices[session_index] < sessions[session_index].size())
        {
            minHeap.emplace(sessions[session_index][indices[session_index]].get(), session_index);
        }
    }
    return true;
}