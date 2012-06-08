#ifndef DPLRBIRNBAUMINDEX_H
#define DPLRBIRNBAUMINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class DPLRBirnbaumIndex : public ComponentIndex
{
protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

public:
    DPLRBirnbaumIndex(Component &paComponent);
    virtual ~DPLRBirnbaumIndex() { }
};

#endif // DPLRBIRNBAUMINDEX_H
