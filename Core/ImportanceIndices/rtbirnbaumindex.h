#ifndef RTBIRNBAUMINDEX_H
#define RTBIRNBAUMINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class RTBirnbaumIndex : public ComponentIndex
{
private:
    ComponentMeasures::ComputeBaseIndex compute;

protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

public:
    RTBirnbaumIndex(Component &paComponent, ComponentMeasures::ComputeBaseIndex paComputeBaseIndex);
    virtual ~RTBirnbaumIndex() { }
};

#endif // RTBIRNBAUMINDEX_H
