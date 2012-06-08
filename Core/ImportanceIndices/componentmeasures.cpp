#include "componentmeasures.h"

#include <math.h>

#include "Core/System/component.h"
#include "Core/System/system.h"

#include "Core/ImportanceIndices/componentindex.h"

#include "Core/ImportanceIndices/rtstructuralindex.h"
#include "Core/ImportanceIndices/rtstructuralmodifiedindex.h"
#include "Core/ImportanceIndices/rtbirnbaumindex.h"
#include "Core/ImportanceIndices/rtcriticalityindex.h"

#include "Core/ImportanceIndices/dplrstructuralindex.h"
#include "Core/ImportanceIndices/dplrstructuralmodifiedindex.h"
#include "Core/ImportanceIndices/dplrbirnbaumindex.h"
#include "Core/ImportanceIndices/dplrcriticalityindex.h"

#include "Core/ImportanceIndices/rcbirnbaumindex.h"

ComponentMeasures::CreateComponentIndexFunctionsMap ComponentMeasures::createComponentIndexMap;
ComponentMeasures::TypesVector ComponentMeasures::types;

void ComponentMeasures::init()
{
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTStructural_v1, createRTStructuralIndex_v1));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTStructuralModified_v1, createRTStructuralModifiedIndex_v1));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTBirnbaum_v1, createRTBirnbaumIndex_v1));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTBirnbaum_v2, createRTBirnbaumIndex_v2));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTCriticality_v1, createRTCriticalityIndex_v1));

    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTStructural_vF, createRTStructuralIndex_vF));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTStructuralModified_vF, createRTStructuralModifiedIndex_vF));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTBirnbaum_vF, createRTBirnbaumIndex_vF));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RTCriticality_vF, createRTCriticalityIndex_vF));

    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(DPLRStructural, createDPLRStructuralIndex));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(DPLRStructuralModified, createDPLRStructuralModifiedIndex));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(DPLRBirnbaum, createDPLRBirnbaumIndex));
    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(DPLRCriticality, createDPLRCriticalityIndex));

    createComponentIndexMap.insert(CreateComponentIndexFunctionsMap::value_type(RCBirnbaum, createRCBirnbaumIndex));

    for (CreateComponentIndexFunctionsMap::iterator it = createComponentIndexMap.begin(); it != createComponentIndexMap.end(); it++)
    {
        types.push_back(it->first);
    }
}

ComponentIndex * ComponentMeasures::createComponentIndex(ComponentMeasures::Type paType, Component &paComponent)
{
    CreateComponentIndexFunctionsMap::iterator pomIt = createComponentIndexMap.find(paType);
    CreateComponentIndexFunction pomFun = pomIt->second;
    return pomFun(paComponent);
}

ComponentIndex * ComponentMeasures::createRTStructuralIndex_v1(Component &paComponent)
{
    return new RTStructuralIndex(paComponent, computeBaseRTStructuralIndex_v1);
}

ComponentIndex * ComponentMeasures::createRTStructuralModifiedIndex_v1(Component &paComponent)
{
    return new RTStructuralModifiedIndex(paComponent, computeBaseRTStructuralModifiedIndex_v1);
}

ComponentIndex * ComponentMeasures::createRTBirnbaumIndex_v1(Component &paComponent)
{
    return new RTBirnbaumIndex(paComponent, computeBaseRTBirnbaumIndex_v1);
}

ComponentIndex * ComponentMeasures::createRTBirnbaumIndex_v2(Component &paComponent)
{
    return new RTBirnbaumIndex(paComponent, computeBaseRTBirnbaumIndex_v2);
}

ComponentIndex * ComponentMeasures::createRTCriticalityIndex_v1(Component &paComponent)
{
    return new RTCriticalityIndex(paComponent, computeBaseRTCriticalityIndex_v1);
}


ComponentIndex * ComponentMeasures::createRTStructuralIndex_vF(Component &paComponent)
{
    return new RTStructuralIndex(paComponent, computeBaseRTStructuralIndex_vF);
}

ComponentIndex * ComponentMeasures::createRTStructuralModifiedIndex_vF(Component &paComponent)
{
    return new RTStructuralModifiedIndex(paComponent, computeBaseRTStructuralModifiedIndex_vF);
}

ComponentIndex * ComponentMeasures::createRTBirnbaumIndex_vF(Component &paComponent)
{
    return new RTBirnbaumIndex(paComponent, computeBaseRTBirnbaumIndex_vF);
}

ComponentIndex * ComponentMeasures::createRTCriticalityIndex_vF(Component &paComponent)
{
    return new RTCriticalityIndex(paComponent, computeBaseRTCriticalityIndex_vF);
}



ComponentIndex * ComponentMeasures::createDPLRStructuralIndex(Component &paComponent)
{
    return new DPLRStructuralIndex(paComponent);
}

ComponentIndex * ComponentMeasures::createDPLRStructuralModifiedIndex(Component &paComponent)
{
    return new DPLRStructuralModifiedIndex(paComponent);
}

ComponentIndex * ComponentMeasures::createDPLRBirnbaumIndex(Component &paComponent)
{
    return new DPLRBirnbaumIndex(paComponent);
}

ComponentIndex * ComponentMeasures::createDPLRCriticalityIndex(Component &paComponent)
{
    return new DPLRCriticalityIndex(paComponent);
}


ComponentIndex * ComponentMeasures::createRCBirnbaumIndex(Component &paComponent)
{
    return new RCBirnbaumIndex(paComponent);
}


d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTStructuralIndex_v1(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    d_SYSTEMCONFIGURATIONSCOUNT pomGood = paComponent.getConditionalSystemConfigurationsCount(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTStructuralIndex_v1: ");
        return 0;
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomBad = paComponent.getConditionalSystemConfigurationsCount(paComponentState-1, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTStructuralIndex_v1: ");
        return 0;
    }

    return (d_IMPORTANCEINDEX) (pomGood - pomBad) / (d_IMPORTANCEINDEX) paComponent.getConditionalSystemConfigurationsCount();
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTStructuralModifiedIndex_v1(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    d_SYSTEMCONFIGURATIONSCOUNT pomGood = paComponent.getConditionalSystemConfigurationsCount(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTStructuralModifiedIndex_v1: ");
        return 0;
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomBad = paComponent.getConditionalSystemConfigurationsCount(paComponentState-1, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTStructuralModifiedIndex_v1: ");
        return 0;
    }

    if (pomGood != pomBad)
        return (d_IMPORTANCEINDEX) (pomGood - pomBad) / (d_IMPORTANCEINDEX) pomGood;
    else
        return 0;
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTBirnbaumIndex_v1(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    d_IMPORTANCEINDEX pomValue = computeBaseRTBirnbaumIndex_v2(paComponent, paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTBirnbaumIndex_v1: ");
        return 0;
    }

    return fabs(pomValue);
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTBirnbaumIndex_v2(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    d_PROBABILITY pomPrevious = paComponent.getConditionalSystemReliability(paComponentState - 1, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTBirnbaumIndex_v2: ");
        return 0;
    }

    d_PROBABILITY pomCurrent = paComponent.getConditionalSystemReliability(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTBirnbaumIndex_v2: ");
        return 0;
    }

    return pomCurrent - pomPrevious;
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTCriticalityIndex_v1(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();

    d_PROBABILITY pomSystemFailedProbability = paComponent.getSystem()->getStateProbability(0, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTCriticalityIndex_v1: ");
        return 0;
    }

    d_PROBABILITY pomComponentFailedProbability = paComponent.getProbability(paComponentState-1, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTCriticalityIndex_v1: ");
        return 0;
    }

    d_IMPORTANCEINDEX pomBaseBirnbaumIndex = computeBaseRTBirnbaumIndex_vF(paComponent, paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTCriticalityIndex_v1: ");
        return 0;
    }

    if (pomSystemFailedProbability > 0)
        return pomBaseBirnbaumIndex * pomComponentFailedProbability / pomSystemFailedProbability;
    else
        return 0;
}


d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTStructuralIndex_vF(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    d_SYSTEMCONFIGURATIONSCOUNT pomCPVCount = paComponent.getCriticalPathVectorsCount(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTStructuralIndex_vF: ");
        return 0;
    }
    return (d_IMPORTANCEINDEX) pomCPVCount / (d_IMPORTANCEINDEX) paComponent.getConditionalSystemConfigurationsCount();
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTStructuralModifiedIndex_vF(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    d_SYSTEMCONFIGURATIONSCOUNT pomCPVCount = paComponent.getCriticalPathVectorsCount(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTStructuralIndex_vF: ");
        return 0;
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomGood = paComponent.getConditionalSystemConfigurationsCount(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTStructuralModifiedIndex_vF: ");
        return 0;
    }

    if (pomCPVCount > 0)
        return (d_IMPORTANCEINDEX) pomCPVCount / (d_IMPORTANCEINDEX) pomGood;
    else
        return 0;
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTBirnbaumIndex_vF(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    d_PROBABILITY pomCPVProbability = paComponent.getCriticalPathVectorsProbability(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTBirnbaumIndex_vF: ");
        return 0;
    }

    return pomCPVProbability;
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseRTCriticalityIndex_vF(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();

    d_PROBABILITY pomSystemFailedProbability = 0;
    for (d_STATESCOUNT i = 0; i < paSystemState; i++)
    {
        pomSystemFailedProbability += paComponent.getSystem()->getStateProbability(i, paError);
        if (paError)
        {
            paError.prependText("ComponentMeasures::computeBaseRTCriticalityIndex_vF: ");
            return 0;
        }
    }

    d_PROBABILITY pomComponentFailedProbability = paComponent.getProbability(paComponentState - 1, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTCriticalityIndex_vF: ");
        return 0;
    }

    d_IMPORTANCEINDEX pomBaseBirnbaumIndex = computeBaseRTBirnbaumIndex_vF(paComponent, paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRTCriticalityIndex_vF: ");
        return 0;
    }

    if (pomSystemFailedProbability > 0)
        return pomBaseBirnbaumIndex * pomComponentFailedProbability / pomSystemFailedProbability;
    else
        return 0;
}



d_IMPORTANCEINDEX ComponentMeasures::computeBaseDPLRStructuralIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    DPLR *pomDPLR = paComponent.getDPLR(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseDPLRStructuralIndex: ");
        return 0;
    }

    return (d_IMPORTANCEINDEX) pomDPLR->getPositiveDerivationsCount() / (d_IMPORTANCEINDEX) pomDPLR->getSize();
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseDPLRStructuralModifiedIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    DPLR *pomDPLR = paComponent.getDPLR(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseDPLRStructuralIndex: ");
        return 0;
    }

    if (pomDPLR->getGoodSystemConfigurationsCount() != 0)
        return (d_IMPORTANCEINDEX) pomDPLR->getPositiveDerivationsCount() / (d_IMPORTANCEINDEX) pomDPLR->getGoodSystemConfigurationsCount();
    else
        return 0;
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseDPLRBirnbaumIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();

    DPLR *pomDPLR = paComponent.getDPLR(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseDPLRBirnbaumIndex: ");
        return 0;
    }

    return (d_IMPORTANCEINDEX) pomDPLR->getPositiveDerivationsProbability();
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseDPLRCriticalityIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();

    d_PROBABILITY pomSystemFailedProbability = 0;
    for (d_STATESCOUNT i = 0; i < paSystemState; i++)
    {
        pomSystemFailedProbability += paComponent.getSystem()->getStateProbability(i, paError);
        if (paError)
        {
            paError.prependText("ComponentMeasures::computeBaseDPLRCriticalityIndex: ");
            return 0;
        }
    }

    d_PROBABILITY pomComponentFailedProbability = paComponent.getProbability(paComponentState-1, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseDPLRCriticalityIndex: ");
        return 0;
    }

    d_IMPORTANCEINDEX pomBaseBirnbaumIndex = computeBaseDPLRBirnbaumIndex(paComponent, paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseDPLRCriticalityIndex: ");
        return 0;
    }

    if (pomSystemFailedProbability > 0)
        return pomBaseBirnbaumIndex * pomComponentFailedProbability / pomSystemFailedProbability;
    else
        return 0;
}

d_IMPORTANCEINDEX ComponentMeasures::computeBaseRCBirnbaumIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    d_PROBABILITY pomSystemStateProbability = paComponent.getSystem()->getStateReliability(paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRCBirnbaumIndex: ");
        return 0;
    }

    d_PROBABILITY pomSystemConditionalProbability = paComponent.getConditionalSystemReliability(paComponentState, paSystemState, paError);
    if (paError)
    {
        paError.prependText("ComponentMeasures::computeBaseRCBirnbaumIndex: ");
        return 0;
    }

    return fabs(pomSystemConditionalProbability-pomSystemStateProbability);
}
