#ifndef ISOLATION_EXCEPTIONS_H
#define ISOLATION_EXCEPTIONS_H

#include <exception>
#include <string>
#include <sstream>
#include "graph/graph.h"
class ISException : public std::exception
{
public:
    explicit ISException(const std::string &message) : message_(message) {}
    virtual const char *what() const noexcept override
    {
        return message_.c_str();
    }

private:
    std::string message_;
};

class AbortedReads : public ISException
{
public:
    explicit AbortedReads(Read *read)
        : ISException(buildMessage(read)) {}

private:
    static std::string buildMessage(Read *read)
    {
        std::ostringstream oss;
        oss << "Aborted Reads. \n Read Operator: " << read->to_string();
        return oss.str();
    }
};

class ReadSelfUpdateFailed : public ISException
{
public:
    explicit ReadSelfUpdateFailed(Transaction *txn)
        : ISException(buildMessage(txn)) {}

private:
    static std::string buildMessage(Transaction *txn)
    {
        std::ostringstream oss;
        oss << "Read Self-Update Failed. \n Transaction: " << txn->to_string();
        return oss.str();
    }
};

class IntermediateReads : public ISException
{
public:
    explicit IntermediateReads(Transaction *txn, Read *read)
        : ISException(buildMessage(txn, read)) {}

private:
    static std::string buildMessage(Transaction *txn, Read *read)
    {
        std::ostringstream oss;
        oss << "Intermediate Reads. \n Read Operator: " << read->to_string() << "\n Read From Transaction:" << txn->to_string();
        return oss.str();
    }
};

class InconsistentReads : public ISException
{
public:
    explicit InconsistentReads(Read *read)
        : ISException(buildMessage(read)) {}

private:
    static std::string buildMessage(Read *read)
    {
        std::ostringstream oss;
        oss << "Inconsistent Reads. \n Read Operator: " << read->to_string();
        return oss.str();
    }
};

class ConcurrentWrite : public ISException
{
public:
    explicit ConcurrentWrite(Vertex s, Vertex t)
        : ISException(buildMessage(s, t)) {}

private:
    static std::string buildMessage(Vertex s, Vertex t)
    {
        std::ostringstream oss;
        oss << "Concurrent Write. \n Transaction A: " << s.to_string() << "\n Transaction B:" << t.to_string();
        return oss.str();
    }
};

class TOCertifierFailed : public ISException
{
public:
    explicit TOCertifierFailed(Vertex s, Vertex t)
        : ISException(buildMessage(s, t)) {}

private:
    static std::string buildMessage(Vertex s, Vertex t)
    {
        std::ostringstream oss;
        oss << "TO Certifier Failed. \n Transaction A: " << s.to_string() << "\n Transaction B:" << t.to_string();
        return oss.str();
    }
};

class SSICertifierFailed : public ISException
{
public:
    explicit SSICertifierFailed(Vertex s, Vertex t, Vertex u)
        : ISException(buildMessage(s, t, u)) {}

private:
    static std::string buildMessage(Vertex s, Vertex t, Vertex u)
    {
        std::ostringstream oss;
        oss << "SSI Certifier Failed. \n Transaction A: " << s.to_string() << "\n Transaction B:" << t.to_string() << "\n Transaction C:" << u.to_string();
        return oss.str();
    }
};

class SerializableException : public ISException
{
public:
    explicit SerializableException(const std::string &message)
        : ISException(message) {}
};

class IsoException3 : public ISException
{
public:
    explicit IsoException3(const std::string &message) : ISException(message) {}
};

#endif // ISOLATION_EXCEPTIONS_H
