#include "dplrcriticalityindex.h"

DPLRCriticalityIndex::DPLRCriticalityIndex(Component &paComponent) : ComponentIndex(paComponent)
{

}

d_IMPORTANCEINDEX DPLRCriticalityIndex::computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    Component *pomComponent = getComponent();
    d_IMPORTANCEINDEX pomResult = ComponentMeasures::computeBaseDPLRCriticalityIndex(*pomComponent, paComponentState, paSystemState, paError);

    if (paError)
    {
        paError.prependText("DPLRCriticalityIndex::computeBaseIndex: ");
    }

    return pomResult;
}
