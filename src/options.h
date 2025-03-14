#ifndef OPTIONS_H
#define OPTIONS_H

#include <iostream>
#include <string>

class VerifyOptions
{
public:
    std::string log = "";
    std::string verifier = "vbox";
    bool time = true;
    bool compact = true;
    bool merge = true;
    std::string prune = "prune_opt";
    std::string construct = "purdom+";
    std::string update = "italino+";
    std::string sat = "vboxsat";

    bool collect = false;

    int density = 10;

    void parse(int argc, char *argv[])
    {
        if (argc > 1)
            log = argv[1];
        if (argc > 2)
            verifier = argv[2];
        if (argc > 3)
            time = (std::string(argv[3]) == "true");
        if (argc > 4)
            compact = (std::string(argv[4]) == "true");
        if (argc > 5)
            merge = (std::string(argv[5]) == "true");
        if (argc > 6)
            prune = argv[6];
        if (argc > 7)
            construct = argv[7];
        if (argc > 8)
            update = argv[8];
        if (argc > 9)
            sat = argv[9];
        collect = (sat == "monosat" || sat == "minisat" || construct == "purdom" || construct == "italino"||construct == "italino+");
    }
    void print() const
    {
        if (verifier == "vbox")
        {
            std::cout << "====================== Verify Options ======================\n";
            std::cout << "Log Path:                 " << log << "\n";
            std::cout << "Verifier Type:            " << verifier << "\n";
            std::cout << "Timing Enabled:           " << (time ? "true" : "false") << "\n";
            std::cout << "Compaction Enabled:       " << (compact ? "true" : "false") << "\n";
            std::cout << "Merging Enabled:          " << (merge ? "true" : "false") << "\n";
            std::cout << "Pruning Strategy:         " << prune << "\n";
            std::cout << "TC Construction Strategy: " << construct << "\n";
            std::cout << "TC Update Strategy:       " << update << "\n";
            std::cout << "SAT Strategy:             " << sat << "\n";
            std::cout << "===========================================================\n";
        }
        else if (verifier == "leopard")
        {
            std::cout << "====================== Verify Options ======================\n";
            std::cout << "Log Path:                 " << log << "\n";
            std::cout << "Verifier Type:            " << verifier << "\n";
        }
        else
        {
            std::cerr << "Error: unknown verifier '" << verifier << "'." << std::endl;
        }
    }
};

#endif
