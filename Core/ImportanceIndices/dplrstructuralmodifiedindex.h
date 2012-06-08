#ifndef DPLRSTRUCTURALMODIFIEDINDEX_H
#define DPLRSTRUCTURALMODIFIEDINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class DPLRStructuralModifiedIndex : public ComponentIndex
{
protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

public:
    DPLRStructuralModifiedIndex(Component &paComponent);
    virtual ~DPLRStructuralModifiedIndex() { }
};

#endif // DPLRSTRUCTURALMODIFIEDINDEX_H
