#ifndef COMPONENTCOMPUTATIONMAP_H
#define COMPONENTCOMPUTATIONMAP_H

#include <map>
#include <vector>

#include "Core/definitions.h"
#include "Core/error.h"
#include "Core/ImportanceIndices/componentmeasures.h"
#include "Core/ImportanceIndices/enums.h"

class System;

class ComponentComputationMap
{
private:
    typedef std::map<d_COMPONENTSCOUNT, d_COMPONENTSCOUNT> ComponentsIDMap;
    typedef std::map<ComponentMeasures::Type, d_MEASURESCOUNT> MeasuresMap;
    typedef std::vector<d_COMPONENTSCOUNT> ComponentsVector;
    typedef std::vector<ComponentMeasures::Type> MeasuresVector;

    ComponentsIDMap componentsIDMap;
    MeasuresMap measuresMap;
    ComponentsVector componentsVector;
    MeasuresVector measuresVector;

    Level **computationMap;

    void createComputationMap(d_COMPONENTSCOUNT paComponentsCount, d_MEASURESCOUNT paMeasuresCount, Level paLevel);
    void clear();
public:
    ComponentComputationMap(System &paSystem, Level paLevel);
    ComponentComputationMap(const ComponentComputationMap &paComponentComputationMap);
    virtual ~ComponentComputationMap();

    virtual void reset();
    Level getLevel(d_COMPONENTSCOUNT paComponentID, ComponentMeasures::Type paMeasureType, Error &paError) const;
    void setLevel(d_COMPONENTSCOUNT paComponentID, ComponentMeasures::Type paMeasureType, Level paLevel, Error &paError);
    void setLevelForAllComponents(ComponentMeasures::Type paMeasureType, Level paLevel, Error &paError);
};

#endif // COMPONENTCOMPUTATIONMAP_H
