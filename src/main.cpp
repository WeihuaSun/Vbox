
#include <chrono>
#include <sys/resource.h>

#include "options.h"
#include "verifier/vbox.h"
#include "verifier/leopard.h"

using namespace std;

long memory_usage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        cout << "====================== Usage Instructions ======================\n";
        cout << "Usage:\n";
        cout << "  " << argv[0] << " <log> <verifier> <time> <compact> <merge> <prune> <construct> <update> <sat>\n";
        cout << "\nParameters:\n";
        cout << "  <log>        Path to the directory containing the log files.\n";
        cout << "  <verifier>   Type of verifier to use. Options are: vbox / leopard\n";
        cout << "  <time>       Timing enabled. Options are: true / false\n";
        cout << "  <compact>    Compaction enabled. Options are: true / false\n";
        cout << "  <merge>      Merging enabled. Options are: true / false\n";
        cout << "  <prune>      Pruning strategy. Options are: prune_opt / prune / no\n";
        cout << "  <construct>  TC construction strategy. Options are: purdom+ / warshall / purdom / italino / italino+ \n";
        cout << "  <update>     TC update strategy. Options are: italino+ / warshall / italino\n";
        cout << "  <sat>        SAT strategy. Options are: vboxsat / monosat / minsat\n";
        cout << "\nExample:\n";
        cout << "  " << argv[0] << " /path/to/logs vbox true true true prune_opt purdom+ italino_opt vboxsat\n";
        cout << "===============================================================\n";
        return 0;
    }
    VerifyOptions options;
    options.parse(argc, argv);
    options.print();
    
    bool accept = false;
    auto start = chrono::high_resolution_clock::now();
    if (options.verifier == "vbox")
    {
        Vbox vbox(options);
        accept = vbox.run();
    }
    else if (options.verifier == "leopard")
    {
        Leopard leopard(options);
        accept = leopard.run();
    }
    else
    {
        return 1;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Verification Time: " << duration.count() << " seconds" << endl;
    cout << "Peak memory usage: " << memory_usage() << " KB" << endl;
    cout << "Accept: " << accept << endl;
    return 0;
}