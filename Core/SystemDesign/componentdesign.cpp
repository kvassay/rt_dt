#include "componentdesign.h"

ComponentDesign::ComponentDesign(d_COMPONENTSCOUNT paId) : ComponentBase(paId)
{
    maxStatesCount = 0;
    setStatesCount(c_MIN_COMPONENT_STATES_COUNT);
}

ComponentDesign::ComponentDesign(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, Error &paError)
{
    paError.reset();

    maxStatesCount = 0;

    ComponentBase::tryLoadFromTxtStream(paComponentNumber, paTxtStream, paIncludeProbabilities, paError);
    if (!paError)
    {
        ComponentBase::checkStatesCount(paError);
    }
    if (paError)
    {
        paError.prependText("ComponentDesign::ComponentDesign: ");
    }
}

void ComponentDesign::setStatesCount(d_STATESCOUNT paStatesCount)
{
    d_STATESCOUNT pomOldStatesCount = ComponentBase::getStatesCount();
    d_STATESCOUNT pomNewStatesCount;

    if (paStatesCount < c_MIN_COMPONENT_STATES_COUNT)
        pomNewStatesCount = c_MIN_COMPONENT_STATES_COUNT;
    else if (paStatesCount > c_MAX_COMPONENT_STATES_COUNT)
        pomNewStatesCount = c_MAX_COMPONENT_STATES_COUNT;
    else
        pomNewStatesCount = paStatesCount;

    if (pomNewStatesCount == pomOldStatesCount)
        return;

    ComponentBase::setStatesCount(pomNewStatesCount);

    if (pomNewStatesCount > maxStatesCount)
    {
        maxProbabilities.resize(pomNewStatesCount);
        maxStatesCount = pomNewStatesCount;
    }

    d_STATESCOUNT pomMinCount = pomNewStatesCount < maxStatesCount ? pomNewStatesCount : maxStatesCount;
    for (d_STATESCOUNT i = pomOldStatesCount; i < pomMinCount; i++)
    {
        trySetProbability(i, maxProbabilities[i]);
    }

    componentDesignStatesCountChangedEvent(*this, pomNewStatesCount);
}

bool ComponentDesign::trySetProbability(d_STATESCOUNT paState, d_PROBABILITY paProbability)
{
    if (ComponentBase::trySetProbability(paState, paProbability))
    {
        maxProbabilities[paState] = paProbability;

        componentDesignProbabilityChangedEvent(*this, paState, paProbability);
        return true;
    }
    else
    {
        return false;
    }
}

void ComponentDesign::addComponentDesignStatesCountChangedEventHandler(IComponentDesignStatesCountChangedEventHandler *paEventHandler)
{
    if (paEventHandler != NULL)
        componentDesignStatesCountChangedMap.insert(ComponentDesignStatesCountChangedMap::value_type(paEventHandler, paEventHandler));
}

void ComponentDesign::removeComponentDesignStatesCountChangedEventHandler(IComponentDesignStatesCountChangedEventHandler *paEventHandler)
{
    if (paEventHandler != NULL)
        componentDesignStatesCountChangedMap.erase(paEventHandler);
}

void ComponentDesign::removeAllComponentDesignStatesCountChangedEventHandler()
{
    componentDesignStatesCountChangedMap.clear();
}

void ComponentDesign::addComponentDesignProbabilityChangedEventHandler(IComponentDesignProbabilityChangedEventHandler *paEventHandler)
{
    if (paEventHandler != NULL)
        componentDesignProbabilityChangedMap.insert(ComponentDesignProbabilityChangedMap::value_type(paEventHandler, paEventHandler));
}

void ComponentDesign::removeComponentDesignProbabilityChangedEventHandler(IComponentDesignProbabilityChangedEventHandler *paEventHandler)
{
    if (paEventHandler != NULL)
        componentDesignProbabilityChangedMap.erase(paEventHandler);
}

void ComponentDesign::removeAllComponentDesignProbabilityChangedEventHandler()
{
    componentDesignProbabilityChangedMap.clear();
}

void ComponentDesign::componentDesignStatesCountChangedEvent(ComponentDesign &paComponentDesign, d_STATESCOUNT paStatesCount)
{
    for (ComponentDesignStatesCountChangedMap::iterator it = componentDesignStatesCountChangedMap.begin();
         it != componentDesignStatesCountChangedMap.end(); it++)
    {
        IComponentDesignStatesCountChangedEventHandler *pomEventHandler = it->second;
        pomEventHandler->componentDesignStatesCountChanged(paComponentDesign, paStatesCount);
    }
}

void ComponentDesign::componentDesignProbabilityChangedEvent(ComponentDesign &paComponentDesign, d_STATESCOUNT paState, d_PROBABILITY paProbability)
{
    for (ComponentDesignProbabilityChangedMap::iterator it = componentDesignProbabilityChangedMap.begin();
         it != componentDesignProbabilityChangedMap.end(); it++)
    {
        IComponentDesignProbabilityChangedEventHandler *pomEventHandler = it->second;
        pomEventHandler->componentDesignProbabilityChanged(paComponentDesign, paState, paProbability);
    }
}

ComponentDesign* ComponentDesign::createNew(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, Error &paError)
{
    paError.reset();
    ComponentDesign *pomResult = new ComponentDesign(paComponentNumber, paTxtStream, paIncludeProbabilities, paError);
    if (paError)
    {
        paError.prependText("ComponentDesign::createNew: ");

        delete pomResult;
        pomResult = NULL;
    }

    return pomResult;
}

ComponentDesign* ComponentDesign::createNew(d_COMPONENTSCOUNT paComponentNumber)
{
    ComponentDesign *pomResult = new ComponentDesign(paComponentNumber);
    return pomResult;
}
