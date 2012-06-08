#include "dplrstructuralmodifiedindex.h"

DPLRStructuralModifiedIndex::DPLRStructuralModifiedIndex(Component &paComponent) : ComponentIndex(paComponent)
{
}

d_IMPORTANCEINDEX DPLRStructuralModifiedIndex::computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    Component *pomComponent = getComponent();
    d_IMPORTANCEINDEX pomResult = ComponentMeasures::computeBaseDPLRStructuralModifiedIndex(*pomComponent, paComponentState, paSystemState, paError);

    if (paError)
    {
        paError.prependText("DPLRStructuralModifiedIndex::computeBaseIndex: ");
    }

    return pomResult;
}
