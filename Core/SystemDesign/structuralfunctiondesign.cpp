#include "structuralfunctiondesign.h"

StructuralFunctionDesign::StructuralFunctionDesign()
{
    defaultInit();
}

StructuralFunctionDesign::StructuralFunctionDesign(std::istream &paTxtStream, ComponentDesignsVector &paComponents, Error &paError)
{
    defaultInit();
    tryLoadFromTxtStream(paTxtStream, paComponents, paError);

    if (paError)
    {
        paError.prependText("StructuralFunctionDesign::StructuralFunctionDesign: ");
    }
}

StructuralFunctionDesign::~StructuralFunctionDesign()
{
    delete systemConfigurationsStates;
    delete systemConfigurationsProbabilities;
    delete systemStatesCounts;
    delete componentsStatesCounts;
    delete multipleComponentsStatesCounts;
}

void StructuralFunctionDesign::defaultInit()
{
    statesCount = 0;
    systemConfigurationsCount = 0;

    systemConfigurationsStates = new StateVector;
    systemConfigurationsProbabilities = new std::vector<d_PROBABILITY>;
    systemStatesCounts = new std::vector<d_SYSTEMCONFIGURATIONSCOUNT>;
    componentsStatesCounts = new StateVector;
    multipleComponentsStatesCounts = new std::vector<d_SYSTEMCONFIGURATIONSCOUNT>;

    systemStatesCounts->resize(1, 0);
}

void StructuralFunctionDesign::clear()
{
    statesCount = 1;
    systemConfigurationsCount = 0;

    systemConfigurationsStates->resize(0);
    systemConfigurationsProbabilities->resize(0);
    systemStatesCounts->resize(1, 0);
    componentsStatesCounts->resize(0);
    multipleComponentsStatesCounts->resize(0);
}

d_SYSTEMCONFIGURATIONSCOUNT StructuralFunctionDesign::computeSystemConfigurationsCount(ComponentDesignsVector &paComponents,
                                                                                       StateVector &paComponentsStatesCountsVector,
                                                                                       std::vector<d_SYSTEMCONFIGURATIONSCOUNT> &paMultipleComponentsStatesCountsVector,
                                                                                       Error &paError)
{
    paError.reset();
    std::stringstream pomErrorString;

    d_COMPONENTSCOUNT pomComponentsCount = paComponents.size();
    if (pomComponentsCount <= 0)
    {
        return 0;
    }
    if (pomComponentsCount > c_MAX_COMPONENTS_COUNT)
    {
        pomErrorString << "StructuralFunctionDesign::computeSystemConfigurationsCount: Too much components (components count = " << pomComponentsCount <<
                          " is more than " << c_MAX_COMPONENTS_COUNT << ").";
        paError.set(e_COMPONENTS_COUNT_IS_INVALID, &pomErrorString);
        return 0;
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomSystemConfigurationsCount = 1;
    for (ComponentDesignsVector::iterator it = paComponents.begin(); it != paComponents.end(); it++)
    {
        ComponentDesign *pomComponent = *it;

        d_SYSTEMCONFIGURATIONSCOUNT pom = pomSystemConfigurationsCount * pomComponent->getStatesCount();
        if (pom < pomSystemConfigurationsCount || pom > c_MAX_SYSTEM_CONFIGURATIONS_COUNT)
        {
            pomErrorString << "StructuralFunctionDesign::computeSystemConfigurationsCount: Component with id = " << pomComponent->getId() <<
                              " will cause excess of maximal system configurations count = " <<
                              c_MAX_SYSTEM_CONFIGURATIONS_COUNT << ".";
            paError.set(e_SYSTEM_CONFIGURATIONS_COUNT_LIMIT_EXCEEDED, &pomErrorString);
            return 0;
        }
        pomSystemConfigurationsCount = pom;

        if (!pomComponent->hasSetProbabilities())
        {
            pomErrorString << "StructuralFunctionDesign::computeSystemConfigurationsCount: Component with id = " << pomComponent->getId() <<
                              " has not set probabilities.";
            paError.set(e_PROBABILITY_IS_NOT_SET, &pomErrorString);
            return 0;
        }
    }

    paComponentsStatesCountsVector.resize(pomComponentsCount);
    paMultipleComponentsStatesCountsVector.resize(pomComponentsCount);
    pomSystemConfigurationsCount = 1;
    for (d_COMPONENTSCOUNT i = pomComponentsCount - 1; i > 0; i--)
    {
        paMultipleComponentsStatesCountsVector.at(i) = pomSystemConfigurationsCount;

        ComponentDesign *pomComponent = paComponents.at(i);
        pomSystemConfigurationsCount *= pomComponent->getStatesCount();
        paComponentsStatesCountsVector.at(i) = pomComponent->getStatesCount();
    }
    {
        paMultipleComponentsStatesCountsVector.at(0) = pomSystemConfigurationsCount;

        ComponentDesign *pomComponent = paComponents.at(0);
        pomSystemConfigurationsCount *= pomComponent->getStatesCount();
        paComponentsStatesCountsVector.at(0) = pomComponent->getStatesCount();
    }

    return pomSystemConfigurationsCount;
}

bool StructuralFunctionDesign::trySet(ComponentDesignsVector &paComponents, Error &paError)
{
    paError.reset();

    d_COMPONENTSCOUNT pomComponentsCount = paComponents.size();
    if (pomComponentsCount <= 0)
    {
        clear();
        return true;
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomSystemConfigurationsCount =
            computeSystemConfigurationsCount(paComponents, *componentsStatesCounts, *multipleComponentsStatesCounts, paError);
    if (paError)
    {
        paError.prependText("StructuralFunctionDesign::trySet: ");
        return false;
    }
    systemConfigurationsCount = pomSystemConfigurationsCount;
    systemConfigurationsStates->resize(systemConfigurationsCount);
    systemConfigurationsStates->assign(systemConfigurationsCount, 0);

    statesCount = 1;
    systemStatesCounts->resize(1, 0);

    computeProbabilities(paComponents);

    return true;
}

bool StructuralFunctionDesign::tryUpdate(ComponentDesignsVector &paComponents, Error &paError)
{
    paError.reset();

    d_COMPONENTSCOUNT pomComponentsCount = paComponents.size();
    if (pomComponentsCount != componentsStatesCounts->size())
    {
        std::stringstream pomErrorString;
        pomErrorString << "StructuralFunctionDesign::tryUpdate: Cannot update structural function because count of components has been changed.";
        paError.set(e_STRUCTURAL_FUNCTION_IS_NOT_UPDATED, &pomErrorString);
        return false;
    }
    if (pomComponentsCount <= 0)
        return true;


    std::vector<d_SYSTEMCONFIGURATIONSCOUNT> *pomNewSystemStatesCounts = new std::vector<d_SYSTEMCONFIGURATIONSCOUNT>;
    StateVector *pomNewComponentsStatesCounts = new StateVector;
    std::vector<d_SYSTEMCONFIGURATIONSCOUNT> *pomNewMultipleComponentsStatesCounts = new std::vector<d_SYSTEMCONFIGURATIONSCOUNT>;
    StateVector *pomNewSystemConfigurationsStates = new StateVector;

    pomNewSystemStatesCounts->resize(statesCount, 0);
    d_SYSTEMCONFIGURATIONSCOUNT pomNewSystemConfigurationsCount = computeSystemConfigurationsCount(paComponents, *pomNewComponentsStatesCounts, *pomNewMultipleComponentsStatesCounts, paError);
    if (paError)
    {
        delete pomNewSystemStatesCounts;
        delete pomNewComponentsStatesCounts;
        delete pomNewMultipleComponentsStatesCounts;
        delete pomNewSystemConfigurationsStates;

        paError.prependText("StructuralFunctionDesign::tryUpdate: ");
        return false;
    }

    pomNewSystemConfigurationsStates->resize(pomNewSystemConfigurationsCount);

    StateVector pomNewStatesVector;
    initStatesVector(pomNewStatesVector, *pomNewComponentsStatesCounts);
    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < pomNewSystemConfigurationsCount; i++)
    {
        if (isStatesVectorOk(pomNewStatesVector, *componentsStatesCounts))
        {
            d_SYSTEMCONFIGURATIONSCOUNT pomConfigNumber = toConfigNumber(pomNewStatesVector, *multipleComponentsStatesCounts);
            (*pomNewSystemConfigurationsStates)[i] = (*systemConfigurationsStates)[pomConfigNumber];
        }
        else
        {
            (*pomNewSystemConfigurationsStates)[i] = 0;
        }
        (*pomNewSystemStatesCounts)[(*pomNewSystemConfigurationsStates)[i]]++;
        incrementStatesVector(pomNewStatesVector, *pomNewComponentsStatesCounts);
    }

    statesCount = pomNewSystemStatesCounts->size();
    systemConfigurationsCount = pomNewSystemConfigurationsCount;

    delete systemConfigurationsStates;
    systemConfigurationsStates = pomNewSystemConfigurationsStates;

    delete systemStatesCounts;
    systemStatesCounts = pomNewSystemStatesCounts;

    delete componentsStatesCounts;
    componentsStatesCounts = pomNewComponentsStatesCounts;

    delete multipleComponentsStatesCounts;
    multipleComponentsStatesCounts = pomNewMultipleComponentsStatesCounts;

    computeProbabilities(paComponents);

    return true;
}

void StructuralFunctionDesign::computeProbabilities(ComponentDesignsVector &paComponents)
{
    systemConfigurationsProbabilities->resize(systemConfigurationsCount);

    d_COMPONENTSCOUNT pomComponentsCount = paComponents.size();
    StateVector pomCode;
    pomCode.assign(pomComponentsCount, 0);
    d_COMPONENTSCOUNT pomComponentIndex = pomComponentsCount - 1;

    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < systemConfigurationsCount; i++)
    {
        d_PROBABILITY pomProbability = 1;
        for (d_COMPONENTSCOUNT j = 0; j < pomComponentsCount; j++)
        {
            pomProbability *= paComponents[j]->getProbability(pomCode[j]);
        }
        (*systemConfigurationsProbabilities)[i] = pomProbability;

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
}

d_SYSTEMCONFIGURATIONSCOUNT StructuralFunctionDesign::toConfigNumber(StateVector &paStatesVector, std::vector<d_SYSTEMCONFIGURATIONSCOUNT> &paMultipleVector)
{
    d_COMPONENTSCOUNT pomCount = paStatesVector.size();
    d_SYSTEMCONFIGURATIONSCOUNT pomResult = 0;
    for (d_COMPONENTSCOUNT i = 0; i < pomCount; i++)
    {
        pomResult += paMultipleVector[i] * paStatesVector[i];
    }

    return pomResult;
}

void StructuralFunctionDesign::toStatesVector(d_SYSTEMCONFIGURATIONSCOUNT paConfigNumber, StateVector &paComponentsStatesCountsVector, StateVector &paStatesVector)
{
    d_SYSTEMCONFIGURATIONSCOUNT pomConfigNumber = paConfigNumber;
    d_COMPONENTSCOUNT pomCount = paComponentsStatesCountsVector.size();
    paStatesVector.resize(pomCount);
    paStatesVector.assign(pomCount, 0);

    for (d_COMPONENTSCOUNT i = 0; i < pomCount; i++)
    {
        paStatesVector[pomCount - 1 - i] = pomConfigNumber % paComponentsStatesCountsVector[pomCount - 1 - i];
        pomConfigNumber = pomConfigNumber / paComponentsStatesCountsVector[pomCount - 1 - i];
    }
}

void StructuralFunctionDesign::initStatesVector(StateVector &paStatesVector, StateVector &paStatesCountsVector)
{
    paStatesVector.clear();
    paStatesVector.resize(paStatesCountsVector.size());
}

bool StructuralFunctionDesign::isStatesVectorOk(StateVector &paStatesVector, StateVector &paStatesCountsVector)
{
    d_COMPONENTSCOUNT pomCount = paStatesCountsVector.size();

    for (d_COMPONENTSCOUNT i = 0; i < pomCount; i++)
    {
        if (paStatesVector[i] >= paStatesCountsVector[i])
            return false;
    }
    return true;
}

void StructuralFunctionDesign::incrementStatesVector(StateVector &paStatesVector, StateVector &paStatesCountsVector)
{
    d_COMPONENTSCOUNT pomCurrentComponentIndex = paStatesVector.size() - 1;

    bool pomNext = true;
    while (pomNext)
    {
        paStatesVector[pomCurrentComponentIndex]++;
        if (paStatesVector[pomCurrentComponentIndex] >= paStatesCountsVector[pomCurrentComponentIndex])
        {
            paStatesVector[pomCurrentComponentIndex] = 0;
        }
        else
        {
            pomNext = false;
        }
        if (pomCurrentComponentIndex > 0)
            pomCurrentComponentIndex--;
        else
            pomNext = false;
    }
}

void StructuralFunctionDesign::systemConfigurationNumberToStatesVector(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationNumber, StateVector &paStatesVector)
{
    toStatesVector(paSystemConfigurationNumber, *componentsStatesCounts, paStatesVector);
}

void StructuralFunctionDesign::setSystemState(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex, d_STATESCOUNT paState)
{
    d_STATESCOUNT pomState = paState;
    if (pomState <= 0)
        pomState = 0;
    if (pomState >= c_MAX_SYSTEM_STATES_COUNT)
        pomState = c_MAX_SYSTEM_STATES_COUNT - 1;

    if (paSystemConfigurationIndex >= 0 && paSystemConfigurationIndex < systemConfigurationsCount)
    {
        if ((*systemConfigurationsStates)[paSystemConfigurationIndex] == pomState)
            return;

        (*systemStatesCounts)[(*systemConfigurationsStates)[paSystemConfigurationIndex]]--;
        (*systemConfigurationsStates)[paSystemConfigurationIndex] = pomState;
        if (pomState >= statesCount)
        {
            statesCount = pomState + 1;
            systemStatesCounts->resize(statesCount, 0);
        }
        (*systemStatesCounts)[pomState]++;

        d_STATESCOUNT pomState2 = statesCount - 1;
        while (pomState2 > 0 && (*systemStatesCounts)[pomState2] <= 0)
        {
            pomState2--;
        }
        if (pomState2 != statesCount - 1)
        {
            statesCount = pomState2 + 1;
            systemStatesCounts->resize(statesCount);
        }
    }
}

bool StructuralFunctionDesign::tryLoadFromTxtStream(std::istream &paTxtStream, ComponentDesignsVector &paComponents, Error &paError)
{
    clear();

    paError.reset();
    std::stringstream pomErrorString;

    if (paComponents.size() <= 0)
    {
        return true;
    }

    systemConfigurationsCount = computeSystemConfigurationsCount(paComponents, *componentsStatesCounts, *multipleComponentsStatesCounts, paError);
    if (paError)
    {
        clear();
        paError.prependText("StructuralFunctionDesign::tryLoadFromTxtStream: ");
        return false;
    }

    d_STATESCOUNT pomState;
    systemConfigurationsStates->resize(systemConfigurationsCount);
    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < systemConfigurationsCount; i++)
    {
        if (paTxtStream.eof())
        {
            pomErrorString << "StructuralFunctionDesign::tryLoadFromTxtStream: Reach end of file before getting system configuration with number = " <<
                              i+1 << ".";
            paError.set(e_REACH_EOF, &pomErrorString);
            clear();
            return false;
        }
        paTxtStream >> pomState;
        if (paTxtStream.fail())
        {
            pomErrorString << "StructuralFunctionDesign::tryLoadFromTxtStream: Error when getting system configuration with number = " <<
                              i+1 << ".";
            paError.set(e_BAD_VALUE, &pomErrorString);
            clear();
            return false;
        }
        if (pomState < 0 || pomState >= c_MAX_SYSTEM_STATES_COUNT)
        {
            pomErrorString << "StructuralFunctionDesign::tryLoadFromTxtStream: System configuration " <<
                              i+1 << " has state = " << pomState << " but state is an integer from 0 to " << c_MAX_SYSTEM_STATES_COUNT - 1 << ">.";
            paError.set(e_SYSTEM_STATE_IS_INVALID, &pomErrorString);
            clear();
            return false;
        }

        systemConfigurationsStates->at(i) = pomState;
        if (pomState >= statesCount)
        {
            statesCount = pomState + 1;
            systemStatesCounts->resize(statesCount, 0);
        }
        systemStatesCounts->at(pomState)++;
    }

    computeProbabilities(paComponents);

    return true;
}

StructuralFunctionDesign* StructuralFunctionDesign::createNew(std::istream &paTxtStream, ComponentDesignsVector &paComponents, Error &paError)
{
    paError.reset();
    StructuralFunctionDesign* pomResult = new StructuralFunctionDesign(paTxtStream, paComponents, paError);
    if (paError)
    {
        paError.prependText("StructuralFunctionDesign::createNew: ");
        delete pomResult;
        pomResult = NULL;
    }

    return pomResult;
}

StructuralFunctionDesign* StructuralFunctionDesign::createNew()
{
    StructuralFunctionDesign* pomResult = new StructuralFunctionDesign();
    return pomResult;
}

void StructuralFunctionDesign::saveToTxtStream(std::ostream &paTxtStream)
{
    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < systemConfigurationsCount; i++)
        paTxtStream << (*systemConfigurationsStates)[i] << std::endl;
}

bool StructuralFunctionDesign::isSet(ComponentDesignsVector &paComponents)
{
    d_COMPONENTSCOUNT pomCount = paComponents.size();
    if (pomCount != componentsStatesCounts->size())
        return false;
    for (d_COMPONENTSCOUNT i = 0; i < pomCount; i++)
    {
        if (paComponents[i]->getStatesCount() != (*componentsStatesCounts)[i])
            return false;
    }

    return true;
}

bool StructuralFunctionDesign::isNonDecreasing(d_SYSTEMCONFIGURATIONSCOUNT &paSystemConfigurationNumberDown, d_SYSTEMCONFIGURATIONSCOUNT &paSystemConfigurationNumberUp)
{
    d_COMPONENTSCOUNT pomComponentsCount = componentsStatesCounts->size();
    StateVector pomCode;
    initStatesVector(pomCode, *componentsStatesCounts);

    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < systemConfigurationsCount; i++)
    {
        bool pomWasIncrease = false;
        for (d_COMPONENTSCOUNT j = 0; j < pomComponentsCount; j++)
        {
            if (j > 0 && pomWasIncrease)
            {
                pomCode[j-1]--;
            }

            if (pomCode[j] + 1 < (*componentsStatesCounts)[j])
            {
                d_SYSTEMCONFIGURATIONSCOUNT pomConfigNumberDown = toConfigNumber(pomCode, *multipleComponentsStatesCounts);
                d_STATESCOUNT pomStateDown = (*systemConfigurationsStates).at(pomConfigNumberDown);

                pomCode[j]++;
                pomWasIncrease = true;

                d_SYSTEMCONFIGURATIONSCOUNT pomConfigNumberUp = toConfigNumber(pomCode, *multipleComponentsStatesCounts);
                d_STATESCOUNT pomStateUp = (*systemConfigurationsStates).at(pomConfigNumberUp);

                if (pomStateDown > pomStateUp)
                {
                    paSystemConfigurationNumberDown = pomConfigNumberDown;
                    paSystemConfigurationNumberUp = pomConfigNumberUp;
                    return false;
                }
            }
            else
            {
                pomWasIncrease = false;
            }

        }
        if (pomWasIncrease)
            pomCode.at(pomComponentsCount-1)--;

        incrementStatesVector(pomCode, *componentsStatesCounts);
    }

    paSystemConfigurationNumberDown = 0;
    paSystemConfigurationNumberUp = 1;
    return true;
}

