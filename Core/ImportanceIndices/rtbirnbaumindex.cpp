#include "rtbirnbaumindex.h"

RTBirnbaumIndex::RTBirnbaumIndex(Component &paComponent, ComponentMeasures::ComputeBaseIndex paComputeBaseIndex) : ComponentIndex(paComponent)
{
    compute = paComputeBaseIndex;
}

d_IMPORTANCEINDEX RTBirnbaumIndex::computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    Component *pomComponent = getComponent();
    d_IMPORTANCEINDEX pomResult = compute(*pomComponent, paComponentState, paSystemState, paError);

    if (paError)
    {
        paError.prependText("RTBirnbaumIndex::computeBaseIndex: ");
    }

    return pomResult;
}
