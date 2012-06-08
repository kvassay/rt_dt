#include "structuralfunction.h"

StructuralFunction::StructuralFunction(std::istream &paTxtStream, bool paEqualReliability, bool paComputeProbabilities, ComponentsMap &paComponents, Error &paError)
{
    paError.reset();
    defaultInit();
    tryLoadFromTxtStream(paTxtStream, paEqualReliability, paComputeProbabilities, paComponents, paError);
    if (paError)
    {
        paError.prependText("StructuralFunction::StructuralFunction: ");
    }
}

StructuralFunction::StructuralFunction(StateVector &paSystemConfigurationsStates, bool paEqualReliability)
{
    defaultInit();

    equalReliability = paEqualReliability;
    statesCount = 0;
    systemConfigurationsCount = paSystemConfigurationsStates.size();
    systemConfigurationsStates.resize(systemConfigurationsCount);

    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < systemConfigurationsCount; i++)
    {
        systemConfigurationsStates[i] = paSystemConfigurationsStates[i];
        if (systemConfigurationsStates[i] > statesCount)
            statesCount = systemConfigurationsStates[i];
    }

    statesCount++;
}

void StructuralFunction::defaultInit()
{
    equalReliability = false;

    statesCount = 0;
    systemConfigurationsCount = 0;

    systemConfigurationsStates.resize(0);
    systemConfigurationsProbabilities.resize(0);
    systemStatesProbabilities.resize(0);
}

bool StructuralFunction::tryLoadFromTxtStream(std::istream &paTxtStream, bool paEqualReliability, bool paComputeProbabilities, ComponentsMap &paComponents, Error &paError)
{
    paError.reset();
    std::stringstream pomErrorString;

    equalReliability = paEqualReliability;

    statesCount = 0;
    systemConfigurationsCount = 1;

    if (paComponents.size() <= 0 || paComponents.size() > c_MAX_COMPONENTS_COUNT)
    {
        if (paComponents.size() <= 0)
            pomErrorString << "StructuralFunction::tryLoadFromTxtStream: Size of paComponents = " << paComponents.size() << ".";
        else
            pomErrorString << "StructuralFunction::tryLoadFromTxtStream: Too much components (size of paComponents = " << paComponents.size() <<
                              " is greater than " << c_MAX_COMPONENTS_COUNT << ").";
        paError.set(e_COMPONENTS_COUNT_IS_INVALID, &pomErrorString);
        return false;
    }

    std::vector<Component*> pomComponents;
    pomComponents.resize(paComponents.size());
    d_COMPONENTSCOUNT pomI = 0;
    for (ComponentsMap::iterator it = paComponents.begin(); it != paComponents.end(); it++)
    {
        Component *pomComponent = it->second;
        if (pomComponent == NULL)
        {
            pomErrorString << "StructuralFunction::tryLoadFromTxtStream: Component with id = " << it->first << " does not exist in paComponents.";
            paError.set(e_COMPONENT_NOT_EXISTS, &pomErrorString);
            return false;
        }

        d_SYSTEMCONFIGURATIONSCOUNT pom = systemConfigurationsCount * pomComponent->getStatesCount();
        if (pom < systemConfigurationsCount || pom > c_MAX_SYSTEM_CONFIGURATIONS_COUNT)
        {
            pomErrorString << "StructuralFunction::tryLoadFromTxtStream: Component with id = " << it->first <<
                              " will cause excess of maximal system configurations count = " <<
                              c_MAX_SYSTEM_CONFIGURATIONS_COUNT << ".";
            paError.set(e_SYSTEM_CONFIGURATIONS_COUNT_LIMIT_EXCEEDED, &pomErrorString);
            return false;
        }
        systemConfigurationsCount = pom;

        if (paComputeProbabilities && !pomComponent->hasSetProbabilities())
        {
            pomErrorString << "StructuralFunction::tryLoadFromTxtStream: Component with id = " << it->first <<
                              " has not set probabilities.";
            paError.set(e_PROBABILITY_IS_NOT_SET, &pomErrorString);
            return false;
        }

        pomComponents[pomI] = pomComponent;
        pomI++;
    }

    d_STATESCOUNT pomState;
    systemConfigurationsStates.resize(systemConfigurationsCount);
    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < systemConfigurationsCount; i++)
    {
        if (paTxtStream.eof())
        {
            pomErrorString << "StructuralFunction::tryLoadFromTxtStream: Reach end of file before getting system configuration with number = " <<
                              i+1 << ".";
            paError.set(e_REACH_EOF, &pomErrorString);
            return false;
        }
        paTxtStream >> pomState;
        if (paTxtStream.fail())
        {
            pomErrorString << "StructuralFunction::tryLoadFromTxtStream: Error when getting system configuration with number = " <<
                              i+1 << ".";
            paError.set(e_BAD_VALUE, &pomErrorString);
            return false;
        }
        if (pomState < 0 || pomState >= c_MAX_SYSTEM_STATES_COUNT)
        {
            pomErrorString << "StructuralFunction::tryLoadFromTxtStream: System configuration " <<
                              i+1 << " has state = " << pomState << " but state is an integer from 0 to " << c_MAX_SYSTEM_STATES_COUNT - 1 << ">.";
            paError.set(e_SYSTEM_STATE_IS_INVALID, &pomErrorString);
            return false;
        }

        systemConfigurationsStates[i] = pomState;
        if (pomState > statesCount)
            statesCount = pomState;
    }
    statesCount++;

    if (paComputeProbabilities)
    {
        computeProbabilities(pomComponents);
    }

    return true;
}

d_PROBABILITY StructuralFunction::getSystemStateProbability(d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    if (paSystemState < 0 || paSystemState >= statesCount)
    {
        std::stringstream pomStr;
        pomStr << "StructuralFunction::getSystemStateProbability: system has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    if (paSystemState >= systemStatesProbabilities.size())
    {
        std::stringstream pomStr;
        pomStr << "StructuralFunction::getSystemStateProbability: system has not set probability of state = " << paSystemState << ".";
        paError.set(e_PROBABILITY_IS_NOT_SET, &pomStr);
        return 0;
    }
    return systemStatesProbabilities[paSystemState];
}

d_PROBABILITY StructuralFunction::getSystemStateReliability(d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    if (paSystemState < 0 || paSystemState >= statesCount)
    {
        std::stringstream pomStr;
        pomStr << "StructuralFunction::getSystemStateReliability: system has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    if (paSystemState >= systemStatesProbabilities.size())
    {
        std::stringstream pomStr;
        pomStr << "StructuralFunction::getSystemStateReliability: system has not set reliability of state = " << paSystemState << ".";
        paError.set(e_PROBABILITY_IS_NOT_SET, &pomStr);
        return 0;
    }
    return systemStatesReliabilities[paSystemState];
}

bool StructuralFunction::trySetProbabilities(ComponentsMap &paComponents)
{
    d_SYSTEMCONFIGURATIONSCOUNT pom;
    d_COMPONENTSCOUNT pomCount = paComponents.size();

    std::vector<Component*> pomComponents;
    pomComponents.resize(pomCount);
    d_COMPONENTSCOUNT pomI = 0;
    for (ComponentsMap::iterator it = paComponents.begin(); it != paComponents.end(); it++)
    {
        pomComponents[pomI] = it->second;
        pomI++;
    }

    pom = 1;
    for (d_COMPONENTSCOUNT i = 0; i < pomCount; i++)
        pom = pom * pomComponents[i]->getStatesCount();

    if (systemConfigurationsCount != pom)
        return false;

    computeProbabilities(pomComponents);

    return true;
}

void StructuralFunction::computeProbabilities(std::vector<Component*> &paComponents)
{
    systemConfigurationsProbabilities.resize(systemConfigurationsCount);
    systemStatesProbabilities.resize(statesCount);
    systemStatesProbabilities.assign(statesCount, 0);

    d_COMPONENTSCOUNT pomComponentsCount = paComponents.size();
    StateVector pomCode;
    pomCode.assign(pomComponentsCount, 0);
    d_COMPONENTSCOUNT pomComponentIndex = pomComponentsCount - 1;

    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < systemConfigurationsCount; i++)
    {
        d_PROBABILITY pomProbability = 1;
        d_PROBABILITY pomProbability2;
        for (d_COMPONENTSCOUNT j = 0; j < pomComponentsCount; j++)
        {
            paComponents[j]->tryGetProbability(pomCode[j], pomProbability2);
            pomProbability *= pomProbability2;
        }
        systemConfigurationsProbabilities[i] = pomProbability;
        systemStatesProbabilities[systemConfigurationsStates[i]] += pomProbability;

        pomCode[pomComponentIndex]++;
        if (pomCode[pomComponentIndex] >= paComponents[pomComponentIndex]->getStatesCount())
        {
            while (pomComponentIndex > 0 && pomCode[pomComponentIndex] >= paComponents[pomComponentIndex]->getStatesCount())
            {
                pomCode[pomComponentIndex] = 0;
                pomComponentIndex--;
                pomCode[pomComponentIndex]++;
            }
            pomComponentIndex = pomComponentsCount - 1;
        }
    }

    computeReliabilities();
}

StructuralFunction * StructuralFunction::createNew(std::istream &paTxtStream, bool paEqualReliability, bool paComputeProbabilities, ComponentsMap &paComponents, Error &paError)
{
    paError.reset();
    StructuralFunction* pomResult = new StructuralFunction(paTxtStream, paEqualReliability, paComputeProbabilities, paComponents, paError);
    if (paError)
    {
        paError.prependText("StructuralFunction::createNew: ");
        delete pomResult;
        pomResult = NULL;
    }

    return pomResult;
}

StructuralFunction * StructuralFunction::createNew(StateVector &paSystemConfigurationsStates, bool paEqualReliability, Error &paError)
{
    paError.reset();
    StructuralFunction* pomResult = new StructuralFunction(paSystemConfigurationsStates, paEqualReliability);

    return pomResult;
}

void StructuralFunction::computeReliabilities()
{
    systemStatesReliabilities.resize(statesCount);
    systemStatesReliabilities.assign(statesCount, 0);

    for (d_STATESCOUNT i = 0; i < statesCount; i++)
    {
        if (equalReliability)
        {
            systemStatesReliabilities[i] = systemStatesProbabilities[i];
        }
        else
        {
            for (d_STATESCOUNT j = 0; j <= i; j++)
            {
                systemStatesReliabilities[j] += systemStatesProbabilities[i];
            }
        }
    }
}
