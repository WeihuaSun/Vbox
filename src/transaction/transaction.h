#ifndef TRANSACTION_H
#define TRANSACTION_H

#define INIT_TID 0
#define INIT_OID 0

#include <cassert>
#include <vector>
#include <memory>
#include <optional>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

enum class OperatorType
{
    READ,
    WRITE,
    BEGIN,
    COMMIT,
    ABORT,
    PREDICATE,
};

class Operator
{
public:
    Operator(uint32_t oid, uint64_t start, uint64_t end);
    virtual ~Operator() = default;
    virtual OperatorType type() const = 0;

    uint32_t oid() const;
    uint64_t start() const;
    uint64_t end() const;

    virtual std::string to_string() const = 0;

protected:
    uint32_t oid_;
    uint64_t start_;
    uint64_t end_;
};

class Read : public Operator
{
public:
    Read(uint32_t oid, uint64_t start, uint64_t end, uint64_t key, uint32_t from_tid, uint32_t from_oid);
    OperatorType type() const override;

    uint64_t key() const;
    uint32_t from_tid() const;
    uint32_t from_oid() const;

    std::string to_string() const override;

private:
    uint64_t key_;
    uint32_t from_tid_;
    uint32_t from_oid_;
};

class Write : public Operator
{
public:
    Write(uint32_t oid, uint64_t start, uint64_t end, uint64_t key, uint64_t field, uint32_t value);
    OperatorType type() const override;

    uint64_t key() const;
    std::unordered_map<uint64_t, u_int32_t> updates();

    void combine(Write *other);
    std::string to_string() const override;

private:
    uint64_t key_;
    std::unordered_set<uint64_t> fields_;
    std::unordered_set<uint32_t> values_;
    std::unordered_map<uint64_t, u_int32_t> updates_; // field->value
};

class Begin : public Operator
{
public:
    Begin(uint32_t oid, uint64_t start, uint64_t end);
    OperatorType type() const override;

    std::string to_string() const override;
};

class Commit : public Operator
{
public:
    Commit(uint32_t oid, uint64_t start, uint64_t end);
    OperatorType type() const override;

    std::string to_string() const override;
};

class Abort : public Operator
{
public:
    Abort(uint32_t oid, uint64_t start, uint64_t end);
    OperatorType type() const override;

    std::string to_string() const override;
};

class Predicate : public Operator
{
public:
    Predicate(uint32_t oid, uint64_t start, uint64_t end, uint64_t field, uint32_t left_bound, uint32_t right_bound,
              std::unordered_set<uint64_t> keys, std::unordered_set<uint32_t> from_tids, std::unordered_set<uint32_t> from_oids);

    OperatorType type() const override;
    const std::unordered_set<uint64_t> &keys() const;
    const std::unordered_set<uint32_t> &from_tids() const;
    const std::unordered_set<uint32_t> &from_oids() const;

    bool relevant(Write *write) const;
    bool match(Write *write) const;
    bool cover(uint64_t key) const;
    std::string to_string() const override;

private:
    uint64_t field_;
    std::unordered_set<uint64_t> keys_;
    std::unordered_set<uint32_t> from_tids_;
    std::unordered_set<uint32_t> from_oids_;
    uint32_t left_bound_;
    uint32_t right_bound_;
};

class Transaction
{
public:
    Transaction(uint32_t tid, uint64_t start, uint64_t end);
    ~Transaction() = default;

    void append_operator(std::unique_ptr<Operator> operateor);
    const std::vector<std::unique_ptr<Operator>> &operators() const;

    uint32_t tid() const;
    uint64_t start() const;
    uint64_t end() const;

    void set_start(uint64_t start);
    void set_end(uint64_t end);

    std::string to_string() const;

private:
    static std::unique_ptr<Transaction> init_trx_;

    uint32_t tid_;
    uint64_t start_;
    uint64_t end_;
    std::vector<std::unique_ptr<Operator>> operators_;
};

struct CompareTransactions
{
    bool operator()(const std::pair<Transaction *, int> &a, const std::pair<Transaction *, int> &b) const
    {
        return a.first->start() > b.first->start();
    }
};

class TransactionManager
{
public:
    TransactionManager();
    ~TransactionManager() = default;
    const std::vector<std::unique_ptr<Transaction>> &transactions() const;
    std::unique_ptr<Transaction> init_transaction();
    bool load(const std::string &root);

private:
    std::vector<std::unique_ptr<Transaction>> trxs_;
    std::unique_ptr<Transaction> init_trx_;
};

#endif // TRANSACTION_H
