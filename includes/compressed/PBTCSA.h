//
// Created by elarielcl on 25/09/18.
//

#ifndef BLOCKTREE_PBTCSA_H
#define BLOCKTREE_PBTCSA_H

#include <compressed/DAPCBlockTree.h>




class PBTCSA {
public:

    static const int PAPER = 0;
    static const int PAPER_NO_CLEAN = 1;
    static const int PRUNIG_C = 2;
    static const int HEURISTIC = 3;
    static const int LIBERAL = 4;
    static const int CONSERVATIVE = 5;
    DAPCBlockTree* dsa_;

    PBTCSA(std::string&, int = PAPER, int = 2, int = 4, int = 2);
    ~PBTCSA();


    int access(int);
    int saccess(int);
    int caccess(int);
    int csaccess(int);
    int mapped_access(int);
    int mapped_saccess(int);

    int mapped_caccess(int);
    int mapped_csaccess(int);
    void print_statistics();
};

#endif //BLOCKTREE_PBTCSA_H
