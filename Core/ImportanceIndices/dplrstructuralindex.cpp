#include "dplrstructuralindex.h"

DPLRStructuralIndex::DPLRStructuralIndex(Component &paComponent) : ComponentIndex(paComponent)
{

}

d_IMPORTANCEINDEX DPLRStructuralIndex::computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    Component *pomComponent = getComponent();
    d_IMPORTANCEINDEX pomResult = ComponentMeasures::computeBaseDPLRStructuralIndex(*pomComponent, paComponentState, paSystemState, paError);

    if (paError)
    {
        paError.prependText("DPLRStructuralIndex::computeBaseIndex: ");
    }

    return pomResult;
}
