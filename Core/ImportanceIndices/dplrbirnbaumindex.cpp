#include "dplrbirnbaumindex.h"

DPLRBirnbaumIndex::DPLRBirnbaumIndex(Component &paComponent) : ComponentIndex(paComponent)
{

}

d_IMPORTANCEINDEX DPLRBirnbaumIndex::computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    Component *pomComponent = getComponent();
    d_IMPORTANCEINDEX pomResult = ComponentMeasures::computeBaseDPLRBirnbaumIndex(*pomComponent, paComponentState, paSystemState, paError);

    if (paError)
    {
        paError.prependText("DPLRBirnbaumIndex::computeBaseIndex: ");
    }

    return pomResult;
}
