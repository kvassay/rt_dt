#ifndef DPLRSTRUCTURALINDEX_H
#define DPLRSTRUCTURALINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class DPLRStructuralIndex : public ComponentIndex
{
protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

public:
    DPLRStructuralIndex(Component &paComponent);
    virtual ~DPLRStructuralIndex() { }
};

#endif // DPLRSTRUCTURALINDEX_H
