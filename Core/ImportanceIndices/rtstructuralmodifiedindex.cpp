#include "rtstructuralmodifiedindex.h"

RTStructuralModifiedIndex::RTStructuralModifiedIndex(Component &paComponent, ComponentMeasures::ComputeBaseIndex paComputeBaseIndex) : ComponentIndex(paComponent)
{
    compute = paComputeBaseIndex;
}

d_IMPORTANCEINDEX RTStructuralModifiedIndex::computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    Component *pomComponent = getComponent();
    d_IMPORTANCEINDEX pomResult = compute(*pomComponent, paComponentState, paSystemState, paError);

    if (paError)
    {
        paError.prependText("RTStructuralModifiedIndex::computeBaseIndex: ");
    }

    return pomResult;
}
