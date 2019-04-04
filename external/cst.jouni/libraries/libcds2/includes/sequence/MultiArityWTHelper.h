//
// Created by alberto on 4/14/15.
//

#ifndef ALL_MULTIARITYWTHELPER_H
#define ALL_MULTIARITYWTHELPER_H
#include <sequence/MultiArityWT.h>

namespace  cds_static {

    class MultiArityWT;
    class MultiArityWTHelper {
    public:
        MultiArityWTHelper();
        uint getDepth(const MultiArityWT &wt);
        const Sequence *getSequence(const MultiArityWT &wt, const vector<uint> &node_id);
        const vector<MultiArityNode *>* getChildren(const MultiArityWT &wt, const vector<uint> &node_id);
    protected:
        uint getDepthRec(MultiArityNode *node);
    };
};


#endif //ALL_MULTIARITYWTHELPER_H
