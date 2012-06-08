#include "systemdesign.h"

SystemDesign::SystemDesign()
{
    name = "";
    isSaved = true;

    componentsCount = 0;
    maxComponentsCount = 0;
    lastComponentsCount = 0;

    structuralFunction = StructuralFunctionDesign::createNew();
}

SystemDesign::~SystemDesign()
{
    removeAllSystemDesignComponentsCountChangedEventHandler();
    clear();

    if (structuralFunction != NULL)
    {
        delete structuralFunction;
        structuralFunction = NULL;
    }
}

ComponentDesign* SystemDesign::getComponentByNumber(d_COMPONENTSCOUNT paComponentNumber)
{
    if (paComponentNumber >= 1 && paComponentNumber <= componentsCount)
        return components[paComponentNumber - 1];
    else
        return NULL;
}

void SystemDesign::setComponentsCount(d_COMPONENTSCOUNT paComponentsCount, Error &paError)
{
    paError.reset();

    d_COMPONENTSCOUNT pomComponentsCount = paComponentsCount;
    if (pomComponentsCount < 0)
    {
        pomComponentsCount = 0;
    }
    if (pomComponentsCount > c_MAX_COMPONENTS_COUNT)
    {
        pomComponentsCount = c_MAX_COMPONENTS_COUNT;
    }

    if (pomComponentsCount == componentsCount)
        return;


    if (pomComponentsCount > maxComponentsCount)
    {
        maxComponents.resize(pomComponentsCount);
        maxComponentsCount = pomComponentsCount;
    }

    d_COMPONENTSCOUNT pomMinCount = pomComponentsCount < maxComponentsCount ? pomComponentsCount : maxComponentsCount;
    d_COMPONENTSCOUNT pomId = componentsCount <= 0 ? 0 : (components[componentsCount - 1])->getId();

    for (d_COMPONENTSCOUNT i = componentsCount; i < pomMinCount; i++)
    {
        if (maxComponents[i] == NULL)
        {
            d_COMPONENTSCOUNT pomId2 = pomId + 1;
            if (pomId2 <= 0)
            {
                maxComponents.resize(i);
                maxComponentsCount = i;

                std::stringstream pomErrorString;
                pomErrorString << "SystemDesign::setComponentsCount: Cannot add an component after the component with id = " << pomId << ".";
                paError.set(e_COMPONENT_ID_OVERFLOW, &pomErrorString);
                break;
            }
            pomId = pomId2;
            maxComponents[i] = ComponentDesign::createNew(pomId);
        }
        ComponentDesign *pomComponent = maxComponents[i];
        pomComponent->addComponentDesignStatesCountChangedEventHandler(this);
        pomComponent->addComponentDesignProbabilityChangedEventHandler(this);
        components.push_back(pomComponent);
    }

    if (pomComponentsCount < componentsCount)
        components.resize(pomComponentsCount);
    for (d_COMPONENTSCOUNT i = pomComponentsCount; i < componentsCount; i++)
    {
        ComponentDesign *pomComponent = maxComponents[i];
        pomComponent->removeAllComponentDesignStatesCountChangedEventHandler();
        pomComponent->removeAllComponentDesignProbabilityChangedEventHandler();
    }

    componentsCount = components.size();
    isSaved = false;

    systemDesignComponentsCountChangedEvent(*this, componentsCount);
}

void SystemDesign::clear()
{
    name.clear();

    structuralFunction->clear();

    d_COMPONENTSCOUNT pomComponentsCount = maxComponents.size();
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
        if (maxComponents[i] != NULL)
            delete maxComponents[i];

    maxComponents.resize(0);
    components.resize(0);

    maxComponentsCount = 0;
    componentsCount = 0;
    lastComponentsCount = 0;
    isSaved = true;
}

void SystemDesign::addSystemDesignComponentsCountChangedEventHandler(ISystemDesignComponentsCountChangedEventHandler *paEventHandler)
{
    if (paEventHandler != NULL)
        systemDesignComponentsCountChangedMap.insert(SystemDesignComponentsCountChangedMap::value_type(paEventHandler, paEventHandler));
}

void SystemDesign::removeSystemDesignComponentsCountChangedEventHandler(ISystemDesignComponentsCountChangedEventHandler *paEventHandler)
{
    if (paEventHandler != NULL)
        systemDesignComponentsCountChangedMap.erase(paEventHandler);
}

void SystemDesign::removeAllSystemDesignComponentsCountChangedEventHandler()
{
    systemDesignComponentsCountChangedMap.clear();
}

void SystemDesign::systemDesignComponentsCountChangedEvent(SystemDesign &paSystemDesign, d_COMPONENTSCOUNT paComponentsCount)
{
    for (SystemDesignComponentsCountChangedMap::iterator it = systemDesignComponentsCountChangedMap.begin();
         it != systemDesignComponentsCountChangedMap.end(); it++)
    {
        ISystemDesignComponentsCountChangedEventHandler *pomEventHandler = it->second;
        pomEventHandler->systemDesignComponentsCountChanged(paSystemDesign, paComponentsCount);
    }
}

bool SystemDesign::tryUpdateStructuralFunction(Error &paError)
{
    paError.reset();
    if (lastComponentsCount != components.size())
    {
        if (structuralFunction->trySet(components, paError))
        {
            lastComponentsCount = components.size();
            return true;
        }
        else
        {
            paError.prependText("SystemDesign::tryUpdateStructuralFunction: ");
            return false;
        }
    }
    else
    {
        if (!structuralFunction->tryUpdate(components, paError))
        {
            paError.prependText("SystemDesign::tryUpdateStructuralFunction: ");
            return false;
        }
        else
        {
            return true;
        }
    }
}

void SystemDesign::configurationNumberToStatesVector(d_SYSTEMCONFIGURATIONSCOUNT paConfigurationNumber, StateVector &paStatesVector)
{
    structuralFunction->systemConfigurationNumberToStatesVector(paConfigurationNumber, paStatesVector);
}

bool SystemDesign::tryLoadFromTxtStream(const std::string &paSystemName, std::istream &paTxtStream, Error &paError)
{
    std::stringstream pomErrorString;

    paError.reset();
    clear();

    name = paSystemName;
    if (paTxtStream.eof())
    {
        pomErrorString << "SystemDesign::tryLoadFromTxtStream: Reach end of file before getting components count.";
        paError.set(e_REACH_EOF, &pomErrorString);
        clear();
        return false;
    }
    paTxtStream >> maxComponentsCount;
    if (paTxtStream.fail())
    {
        pomErrorString << "SystemDesign::tryLoadFromTxtStream: Error when getting components count.";
        paError.set(e_BAD_VALUE, &pomErrorString);
        clear();
        return false;
    }

    if (maxComponentsCount < 0 || maxComponentsCount > c_MAX_COMPONENTS_COUNT)
    {
        if (maxComponentsCount < 0)
            pomErrorString << "SystemDesign::tryLoadFromTxtStream: Components count = " << maxComponentsCount << ".";
        else
            pomErrorString << "SystemDesign::tryLoadFromTxtStream: Too much components (components count = " << maxComponentsCount <<
                              " is more than " << c_MAX_COMPONENTS_COUNT << ").";
        paError.set(e_COMPONENTS_COUNT_IS_INVALID, &pomErrorString);
        clear();
        return false;
    }

    ComponentDesignsMap componentsMap;
    for (d_COMPONENTSCOUNT i = 0; i < maxComponentsCount; i++)
    {
        ComponentDesign *pomComponent = ComponentDesign::createNew(i+1, paTxtStream, true, paError);
        if (paError)
        {
            paError.prependText("SystemDesign::tryLoadFromTxtStream: ");
            break;
        }
        pomComponent->addComponentDesignStatesCountChangedEventHandler(this);
        pomComponent->addComponentDesignProbabilityChangedEventHandler(this);
        d_COMPONENTSCOUNT pomComponentID = pomComponent->getId();

        ComponentDesignsMap::iterator pomIt = componentsMap.find(pomComponentID);
        if (pomIt != componentsMap.end())
        {
            pomErrorString << "SystemDesign::tryLoadFromTxtStream: System already contains component with id = " <<
                              pomComponentID << ".";
            paError.set(e_COMPONENT_ALREADY_EXISTS, &pomErrorString);
            delete pomComponent;
            break;
        }
        componentsMap.insert(ComponentDesignsMap::value_type(pomComponentID, pomComponent));
    }

    if (paError)
    {
        clear();
        for (ComponentDesignsMap::iterator it = componentsMap.begin(); it != componentsMap.end(); it++)
        {
            ComponentDesign *pomComponent = it->second;
            if (pomComponent != NULL)
                delete pomComponent;
        }
        return false;
    }

    maxComponents.resize(maxComponentsCount, NULL);
    d_COMPONENTSCOUNT pomI = 0;
    for (ComponentDesignsMap::iterator it = componentsMap.begin(); it != componentsMap.end(); it++)
    {
        maxComponents[pomI] = it->second;
        pomI++;
    }

    componentsCount = maxComponentsCount;
    components = maxComponents;
    lastComponentsCount = maxComponentsCount;

    StructuralFunctionDesign *pomStructuralFunction = StructuralFunctionDesign::createNew(paTxtStream, components, paError);
    if (paError)
    {
        paError.prependText("SystemDesign::tryLoadFromTxtStream: ");
        clear();
        return false;
    }
    delete structuralFunction;
    structuralFunction = pomStructuralFunction;
    isSaved = true;

    return true;
}

bool SystemDesign::trySaveToTxtStream(const std::string &paSystemName, std::ostream &paTxtStream, Error &paError)
{
    paError.reset();

    if (!structuralFunction->isSet(components))
    {
        std::stringstream pomErrorString;
        pomErrorString << "SystemDesign::trySaveToTxtStream: Structural function is not updated.";
        paError.set(e_STRUCTURAL_FUNCTION_IS_NOT_UPDATED, &pomErrorString);

        return false;
    }

    paTxtStream << componentsCount << std::endl;
    for (d_COMPONENTSCOUNT i = 0; i < componentsCount; i++)
        components[i]->saveToTxtStream(paTxtStream, true);

    structuralFunction->saveToTxtStream(paTxtStream);
    name = paSystemName;
    isSaved = true;

    return true;
}

bool SystemDesign::isStructuralFunctionUpdated()
{
    return structuralFunction->isSet(components);
}

void SystemDesign::setConfigurationState(d_SYSTEMCONFIGURATIONSCOUNT paConfigurationNumber, d_STATESCOUNT paState)
{
    structuralFunction->setSystemState(paConfigurationNumber, paState);
    isSaved = false;
}

bool SystemDesign::isStructuralFunctionNonDecreasing(d_SYSTEMCONFIGURATIONSCOUNT &paSystemConfigurationNumberDown, d_SYSTEMCONFIGURATIONSCOUNT &paSystemConfigurationNumberUp, Error &paError)
{
    paError.reset();

    if (!structuralFunction->isSet(components))
    {
        std::stringstream pomErrorString;
        pomErrorString << "SystemDesign::isStructuralFunctionNonDecreasing: Structural function is not updated.";
        paError.set(e_STRUCTURAL_FUNCTION_IS_NOT_UPDATED, &pomErrorString);

        paSystemConfigurationNumberDown = 0;
        paSystemConfigurationNumberUp = 0;
        return false;
    }

    return structuralFunction->isNonDecreasing(paSystemConfigurationNumberDown, paSystemConfigurationNumberUp);
}
