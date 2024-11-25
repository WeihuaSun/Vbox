#ifndef LEOPARD_H
#define LEOPARD_H

#include "transaction/transaction.h"
#include "graph/graph.h"
#include "options.h"

class Leopard
{

public:
    Leopard(const VerifyOptions &options);
    bool run();

private:
};

#endif