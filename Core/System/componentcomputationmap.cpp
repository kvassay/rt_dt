#include "componentcomputationmap.h"

#include <string>

#include "Core/System/component.h"
#include "Core/System/system.h"

ComponentComputationMap::ComponentComputationMap(System &paSystem, Level paLevel)
{
    ComponentsMap pomComponentsMap;
    paSystem.getComponents(pomComponentsMap);
    std::vector<ComponentMeasures::Type> pomMeasuresVector;
    ComponentMeasures::getAllTypes(pomMeasuresVector);

    d_COMPONENTSCOUNT pomComponentsCount = pomComponentsMap.size();
    componentsVector.resize(pomComponentsCount);
    d_COMPONENTSCOUNT k = 0;
    for (ComponentsMap::iterator it = pomComponentsMap.begin(); it != pomComponentsMap.end(); it++)
    {
        componentsIDMap.insert(ComponentsIDMap::value_type(it->second->getId(), k));
        componentsVector[k] = it->second->getId();
        k++;
    }

    d_MEASURESCOUNT pomMeasuresCount = pomMeasuresVector.size();
    measuresVector.resize(pomMeasuresCount);
    for (d_MEASURESCOUNT i = 0; i < pomMeasuresCount; i++)
    {
        measuresMap.insert(MeasuresMap::value_type(pomMeasuresVector[i], i));
        measuresVector[i] = pomMeasuresVector[i];
    }

    createComputationMap(pomComponentsCount, pomMeasuresCount, paLevel);
}

ComponentComputationMap::ComponentComputationMap(const ComponentComputationMap &paComponentComputationMap)
{
    componentsIDMap = paComponentComputationMap.componentsIDMap;
    measuresMap = paComponentComputationMap.measuresMap;
    componentsVector = paComponentComputationMap.componentsVector;
    measuresVector = paComponentComputationMap.measuresVector;

    if (paComponentComputationMap.computationMap != NULL)
    {
        createComputationMap(componentsVector.size(), measuresVector.size(), None);

        d_COMPONENTSCOUNT pomComponentsCount = componentsVector.size();
        d_MEASURESCOUNT pomMeasuresCount = measuresVector.size();
        Level **pomComputationMap = paComponentComputationMap.computationMap;
        for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
        {
            for (d_MEASURESCOUNT j = 0; j < pomMeasuresCount; j++)
            {
                computationMap[i][j] = pomComputationMap[i][j];
            }
        }
    }
    else
    {
        computationMap = NULL;
    }

}

ComponentComputationMap::~ComponentComputationMap()
{
    clear();
}

void ComponentComputationMap::createComputationMap(d_COMPONENTSCOUNT paComponentsCount, d_MEASURESCOUNT paMeasuresCount, Level paLevel)
{
    computationMap = new Level*[paComponentsCount];
    for (d_COMPONENTSCOUNT i = 0; i < paComponentsCount; i++)
    {
        computationMap[i] = new Level[paMeasuresCount];
        for (d_COMPONENTSCOUNT j = 0; j < paMeasuresCount; j++)
        {
            computationMap[i][j] = paLevel;
        }
    }
}

void ComponentComputationMap::clear()
{
    d_COMPONENTSCOUNT pomComponentsCount = componentsVector.size();
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
    {
        delete computationMap[i];
    }
    delete computationMap;
    computationMap = NULL;

    componentsIDMap.clear();
    measuresMap.clear();
    componentsVector.clear();
    measuresVector.clear();

}

void ComponentComputationMap::reset()
{
    if (computationMap != NULL)
    {
        d_COMPONENTSCOUNT pomComponentsCount = componentsVector.size();
        d_MEASURESCOUNT pomMeasuresCount = measuresVector.size();

        for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
        {
            computationMap[i] = new Level[pomMeasuresCount];
            memset(computationMap[i],None,pomMeasuresCount*sizeof(Level));
        }
    }
}

Level ComponentComputationMap::getLevel(d_COMPONENTSCOUNT paComponentID, ComponentMeasures::Type paMeasureType, Error &paError) const
{
    paError.reset();

    ComponentsIDMap::const_iterator pomIt = componentsIDMap.find(paComponentID);
    if (pomIt == componentsIDMap.end())
    {
        std::stringstream pomStr;
        pomStr << "ComponentComputationMap::getLevel: Component with ID = " << paComponentID << " does not exist.";
        paError.set(e_COMPONENT_NOT_EXISTS, &pomStr);
        return None;
    }

    MeasuresMap::const_iterator pomIt2 = measuresMap.find(paMeasureType);
    return computationMap[pomIt->second][pomIt2->second];
}

void ComponentComputationMap::setLevel(d_COMPONENTSCOUNT paComponentID, ComponentMeasures::Type paMeasureType, Level paLevel, Error &paError)
{
    paError.reset();

    ComponentsIDMap::iterator pomIt = componentsIDMap.find(paComponentID);
    if (pomIt == componentsIDMap.end())
    {
        std::stringstream pomStr;
        pomStr << "ComponentComputationMap::setLevel: Component with ID = " << paComponentID << " does not exist.";
        paError.set(e_COMPONENT_NOT_EXISTS, &pomStr);
        return;
    }

    MeasuresMap::iterator pomIt2 = measuresMap.find(paMeasureType);
    computationMap[pomIt->second][pomIt2->second] = paLevel;
}

void ComponentComputationMap::setLevelForAllComponents(ComponentMeasures::Type paMeasureType, Level paLevel, Error &paError)
{
    paError.reset();

    d_COMPONENTSCOUNT pomComponentsCount = componentsIDMap.size();
    MeasuresMap::iterator pomIt = measuresMap.find(paMeasureType);
    d_MEASURESCOUNT pomMeasureIndex = pomIt->second;
    for (d_COMPONENTSCOUNT i = 0; i < pomComponentsCount; i++)
    {
        computationMap[i][pomMeasureIndex] = paLevel;
    }
}
