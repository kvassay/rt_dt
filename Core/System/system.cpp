#include "system.h"

#include <sstream>
#include <iostream>

#include "Core/ImportanceIndices/componentindex.h"

System::System(bool paEqualReliability)
{
    equalReliability = paEqualReliability;

    componentsCount = 0;
    structuralFunction = NULL;

    isInit = false;
}

System::~System()
{
    clear();
}

void System::clear()
{
    name.clear();

    if (structuralFunction != NULL)
    {
        delete structuralFunction;
        structuralFunction = NULL;
    }

    for (ComponentsMap::iterator it = componentsMap.begin(); it != componentsMap.end(); it++)
    {
        delete it->second;
        it->second = NULL;
    }
    componentsMap.clear();
    componentsVector.clear();
    componentsCount = 0;

    statesCountsVector.clear();
    multipleStatesCountsVector.clear();

    isInit = false;
}

void System::updateOtherStructures()
{
    componentsVector.resize(componentsCount);
    statesCountsVector.resize(componentsCount);
    multipleStatesCountsVector.resize(componentsCount);

    d_COMPONENTSCOUNT i = componentsCount - 1;

    multipleStatesCountsVector[i] = 1;
    for (ComponentsMap::reverse_iterator rit = componentsMap.rbegin(); rit != componentsMap.rend(); rit++)
    {
        componentsVector[i] = rit->second;

        statesCountsVector[i] = rit->second->getStatesCount();
        if (i < componentsCount - 1)
            multipleStatesCountsVector[i] = multipleStatesCountsVector[i+1] * statesCountsVector[i+1];
        i--;
    }
}

bool System::tryLoadFromTxtStream(const std::string &paSystemName, std::istream &paTxtStream, bool paIncludeProbabilities, Error &paError)
{
    std::stringstream pomErrorString;

    paError.reset();
    clear();

    name = paSystemName;
    if (paTxtStream.eof())
    {
        pomErrorString << "System::tryLoadFromTxtStream: Reach end of file before getting components count.";
        paError.set(e_REACH_EOF, &pomErrorString);
        clear();
        return false;
    }
    paTxtStream >> componentsCount;
    if (paTxtStream.fail())
    {
        pomErrorString << "System::tryLoadFromTxtStream: Error when getting components count.";
        paError.set(e_BAD_VALUE, &pomErrorString);
        clear();
        return false;
    }

    if (componentsCount <= 0 || componentsCount > c_MAX_COMPONENTS_COUNT)
    {
        if (componentsCount <= 0)
            pomErrorString << "System::tryLoadFromTxtStream: Components count = " << componentsCount << ".";
        else
            pomErrorString << "System::tryLoadFromTxtStream: Too much components (components count = " << componentsCount <<
                              " is more than " << c_MAX_COMPONENTS_COUNT << ").";
        paError.set(e_COMPONENTS_COUNT_IS_INVALID, &pomErrorString);
        clear();
        return false;
    }

    for (d_COMPONENTSCOUNT i = 0; i < componentsCount; i++)
    {
        Component *pomComponent = Component::createNew(i+1, paTxtStream, paIncludeProbabilities, *this, paError);
        if (paError)
        {
            paError.prependText("System::tryLoadFromTxtStream: ");
            clear();
            return false;
        }
        d_COMPONENTSCOUNT pomComponentID = pomComponent->getId();

        ComponentsMap::iterator pomIt = componentsMap.find(pomComponentID);
        if (pomIt != componentsMap.end())
        {
            pomErrorString << "System::tryLoadFromTxtStream: System already contains component with id = " <<
                              pomComponentID << ".";
            paError.set(e_COMPONENT_ALREADY_EXISTS, &pomErrorString);
            delete pomComponent;
            clear();
            return false;
        }
        componentsMap.insert(ComponentsMap::value_type(pomComponentID, pomComponent));
    }

    structuralFunction = StructuralFunction::createNew(paTxtStream, equalReliability, paIncludeProbabilities, componentsMap, paError);
    if (paError)
    {
        paError.prependText("System::tryLoadFromTxtStream: ");
        clear();
        return false;
    }

    updateOtherStructures();
    isInit = true;

    return true;
}

bool System::tryCalculate(Error &paError)
{
    if (isInit)
    {
        paError.reset();
        std::vector<ComponentMeasures::Type> pomVector;
        ComponentMeasures::getAllTypes(pomVector);
        for (ComponentsMap::iterator it = componentsMap.begin(); it != componentsMap.end(); it++)
        {
            Component* pomComponent = it->second;
            for (std::vector<ComponentMeasures::Type>::iterator it2 = pomVector.begin(); it2 != pomVector.end(); it2++)
            {
                pomComponent->computeComponentIndex(*it2, Third, paError);
                if (paError == e_IMPORTANCE_INDEX_IS_NOT_DEFINED)
                {
                    paError.reset();
                }
                if (paError)
                {
                    return false;
                }
            }
        }

        return true;
    }
    else
        return false;
}

bool System::tryCalculate(const ComponentComputationMap &paComponentComputationMap, Error &paError)
{
    paError.reset();
    if (isInit)
    {
        std::vector<ComponentMeasures::Type> pomVector;
        ComponentMeasures::getAllTypes(pomVector);
        for (ComponentsMap::iterator it = componentsMap.begin(); it != componentsMap.end(); it++)
        {
            Component* pomComponent = it->second;
            d_COMPONENTSCOUNT pomComponentID = pomComponent->getId();
            for (std::vector<ComponentMeasures::Type>::iterator it2 = pomVector.begin(); it2 != pomVector.end(); it2++)
            {
                Level pomLevel = paComponentComputationMap.getLevel(pomComponentID, *it2, paError);
                if (paError)
                {
                    return false;
                }

                pomComponent->computeComponentIndex(*it2, pomLevel, paError);
                if (paError == e_IMPORTANCE_INDEX_IS_NOT_DEFINED)
                {
                    paError.reset();
                }
                if (paError)
                {
                    return false;
                }
            }
        }

        return true;
    }
    else
        return false;
}

void System::initStatesVector(Component &paComponent, VectorWithBitmap<d_STATESCOUNT> &paStatesVector, d_COMPONENTSCOUNT &paFixedComponentIndex)
{
    d_COMPONENTSCOUNT i = 0;

    paStatesVector.resize(componentsCount);
    for (ComponentsMap::iterator pomIt2 = componentsMap.begin(); pomIt2 != componentsMap.end(); pomIt2++)
    {
        if (pomIt2->first == paComponent.getId())
        {
            paFixedComponentIndex = i;
        }
        paStatesVector.trySet(i, 0);
        i++;
    }
}

void System::incrementStatesVector(VectorWithBitmap<d_STATESCOUNT> &paStatesVector)
{
    d_COMPONENTSCOUNT pomCurrentComponentIndex = paStatesVector.getSize() - 1;
    d_STATESCOUNT pomState;

    bool pomNext = true;
    while (pomNext)
    {
        if (paStatesVector.isSet(pomCurrentComponentIndex))
        {
            paStatesVector.tryGet(pomCurrentComponentIndex, pomState);
            pomState++;

            if (pomState >= statesCountsVector[pomCurrentComponentIndex])
            {
                paStatesVector.trySet(pomCurrentComponentIndex, 0);
            }
            else
            {
                paStatesVector.trySet(pomCurrentComponentIndex, pomState);
                pomNext = false;
            }
        }
        if (pomCurrentComponentIndex > 0)
            pomCurrentComponentIndex--;
        else
            pomNext = false;
    }
}

d_SYSTEMCONFIGURATIONSCOUNT System::getSystemConfigurationNumber(VectorWithBitmap<d_STATESCOUNT> &paStatesVector)
{
    d_SYSTEMCONFIGURATIONSCOUNT pomResult = 0;
    bool pomOK;

    for (d_COMPONENTSCOUNT i = 0; i < componentsCount; i++)
    {
        pomResult += paStatesVector.get(i, pomOK) * multipleStatesCountsVector[i];
    }
    return pomResult;
}

d_PROBABILITY System::getProbability(VectorWithBitmap<d_STATESCOUNT> &paStatesVector, Error &paError)
{
    paError.reset();

    d_PROBABILITY pomResult = 1.0;
    for (d_COMPONENTSCOUNT i = 0; i < componentsCount; i++)
    {
        d_STATESCOUNT pomState;
        if (paStatesVector.tryGet(i, pomState))
        {
            pomResult *= componentsVector[i]->getProbability(pomState, paError);
            if (paError)
            {
                paError.prependText("System::getProbability: ");
                return 0;
            }
        }
    }

    return pomResult;
}

bool System::isSystemStateReliable(d_STATESCOUNT paSystemState, d_STATESCOUNT paReliableSystemState)
{
    if (equalReliability)
    {
        if (paSystemState == paReliableSystemState)
            return true;
        else
            return false;
    }
    else
    {
        if (paSystemState >= paReliableSystemState)
            return true;
        else
            return false;
    }
}

d_PROBABILITY System::getStateProbability(d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();

    d_PROBABILITY pomResult = structuralFunction->getSystemStateProbability(paSystemState, paError);
    if (paError)
    {
        paError.prependText("System::getStateProbability: ");
    }
    return pomResult;
}

d_PROBABILITY System::getStateReliability(d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();

    d_PROBABILITY pomResult = structuralFunction->getSystemStateReliability(paSystemState, paError);
    if (paError)
    {
        paError.prependText("System::getStateReliability: ");
    }
    return pomResult;
}

d_PROBABILITY System::calculateConditionalSystemReliability(Component &paComponent,
                                                            d_STATESCOUNT paComponentState,
                                                            d_STATESCOUNT paSystemState,
                                                            Error &paError)
{
    ComponentsMap::iterator pomIt = componentsMap.find(paComponent.getId());
    if (pomIt == componentsMap.end())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateConditionalSystemReliability: Component with id = " << paComponent.getId() << " does not exist in system.";
        paError.set(e_COMPONENT_NOT_EXISTS, &pomStr);
        return 0;
    }
    else if (paComponentState < 0 || paComponentState >= paComponent.getStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateConditionalSystemReliability: Component with id = " << paComponent.getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    else if (paSystemState < 0 || paSystemState >= structuralFunction->getSystemStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateConditionalSystemReliability: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    else
    {
        paError.reset();
    }

    bool pomOK;
    d_PROBABILITY pomProbability = 0;
    d_SYSTEMCONFIGURATIONSCOUNT pomCount = structuralFunction->getSystemConfigurationsCount() / paComponent.getStatesCount();

    d_COMPONENTSCOUNT pomComponentIndex;
    VectorWithBitmap<d_STATESCOUNT> pomStatesVector;

    initStatesVector(paComponent, pomStatesVector, pomComponentIndex);

    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < pomCount; i++)
    {
        pomStatesVector.trySet(pomComponentIndex, paComponentState);
        d_SYSTEMCONFIGURATIONSCOUNT pomGoodSystemConfiguration = getSystemConfigurationNumber(pomStatesVector);

        pomStatesVector.tryReset(pomComponentIndex);
        if (isSystemStateReliable(structuralFunction->getSystemState(pomGoodSystemConfiguration, pomOK), paSystemState))
        {
            pomProbability += getProbability(pomStatesVector, paError);
        }
        if (paError)
        {
            std::stringstream pomStr;
            pomStr << "System::calculateConditionalSystemReliability (component\'s id = " << paComponent.getId() <<
                      ", component\'s state = " << paComponentState << ", system\'s state = " << paSystemState << "): ";
            paError.prependText(&pomStr);
            return 0;
        }
        incrementStatesVector(pomStatesVector);
    }

    return pomProbability;
}

d_PROBABILITY System::calculateConditionalSystemConfigurationsCount(Component &paComponent,
                                                                    d_STATESCOUNT paComponentState,
                                                                    d_STATESCOUNT paSystemState,
                                                                    Error &paError)
{
    ComponentsMap::iterator pomIt = componentsMap.find(paComponent.getId());
    if (pomIt == componentsMap.end())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateConditionalSystemConfigurationsCount: Component with id = " << paComponent.getId() << " does not exist in system.";
        paError.set(e_COMPONENT_NOT_EXISTS, &pomStr);
        return 0;
    }
    else if (paComponentState < 0 || paComponentState >= paComponent.getStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateConditionalSystemConfigurationsCount: Component with id = " << paComponent.getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    else if (paSystemState < 0 || paSystemState >= structuralFunction->getSystemStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateConditionalSystemConfigurationsCount: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    else
    {
        paError.reset();
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomResult;
    d_SYSTEMCONFIGURATIONSCOUNT pomStep;
    d_SYSTEMCONFIGURATIONSCOUNT pomK;
    d_SYSTEMCONFIGURATIONSCOUNT pomCount;
    d_SYSTEMCONFIGURATIONSCOUNT j;
    bool pomOK;

    pomStep = 1;
    for (ComponentsMap::iterator pomIt2 = ++pomIt; pomIt2 != componentsMap.end(); pomIt2++)
    {
        pomStep *= pomIt2->second->getStatesCount();
    }
    pomK = pomStep * (paComponent.getStatesCount() - 1);

    pomResult = 0;
    pomCount = structuralFunction->getSystemConfigurationsCount() / paComponent.getStatesCount();
    j = paComponentState * pomStep;
    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < pomCount; i++)
    {
        if (isSystemStateReliable(structuralFunction->getSystemState(j,pomOK), paSystemState))
        {
            pomResult += 1;
        }
        j++;
        if ((i+1) % pomStep == 0)
        {
            j = j + pomK;
        }
    }

    return pomResult;
}

bool System::tryCalculateCriticalPathVectorsCountAndProbability(Component &paComponent,
                                                                d_STATESCOUNT paComponentState,
                                                                d_STATESCOUNT paSystemState,
                                                                d_SYSTEMCONFIGURATIONSCOUNT &paCoherentCriticalPathVectorsCount,
                                                                d_PROBABILITY &paCoherentCriticalPathVectorsProbability,
                                                                d_SYSTEMCONFIGURATIONSCOUNT &paNonCoherentCriticalPathVectorsCount,
                                                                d_PROBABILITY &paNonCoherentCriticalPathVectorsProbability,
                                                                Error &paError)
{
    paCoherentCriticalPathVectorsCount = 0;
    paCoherentCriticalPathVectorsProbability = 0;
    paNonCoherentCriticalPathVectorsCount = 0;
    paNonCoherentCriticalPathVectorsProbability = 0;

    ComponentsMap::iterator pomIt = componentsMap.find(paComponent.getId());
    if (pomIt == componentsMap.end())
    {
        std::stringstream pomStr;
        pomStr << "System::tryCalculateCriticalPathVectorsCountAndProbability: Component with id = " << paComponent.getId() << " does not exist in system.";
        paError.set(e_COMPONENT_NOT_EXISTS, &pomStr);
        return false;
    }
    else if (paComponentState < 0 || paComponentState >= paComponent.getStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "System::tryCalculateCriticalPathVectorsCountAndProbability: Component with id = " << paComponent.getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return false;
    }
    else if (paComponentState == 0)
    {
        std::stringstream pomStr;
        pomStr << "System::tryCalculateCriticalPathVectorsCountAndProbability: Cannot calculate critical path vectors for component with id = " << paComponent.getId() << " and state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return false;
    }
    else if (paSystemState < 0 || paSystemState >= structuralFunction->getSystemStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "System::tryCalculateCriticalPathVectorsCountAndProbability: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return false;
    }
    else if (paSystemState == 0)
    {
        std::stringstream pomStr;
        pomStr << "System::tryCalculateCriticalPathVectorsCountAndProbability: Cannot calculate critical path vectors for system\'s state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return false;
    }
    else
    {
        paError.reset();
    }

    bool pomOK;
    d_SYSTEMCONFIGURATIONSCOUNT pomCoherentCriticalPathVectorsCount = 0;
    d_PROBABILITY pomCoherentCriticalPathVectorsProbability = 0;
    d_SYSTEMCONFIGURATIONSCOUNT pomNonCoherentCriticalPathVectorsCount = 0;
    d_PROBABILITY pomNonCoherentCriticalPathVectorsProbability = 0;


    d_STATESCOUNT pomWorseSystemState = paSystemState - 1;
    d_SYSTEMCONFIGURATIONSCOUNT pomCount = structuralFunction->getSystemConfigurationsCount() / paComponent.getStatesCount();

    d_COMPONENTSCOUNT pomComponentIndex;
    VectorWithBitmap<d_STATESCOUNT> pomStatesVector;

    initStatesVector(paComponent, pomStatesVector, pomComponentIndex);

    d_COMPONENTSCOUNT pomGoodComponentState = paComponentState;
    d_COMPONENTSCOUNT pomBadComponentState = paComponentState-1;

    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < pomCount; i++)
    {
        pomStatesVector.trySet(pomComponentIndex, pomGoodComponentState);
        d_SYSTEMCONFIGURATIONSCOUNT pomGoodSystemConfiguration = getSystemConfigurationNumber(pomStatesVector);

        pomStatesVector.trySet(pomComponentIndex, pomBadComponentState);
        d_SYSTEMCONFIGURATIONSCOUNT pomBadSystemConfiguration = getSystemConfigurationNumber(pomStatesVector);

        pomStatesVector.tryReset(pomComponentIndex);

        if ((structuralFunction->getSystemState(pomBadSystemConfiguration, pomOK) <= pomWorseSystemState) &&
            isSystemStateReliable(structuralFunction->getSystemState(pomGoodSystemConfiguration, pomOK), paSystemState))
        {
            d_PROBABILITY pomProbability = getProbability(pomStatesVector, paError);
            if (paError)
            {
                std::stringstream pomStr;
                pomStr << "System::tryCalculateCriticalPathVectorsCountAndProbability (component\'s id = " << paComponent.getId() <<
                          ", component\'s state = " << paComponentState << ", system\'s state = " << paSystemState << "): ";
                paError.prependText(&pomStr);
                return false;
            }

            pomCoherentCriticalPathVectorsCount++;
            pomCoherentCriticalPathVectorsProbability += pomProbability;
        }
        else if (isSystemStateReliable(structuralFunction->getSystemState(pomBadSystemConfiguration, pomOK), paSystemState) &&
                 (structuralFunction->getSystemState(pomGoodSystemConfiguration, pomOK) <= pomWorseSystemState))
        {
            d_PROBABILITY pomProbability = getProbability(pomStatesVector, paError);
            if (paError)
            {
                std::stringstream pomStr;
                pomStr << "System::tryCalculateCriticalPathVectorsCountAndProbability (component\'s id = " << paComponent.getId() <<
                          ", component\'s state = " << paComponentState << ", system\'s state = " << paSystemState << "): ";
                paError.prependText(&pomStr);
                return false;
            }

            pomNonCoherentCriticalPathVectorsCount++;
            pomNonCoherentCriticalPathVectorsProbability += pomProbability;
        }

        incrementStatesVector(pomStatesVector);
    }

    paCoherentCriticalPathVectorsCount = pomCoherentCriticalPathVectorsCount;
    paCoherentCriticalPathVectorsProbability = pomCoherentCriticalPathVectorsProbability;
    paNonCoherentCriticalPathVectorsCount = pomNonCoherentCriticalPathVectorsCount;
    paNonCoherentCriticalPathVectorsProbability = pomNonCoherentCriticalPathVectorsProbability;

    return true;
}

void System::calculateDPLR(Component &paComponent,
                           d_STATESCOUNT paComponentState,
                           d_STATESCOUNT paSystemState,
                           DPLR &paDPLR,
                           Error &paError)
{
    ComponentsMap::iterator pomIt = componentsMap.find(paComponent.getId());
    if (pomIt == componentsMap.end())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateDPLR: Component with id = " << paComponent.getId() << " does not exist in system.";
        paError.set(e_COMPONENT_NOT_EXISTS, &pomStr);
        return;
    }
    else if (paComponentState < 0 || paComponentState >= paComponent.getStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateDPLR: Component with id = " << paComponent.getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return;
    }
    else if (paComponentState == 0)
    {
        std::stringstream pomStr;
        pomStr << "System::calculateDPLR: Cannot calculate DPLR for component with id = " << paComponent.getId() << " and state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return;
    }
    else if (paSystemState < 0 || paSystemState >= structuralFunction->getSystemStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "System::calculateDPLR: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return;
    }
    else if (paSystemState == 0)
    {
        std::stringstream pomStr;
        pomStr << "System::calculateDPLR: Cannot calculate DPLR for system\'s state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return;
    }
    else
    {
        paError.reset();
    }

    bool pomOK;
    std::vector<d_LDVALUE> pomDPLRVector;
    std::vector<d_PROBABILITY> pomDPLRProbabilitiesVector;

    d_SYSTEMCONFIGURATIONSCOUNT pomGoodSystemStatesCount = 0;

    d_STATESCOUNT pomWorseSystemState = paSystemState - 1;
    d_SYSTEMCONFIGURATIONSCOUNT pomCount = structuralFunction->getSystemConfigurationsCount() / paComponent.getStatesCount();

    d_COMPONENTSCOUNT pomComponentIndex;
    VectorWithBitmap<d_STATESCOUNT> pomStatesVector;

    initStatesVector(paComponent, pomStatesVector, pomComponentIndex);

    d_COMPONENTSCOUNT pomGoodComponentState = paComponentState;
    d_COMPONENTSCOUNT pomBadComponentState = paComponentState-1;

    pomDPLRVector.resize(pomCount);
    pomDPLRProbabilitiesVector.resize(pomCount);
    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < pomCount; i++)
    {
        pomStatesVector.trySet(pomComponentIndex, pomGoodComponentState);
        d_SYSTEMCONFIGURATIONSCOUNT pomGoodSystemConfiguration = getSystemConfigurationNumber(pomStatesVector);

        pomStatesVector.trySet(pomComponentIndex, pomBadComponentState);
        d_SYSTEMCONFIGURATIONSCOUNT pomBadSystemConfiguration = getSystemConfigurationNumber(pomStatesVector);

        if (isSystemStateReliable(structuralFunction->getSystemState(pomGoodSystemConfiguration, pomOK), paSystemState))
        {
            pomGoodSystemStatesCount++;
        }

        pomStatesVector.tryReset(pomComponentIndex);
        pomDPLRProbabilitiesVector[i] = getProbability(pomStatesVector, paError);
        if (paError)
        {
            std::stringstream pomStr;
            pomStr << "System::calculateDPLR (component\'s id = " << paComponent.getId() <<
                      ", component\'s state = " << paComponentState << ", system\'s state = " << paSystemState << "): ";
            paError.prependText(&pomStr);
            return;
        }

        if ((structuralFunction->getSystemState(pomBadSystemConfiguration, pomOK) <= pomWorseSystemState) &&
            isSystemStateReliable(structuralFunction->getSystemState(pomGoodSystemConfiguration, pomOK), paSystemState))
        {
            pomDPLRVector[i] = 1;
        }
        else if (isSystemStateReliable(structuralFunction->getSystemState(pomBadSystemConfiguration, pomOK), paSystemState) &&
                 (structuralFunction->getSystemState(pomGoodSystemConfiguration, pomOK) <= pomWorseSystemState))
        {
            pomDPLRVector[i] = -1;
        }
        else
        {
            pomDPLRVector[i] = 0;
        }

        incrementStatesVector(pomStatesVector);
    }

    paDPLR.set(pomDPLRVector, pomDPLRProbabilitiesVector, pomGoodSystemStatesCount, paError);
    if (paError)
    {
        std::stringstream pomStr;
        pomStr << "System::calculateDPLR (component\'s id = " << paComponent.getId() <<
                  ", component\'s state = " << paComponentState << ", system\'s state = " << paSystemState << "): ";
        paError.prependText(&pomStr);
        return;
    }
}

void System::getComponents(ComponentsMap &paComponents)
{
    paComponents = componentsMap;
}

void System::getComponents(ComponentsVector &paComponents)
{
    paComponents = componentsVector;
}

void System::set(std::string &paSystemName, std::vector<Component*> &paComponents, StructuralFunction &paStructuralFunction, Error &paError)
{
    clear();

    paError.reset();
    name = paSystemName;
    d_SYSTEMCONFIGURATIONSCOUNT pomSysConfigCount = 1;
    ComponentsMap::iterator pomIt;
    Component *pomComponent;
    for (std::vector<Component*>::iterator it = paComponents.begin(); it != paComponents.end(); it++)
    {
        pomComponent = *it;
        pomIt = componentsMap.find(pomComponent->getId());
        if (pomIt != componentsMap.end())
        {
            componentsMap.clear();
            std::stringstream pomStr;
            pomStr << "System::set: Component with ID = " << pomComponent->getId() << " does already exist in system.";
            paError.set(e_SYSTEM_IS_NOT_INIT, &pomStr);
            return;
        }
        componentsMap.insert(ComponentsMap::value_type(pomComponent->getId(), pomComponent));
        pomSysConfigCount *= (d_SYSTEMCONFIGURATIONSCOUNT)pomComponent->getStatesCount();
    }
    if (paStructuralFunction.getSystemConfigurationsCount() != pomSysConfigCount)
    {
        componentsMap.clear();
        std::stringstream pomStr;
        pomStr << "System::set: count of system cofigurations of structuralFunction = " << paStructuralFunction.getSystemConfigurationsCount() <<
                  "is different from count computed from components = " << pomSysConfigCount << ".";
        paError.set(e_SYSTEM_IS_NOT_INIT, &pomStr);
        return;
    }

    isInit = true;
}

void System::exportToCSVStream(Level paLevel, std::vector<ComponentMeasures::Type> &paMeasures, char paSeparator, std::ostream &paCSVStream)
{
    if (!isInit)
        return;

    d_STATESCOUNT pomSystemStatesCount = structuralFunction->getSystemStatesCount();
    d_MEASURESCOUNT pomCount = paMeasures.size();


    if (paLevel == None)
    {
        paCSVStream << "Component ID";
    }
    else
    {
        if (paLevel == First)
        {
            paCSVStream << "\"" << "Component ID" << "\"";
        }
        for (d_MEASURESCOUNT i = 0; i < pomCount; i++)
        {
            paCSVStream << paSeparator << "\"" << ComponentMeasures::toString(paMeasures[i]) << "\"";

            if (paLevel > First)
            {
                for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                {
                    paCSVStream << paSeparator;
                }
                paCSVStream << paSeparator << paSeparator;

                if (i < pomCount - 1)
                    paCSVStream << paSeparator;
            }
        }
    }
    paCSVStream << std::endl;

    for (ComponentsMap::iterator it = componentsMap.begin(); it != componentsMap.end(); it++)
    {
        Component *pomComponent = it->second;
        pomComponent->exportToCSVStream(paLevel, paMeasures, paSeparator, paCSVStream);
    }
}
