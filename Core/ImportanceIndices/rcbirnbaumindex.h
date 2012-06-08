#ifndef RCBIRNBAUMINDEX_H
#define RCBIRNBAUMINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class RCBirnbaumIndex : public ComponentIndex
{
protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    virtual void computeDataStructures(Level paLevel, ImportanceIndex &paIndex, ImportanceIndex &paIndexSum,
                                       ComponentIndex::IndicesVector &paComponentStatesIndicesSum, ComponentIndex::IndicesVector &paComponentStatesIndicesAvg,
                                       ComponentIndex::IndicesVector &paSystemStatesIndicesSum, ComponentIndex::IndicesVector &paSystemStatesIndicesAvg,
                                       ComponentIndex::IndicesMatrix &paIndicesMatrix, Error &paError);

public:
    RCBirnbaumIndex(Component &paComponent);
    virtual ~RCBirnbaumIndex() { }
};


#endif // RCBIRNBAUMINDEX_H
