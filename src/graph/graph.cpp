#include "graph.h"

using namespace std;

uint32_t Vertex::index() const { return index_; }
void Vertex::set_index(uint32_t index) { index_ = index; }

uint32_t Vertex::left() const { return left_; }
void Vertex::set_left(uint32_t left) { left_ = (left_ == 0) ? left : left_; }

uint32_t Vertex::right() const { return right_; }
void Vertex::set_right(uint32_t right) { right_ = (right_ == UINT32_MAX) ? right : right_; }

uint64_t Vertex::start() const { return trx_->start(); }
uint64_t Vertex::end() const { return trx_->end(); }

const unordered_map<uint64_t, unordered_set<uint32_t>> &Vertex::reads() const { return reads_; }
void Vertex::set_read(uint64_t k, uint32_t v) { reads_[k].insert(v); }

const unordered_map<uint64_t, Write *> &Vertex::writes() const { return writes_; }
void Vertex::set_write(uint64_t k, Write *w)
{
    if (writes_.count(k) > 0)
    {
        writes_[k]->combine(w);
    }
    else{
        writes_[k] = w;
    }
}

void Vertex::set_predicate(Predicate *p) { predicates_.insert(p); }
const unordered_set<Predicate *> Vertex::predicates() const { return predicates_; }

Transaction *Vertex::transaction() const { return trx_; }

string Vertex::to_string() const
{
    ostringstream oss;
    oss << "Vertex (left=" << left_ << ", right=" << right_ << ", index=" << index_ << ", transaction=" << trx_->to_string() << ")";
    return oss.str();
}

///////////////////////////////////////////////////////////////////
Edge::Edge(): s_(0), t_(0) {}
Edge::Edge(uint32_t s, uint32_t t) : s_(s), t_(t) {}
uint32_t Edge::from() const { return s_; }
uint32_t Edge::to() const { return t_; }
