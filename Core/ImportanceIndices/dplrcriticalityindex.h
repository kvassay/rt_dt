#ifndef DPLRCRITICALITYINDEX_H
#define DPLRCRITICALITYINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class DPLRCriticalityIndex : public ComponentIndex
{
protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

public:
    DPLRCriticalityIndex(Component &paComponent);
    virtual ~DPLRCriticalityIndex() { }
};

#endif // DPLRCRITICALITYINDEX_H
