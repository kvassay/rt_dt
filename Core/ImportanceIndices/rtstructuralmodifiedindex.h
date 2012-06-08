#ifndef RTSTRUCTURALMODIFIEDINDEX_H
#define RTSTRUCTURALMODIFIEDINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class RTStructuralModifiedIndex : public ComponentIndex
{
private:
    ComponentMeasures::ComputeBaseIndex compute;

protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

public:
    RTStructuralModifiedIndex(Component &paComponent, ComponentMeasures::ComputeBaseIndex paComputeBaseIndex);
    virtual ~RTStructuralModifiedIndex() { }
};

#endif // RTSTRUCTURALMODIFIEDINDEX_H
