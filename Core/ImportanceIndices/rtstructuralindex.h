#ifndef RTSTRUCTURALINDEX_H
#define RTSTRUCTURALINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class RTStructuralIndex : public ComponentIndex
{
private:
    ComponentMeasures::ComputeBaseIndex compute;

protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

public:
    RTStructuralIndex(Component &paComponent, ComponentMeasures::ComputeBaseIndex paComputeBaseIndex);
    virtual ~RTStructuralIndex() { }
};

#endif // RTSTRUCTURALINDEX_H
