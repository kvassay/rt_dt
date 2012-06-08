#ifndef RTCRITICALITYINDEX_H
#define RTCRITICALITYINDEX_H

#include "Core/ImportanceIndices/componentindex.h"

class RTCriticalityIndex : public ComponentIndex
{
private:
    ComponentMeasures::ComputeBaseIndex compute;

protected:
    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

public:
    RTCriticalityIndex(Component &paComponent, ComponentMeasures::ComputeBaseIndex paComputeBaseIndex);
    virtual ~RTCriticalityIndex() { }
};

#endif // RTCRITICALITYINDEX_H
