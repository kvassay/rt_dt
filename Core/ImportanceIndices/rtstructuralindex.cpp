#include "rtstructuralindex.h"

RTStructuralIndex::RTStructuralIndex(Component &paComponent, ComponentMeasures::ComputeBaseIndex paComputeBaseIndex) : ComponentIndex(paComponent)
{
    compute = paComputeBaseIndex;
}

d_IMPORTANCEINDEX RTStructuralIndex::computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    Component *pomComponent = getComponent();
    d_IMPORTANCEINDEX pomResult = compute(*pomComponent, paComponentState, paSystemState, paError);

    if (paError)
    {
        paError.prependText("RTStructuralIndex::computeBaseIndex: ");
    }

    return pomResult;
}
