#include "component.h"

#include <sstream>
#include <string>

#include "Core/ImportanceIndices/componentindex.h"
#include "Core/System/system.h"

Component::Component(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, System &paSystem, Error &paError)
{
    paError.reset();
    defaultInit();

    system = &paSystem;

    ComponentBase::tryLoadFromTxtStream(paComponentNumber, paTxtStream, paIncludeProbabilities, paError);
    if (!paError)
    {
        if (ComponentBase::checkStatesCount(paError))
        {
            ComponentBase::checkProbabilities(paError);
        }
    }
    if (paError)
    {
        paError.prependText("Component::Component: ");
    }
}

Component::Component(d_COMPONENTSCOUNT paId, d_STATESCOUNT paStatesCount, System &paSystem, Error &paError)
    : ComponentBase(paId, paStatesCount)
{
    paError.reset();
    defaultInit();

    system = &paSystem;

    ComponentBase::checkStatesCount(paError);
    if (paError)
    {
        paError.prependText("Component::Component: ");
    }
}

Component::Component(d_COMPONENTSCOUNT paId, std::vector<d_PROBABILITY> &paProbabilites, System &paSystem, Error &paError)
    : ComponentBase(paId, paProbabilites)
{
    paError.reset();
    defaultInit();

    system = &paSystem;

    if (ComponentBase::checkStatesCount(paError))
        ComponentBase::checkProbabilities(paError);
    if (paError)
    {
        paError.prependText("Component::Component: ");
    }
}

void Component::defaultInit()
{
    system = NULL;

    componentIndexMap.clear();
    dplrMap.clear();
    conditionalReliabilities.clear();
    reliabilities.clear();

    criticalPathVectorsCounts.clear();
    criticalPathVectorsProbabilities.clear();
}


Component::~Component()
{
    resetDataStructures();
}

void Component::resetDataStructures()
{
    for(ComponentIndexMap::iterator it = componentIndexMap.begin(); it != componentIndexMap.end(); it++)
    {
        ComponentIndex *pomComponentIndex = it->second;
        if (pomComponentIndex != NULL)
        {
            delete pomComponentIndex;
            componentIndexChangedEvent(it->first, NULL);
        }
    }
    componentIndexMap.clear();

    for(DPLRMap::iterator it = dplrMap.begin(); it != dplrMap.end(); it++)
    {
        DPLRBaseMap* pomBaseMap = it->second;
        for(DPLRBaseMap::iterator it2 = pomBaseMap->begin(); it2 != pomBaseMap->end(); it2++)
        {
            delete it2->second;
        }
        delete pomBaseMap;
    }
    dplrMap.clear();

    conditionalReliabilities.clear();
    reliabilities.clear();

    criticalPathVectorsCounts.clear();
    criticalPathVectorsProbabilities.clear();
}

d_PROBABILITY Component::getProbability(d_STATESCOUNT paComponentState, Error &paError)
{
    paError.reset();
    if (paComponentState < 0 || paComponentState >= ComponentBase::getStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "Component::getProbability: Component with id = " << ComponentBase::getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    if (!ComponentBase::hasSetProbabilities())
    {
        std::stringstream pomStr;
        pomStr << "Component::getProbability: Component with id = " << ComponentBase::getId() << " has not set probability of state = " << paComponentState << ".";
        paError.set(e_PROBABILITY_IS_NOT_SET, &pomStr);
        return 0;
    }
    return ComponentBase::getProbability(paComponentState);
}

d_PROBABILITY Component::getConditionalSystemReliability(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    d_STATESCOUNT pomSystemStatesCount = system->getStatesCount();
    d_STATESCOUNT pomComponentStatesCount = ComponentBase::getStatesCount();

    paError.reset();
    if (paComponentState < 0 || paComponentState >= pomComponentStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getSystemConditionalReliability: Component with id = " << ComponentBase::getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    if (paSystemState < 0 || paSystemState >= pomSystemStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getSystemConditionalReliability: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return 0;
    }

    if (conditionalReliabilities.getRowsCount() == 0 || conditionalReliabilities.getColumnsCount() == 0)
    {
        conditionalReliabilities.resize(pomComponentStatesCount, pomSystemStatesCount);
    }

    d_PROBABILITY pomValue;
    if (!conditionalReliabilities.tryGet(paComponentState, paSystemState, pomValue))
    {
        pomValue = system->calculateConditionalSystemReliability(*this, paComponentState, paSystemState, paError);
        if (paError)
        {
            paError.prependText("Component::getSystemConditionalReliability: ");
        }
        else
            conditionalReliabilities.trySet(paComponentState, paSystemState, pomValue);
    }
    return pomValue;
}

d_SYSTEMCONFIGURATIONSCOUNT Component::getConditionalSystemConfigurationsCount(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    d_STATESCOUNT pomSystemStatesCount = system->getStatesCount();
    d_STATESCOUNT pomComponentStatesCount = ComponentBase::getStatesCount();

    paError.reset();
    if (paComponentState < 0 || paComponentState >= pomComponentStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getConditionalSystemConfigurationsCount: Component with id = " << ComponentBase::getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    if (paSystemState < 0 || paSystemState >= pomSystemStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getConditionalSystemConfigurationsCount: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return 0;
    }

    if (reliabilities.getRowsCount() == 0 || reliabilities.getColumnsCount() == 0)
    {
        reliabilities.resize(pomComponentStatesCount, pomSystemStatesCount);
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomValue;
    if (!reliabilities.tryGet(paComponentState, paSystemState, pomValue))
    {
        pomValue = system->calculateConditionalSystemConfigurationsCount(*this, paComponentState, paSystemState, paError);
        if (paError)
        {
            paError.prependText("Component::getConditionalSystemConfigurationsCount: ");
        }
        else
            reliabilities.trySet(paComponentState, paSystemState, pomValue);
    }
    return pomValue;
}

d_SYSTEMCONFIGURATIONSCOUNT Component::getConditionalSystemConfigurationsCount()
{
    d_SYSTEMCONFIGURATIONSCOUNT pomValue = (d_SYSTEMCONFIGURATIONSCOUNT)(system->getConfigurationsCount()) / (d_SYSTEMCONFIGURATIONSCOUNT)ComponentBase::getStatesCount();
    return pomValue;
}

d_SYSTEMCONFIGURATIONSCOUNT Component::getCriticalPathVectorsCount(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    d_STATESCOUNT pomSystemStatesCount = system->getStatesCount();
    d_STATESCOUNT pomComponentStatesCount = ComponentBase::getStatesCount();

    paError.reset();
    if (paComponentState < 0 || paComponentState >= pomComponentStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getCriticalPathVectorsCount: Component with id = " << ComponentBase::getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    if (paSystemState < 0 || paSystemState >= pomSystemStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getCriticalPathVectorsCount: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return 0;
    }

    if (criticalPathVectorsCounts.getRowsCount() == 0 || criticalPathVectorsCounts.getColumnsCount() == 0)
    {
        criticalPathVectorsCounts.resize(pomComponentStatesCount, pomSystemStatesCount);
    }

    d_SYSTEMCONFIGURATIONSCOUNT pomCoherentCount;
    if (!criticalPathVectorsCounts.tryGet(paComponentState, paSystemState, pomCoherentCount))
    {
        d_PROBABILITY pomCoherentProbability;
        d_SYSTEMCONFIGURATIONSCOUNT pomNonCoherentCount;
        d_PROBABILITY pomNonCoherentProbability;
        if (system->tryCalculateCriticalPathVectorsCountAndProbability(*this, paComponentState, paSystemState,
                                                                       pomCoherentCount, pomCoherentProbability,
                                                                       pomNonCoherentCount, pomNonCoherentProbability, paError))
        {
            criticalPathVectorsCounts.trySet(paComponentState, paSystemState, pomCoherentCount);
            criticalPathVectorsProbabilities.trySet(paComponentState, paSystemState, pomCoherentProbability);
        }
        else
            paError.prependText("Component::getCriticalPathVectorsCount: ");
    }
    return pomCoherentCount;
}

d_PROBABILITY Component::getCriticalPathVectorsProbability(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    d_STATESCOUNT pomSystemStatesCount = system->getStatesCount();
    d_STATESCOUNT pomComponentStatesCount = ComponentBase::getStatesCount();

    paError.reset();
    if (paComponentState < 0 || paComponentState >= pomComponentStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getCriticalPathVectorsProbability: Component with id = " << ComponentBase::getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return 0;
    }
    if (paSystemState < 0 || paSystemState >= pomSystemStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getCriticalPathVectorsProbability: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return 0;
    }

    if (criticalPathVectorsProbabilities.getRowsCount() == 0 || criticalPathVectorsProbabilities.getColumnsCount() == 0)
    {
        criticalPathVectorsProbabilities.resize(pomComponentStatesCount, pomSystemStatesCount);
    }

    d_PROBABILITY pomCoherentProbability;
    if (!criticalPathVectorsProbabilities.tryGet(paComponentState, paSystemState, pomCoherentProbability))
    {
        d_SYSTEMCONFIGURATIONSCOUNT pomCoherentCount;
        d_SYSTEMCONFIGURATIONSCOUNT pomNonCoherentCount;
        d_PROBABILITY pomNonCoherentProbability;
        if (system->tryCalculateCriticalPathVectorsCountAndProbability(*this, paComponentState, paSystemState,
                                                                       pomCoherentCount, pomCoherentProbability,
                                                                       pomNonCoherentCount, pomNonCoherentProbability, paError))
        {
            criticalPathVectorsCounts.trySet(paComponentState, paSystemState, pomCoherentCount);
            criticalPathVectorsProbabilities.trySet(paComponentState, paSystemState, pomCoherentProbability);
        }
        else
            paError.prependText("Component::getCriticalPathVectorsProbability: ");
    }
    return pomCoherentProbability;
}

void Component::setDPLR(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, DPLR *paDPLR)
{
    DPLRMap::iterator pomIt = dplrMap.find(paComponentState);

    if (pomIt == dplrMap.end())
    {
        DPLRBaseMap* pomDPLRBaseMap = new DPLRBaseMap();
        pomDPLRBaseMap->insert(std::make_pair(paSystemState, paDPLR));
        dplrMap.insert(std::make_pair(paComponentState, pomDPLRBaseMap));
    }
    else
    {
        DPLRBaseMap::iterator pomIt2 = pomIt->second->find(paSystemState);
        if (pomIt2 != pomIt->second->end())
        {
            delete pomIt2->second;
            pomIt2->second = paDPLR;
        }
        else
        {
            pomIt->second->insert(std::make_pair(paSystemState, paDPLR));
        }
    }
}

DPLR* Component::getDPLR(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    DPLR* pomDPLR = NULL;
    d_STATESCOUNT pomComponentStatesCount = ComponentBase::getStatesCount();

    paError.reset();
    if (paComponentState < 0 || paComponentState >= pomComponentStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "Component::getDPLR: Component with id = " << ComponentBase::getId() << " has not state = " << paComponentState << ".";
        paError.set(e_COMPONENT_STATE_IS_INVALID, &pomStr);
        return NULL;
    }
    if (paSystemState < 0 || paSystemState >= system->getStatesCount())
    {
        std::stringstream pomStr;
        pomStr << "Component::getDPLR: System has not state = " << paSystemState << ".";
        paError.set(e_SYSTEM_STATE_IS_INVALID, &pomStr);
        return NULL;
    }

    DPLRMap::iterator pomIt = dplrMap.find(paComponentState);
    if (pomIt == dplrMap.end())
    {
        pomDPLR = new DPLR();
        system->calculateDPLR(*this, paComponentState, paSystemState, *pomDPLR, paError);

        if (paError)
        {
            paError.prependText("Component::getDPLR: ");

            delete pomDPLR;
            pomDPLR = NULL;
            return NULL;
        }

        DPLRBaseMap *pomDPLRBaseMap = new DPLRBaseMap();
        pomDPLRBaseMap->insert(std::make_pair(paSystemState, pomDPLR));
        dplrMap.insert(std::make_pair(paComponentState, pomDPLRBaseMap));
    }
    else
    {
        DPLRBaseMap::iterator pomIt2 = pomIt->second->find(paSystemState);
        if (pomIt2 == pomIt->second->end())
        {
            pomDPLR = new DPLR();
            system->calculateDPLR(*this, paComponentState, paSystemState, *pomDPLR, paError);

            if (paError != e_NO_ERROR)
            {
                paError.prependText("Component::getDPLR: ");

                delete pomDPLR;
                pomDPLR = NULL;
                return NULL;
            }

            pomIt->second->insert(std::make_pair(paSystemState, pomDPLR));
        }
        else
        {
            pomDPLR = pomIt2->second;
        }
    }

    return pomDPLR;
}

ComponentIndex* Component::getComponentIndex(ComponentMeasures::Type paType, Error &paError)
{
    paError.reset();
    ComponentIndexMap::iterator pomIt = componentIndexMap.find(paType);
    if (pomIt != componentIndexMap.end() && pomIt->second != NULL)
        return pomIt->second;
    else
    {
        std::stringstream pomStr;
        pomStr << "Component::getComponentIndex: ComponentIndex of type = " << paType <<
                  " is not computed.";
        paError.set(e_INDEX_IS_NOT_COMPUTED, &pomStr);
        return NULL;
    }
}

void Component::computeComponentIndex(ComponentMeasures::Type paType, Level paLevel, Error &paError)
{
    paError.reset();
    ComponentIndex *pomComponentIndex = ComponentMeasures::createComponentIndex(paType, *this);

    pomComponentIndex->compute(paLevel, paError);
    if (!paError)
    {
        ComponentIndexMap::iterator pomIt = componentIndexMap.find(paType);
        if (pomIt != componentIndexMap.end() && pomIt->second != NULL)
        {
            delete pomIt->second;
            pomIt->second = pomComponentIndex;
        }
        else
        {
            componentIndexMap.insert(ComponentIndexMap::value_type(paType, pomComponentIndex));
        }
    }
    else
    {
        paError.prependText("Component::computeComponentIndex: ");

        delete pomComponentIndex;
        pomComponentIndex = NULL;
    }
    componentIndexChangedEvent(paType, pomComponentIndex);
}

void Component::exportToCSVStream(Level paLevel, std::vector<ComponentMeasures::Type> &paMeasures, char paSeparator, std::ostream &paCSVStream)
{
    paCSVStream << "\"" << this->getName() << "\"";

    d_STATESCOUNT pomSystemStatesCount = system->getStatesCount();
    d_STATESCOUNT pomComponentStatesCount = ComponentBase::getStatesCount();
    d_MEASURESCOUNT pomCount = paMeasures.size();
    std::vector<ComponentIndex*> pomVector;
    pomVector.resize(pomCount);

    for (d_MEASURESCOUNT i = 0; i < pomCount; i++)
    {
        ComponentIndexMap::iterator pomIt = componentIndexMap.find(paMeasures[i]);
        if (pomIt != componentIndexMap.end())
        {
            pomVector[i] = pomIt->second;
        }
        else
        {
            pomVector[i] = NULL;
        }
    }

    if (paLevel == None)
    {
        paCSVStream << std::endl;
        return;
    }
    else if (paLevel == First)
    {
        for (d_MEASURESCOUNT i = 0; i < pomCount; i++)
        {
            ComponentIndex *pomIndex = pomVector[i];
            if (pomIndex != NULL)
            {
                const ImportanceIndex *pomImportanceIndex = pomIndex->getMainIndexAvg();
                if (pomImportanceIndex != NULL)
                    paCSVStream << paSeparator << "\"" << pomImportanceIndex << "\"";
                else
                    paCSVStream << paSeparator;
            }
            else
            {
                paCSVStream << paSeparator;
            }
        }
        paCSVStream << std::endl;
    }
    else if (paLevel > First)
    {
        for (d_MEASURESCOUNT i = 0; i < pomCount; i++)
        {
            for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                paCSVStream << paSeparator << "\"" << j << "\"";
            paCSVStream << paSeparator << "\"" << "Sum" << "\"" << paSeparator << "\"" << "Avg" << "\"" << paSeparator;

            if (i < pomCount-1)
                paCSVStream << paSeparator;
        }
        paCSVStream << std::endl;

        for (d_STATESCOUNT k = 0; k < pomComponentStatesCount; k++)
        {
            for (d_MEASURESCOUNT i = 0; i < pomCount; i++)
            {
                paCSVStream << "\"" << k << "\"" << paSeparator;

                ComponentIndex *pomIndex = pomVector[i];
                if (pomIndex != NULL)
                {
                    const ImportanceIndex *pomImportanceIndex;

                    if (paLevel > Second)
                    {
                        for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                        {
                            pomImportanceIndex = pomIndex->getBaseIndex(k, j);
                            if (pomImportanceIndex != NULL)
                                paCSVStream << "\"" << pomImportanceIndex << "\"" << paSeparator;
                            else
                                paCSVStream << paSeparator;
                        }
                    }
                    else
                    {
                        for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                            paCSVStream << paSeparator;
                    }

                    pomImportanceIndex = pomIndex->getComponentStateIndexSum(k);
                    if (pomImportanceIndex != NULL)
                        paCSVStream << "\"" << pomImportanceIndex << "\"";

                    paCSVStream << paSeparator;

                    pomImportanceIndex = pomIndex->getComponentStateIndexAvg(k);
                    if (pomImportanceIndex != NULL)
                        paCSVStream << "\"" << pomImportanceIndex << "\"";
                }
                else
                {
                    for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                        paCSVStream << paSeparator;
                    paCSVStream << paSeparator;
                }

                if (i < pomCount-1)
                    paCSVStream << paSeparator << paSeparator;
            }
            paCSVStream << std::endl;
        }

        for (d_MEASURESCOUNT i = 0; i < pomCount; i++)
        {
            paCSVStream << "Sum";
            ComponentIndex *pomIndex = pomVector[i];
            if (pomIndex != NULL)
            {
                for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                {
                    const ImportanceIndex *pomImportanceIndex = pomIndex->getSystemStateIndexSum(j);
                    if (pomImportanceIndex != NULL)
                        paCSVStream << paSeparator << "\"" << pomImportanceIndex << "\"";
                    else
                        paCSVStream << paSeparator;
                }
                paCSVStream << paSeparator << paSeparator;
            }
            else
            {
                for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                    paCSVStream << paSeparator;
                paCSVStream << paSeparator <<  paSeparator << paSeparator;
            }

            if (i < pomCount-1)
                paCSVStream << paSeparator << paSeparator;
        }
        paCSVStream << std::endl;

        for (d_MEASURESCOUNT i = 0; i < pomCount; i++)
        {
            paCSVStream << "Avg";
            ComponentIndex *pomIndex = pomVector[i];
            if (pomIndex != NULL)
            {
                for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                {
                    const ImportanceIndex *pomImportanceIndex = pomIndex->getSystemStateIndexAvg(j);
                    if (pomImportanceIndex != NULL)
                        paCSVStream << paSeparator << "\"" << pomImportanceIndex << "\"";
                    else
                        paCSVStream << paSeparator;
                }
                paCSVStream << paSeparator << "\"" << pomIndex->getMainIndexSum() << "\"" << paSeparator << "\"" << pomIndex->getMainIndexAvg() << "\"";
            }
            else
            {
                for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                    paCSVStream << paSeparator;
                paCSVStream << paSeparator <<  paSeparator << paSeparator;
            }

            if (i < pomCount-1)
                paCSVStream << paSeparator << paSeparator;
        }
        paCSVStream << std::endl;

        for (d_MEASURESCOUNT i = 0; i < pomCount; i++)
        {
            for (d_STATESCOUNT j = 0; j < pomSystemStatesCount; j++)
                paCSVStream << paSeparator;
            paCSVStream << paSeparator;

            if (i < pomCount-1)
                paCSVStream << paSeparator << paSeparator;
            paCSVStream << paSeparator;
        }
        paCSVStream << std::endl;
    }
}

Component* Component::createNew(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, System &paSystem, Error &paError)
{
    paError.reset();
    Component *pomResult = new Component(paComponentNumber, paTxtStream, paIncludeProbabilities, paSystem, paError);
    if (paError)
    {
        paError.prependText("Component::createNew: ");

        delete pomResult;
        pomResult = NULL;
    }

    return pomResult;
}

Component* Component::createNew(d_COMPONENTSCOUNT paId, d_STATESCOUNT paStatesCount, System &paSystem, Error &paError)
{
    paError.reset();
    Component *pomResult = new Component(paId, paStatesCount, paSystem, paError);
    if (paError)
    {
        paError.prependText("Component::createNew: ");

        delete pomResult;
        pomResult = NULL;
    }

    return pomResult;
}

Component* Component::createNew(d_COMPONENTSCOUNT paId, std::vector<d_PROBABILITY> &paProbabilites, System &paSystem, Error &paError)
{
    paError.reset();
    Component *pomResult = new Component(paId, paProbabilites, paSystem, paError);
    if (paError)
    {
        paError.prependText("Component::createNew: ");

        delete pomResult;
        pomResult = NULL;
    }

    return pomResult;
}

void Component::componentIndexChangedEvent(ComponentMeasures::Type paComponentMeasureType, ComponentIndex* paComponentIndex)
{
    IndexChangedEventHandlerMap::iterator pomIt = indexChangedEventHandlerMap.find(paComponentMeasureType);
    if (pomIt != indexChangedEventHandlerMap.end())
    {
        IndexChangedEventHandlerBaseMap *pomMap = pomIt->second;

        for (IndexChangedEventHandlerBaseMap::iterator it = pomMap->begin(); it != pomMap->end(); it++)
        {
            IComponentIndexChangedEventHandler *pomEventHandler = it->second;
            if (pomEventHandler != NULL)
            {
                pomEventHandler->componentIndexChanged(*this, paComponentIndex);
            }
        }
    }
}

void Component::addComponentIndexChangedEventHandler(ComponentMeasures::Type paComponentMeasureType, IComponentIndexChangedEventHandler *paEventHandler)
{
    if (paEventHandler == NULL)
        return;

    IndexChangedEventHandlerMap::iterator pomIt = indexChangedEventHandlerMap.find(paComponentMeasureType);
    IndexChangedEventHandlerBaseMap *pomMap;
    if (pomIt == indexChangedEventHandlerMap.end())
    {
        pomMap = new IndexChangedEventHandlerBaseMap();
        indexChangedEventHandlerMap.insert(IndexChangedEventHandlerMap::value_type(paComponentMeasureType, pomMap));
    }
    else
    {
        pomMap = pomIt->second;
    }
    pomMap->insert(IndexChangedEventHandlerBaseMap::value_type(paEventHandler, paEventHandler));
}

void Component::removeComponentIndexChangedEventHandler(ComponentMeasures::Type paComponentMeasureType, IComponentIndexChangedEventHandler *paEventHandler)
{
    if (paEventHandler == NULL)
        return;

    IndexChangedEventHandlerMap::iterator pomIt = indexChangedEventHandlerMap.find(paComponentMeasureType);
    IndexChangedEventHandlerBaseMap *pomMap;
    if (pomIt != indexChangedEventHandlerMap.end())
    {
        pomMap = pomIt->second;
        pomMap->erase(paEventHandler);

        if (pomMap->size() <= 0)
        {
            delete pomMap;
            indexChangedEventHandlerMap.erase(pomIt);
        }
    }
}

