#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "Core/SystemBase/componentbase.h"
#include "Core/definitions.h"
#include "Core/error.h"
#include "Core/System/dplr.h"
#include "Core/ImportanceIndices/componentmeasures.h"
#include "Core/ImportanceIndices/enums.h"
#include "Core/Utilities/matrixwithbitmap.h"

class ComponentIndex;
class Component;
class System;

class IComponentIndexChangedEventHandler
{
public:
    virtual ~IComponentIndexChangedEventHandler() { }
    virtual void componentIndexChanged(Component &paComponent, ComponentIndex* paComponentIndex) = 0;
};

typedef std::map<d_COMPONENTSCOUNT, Component*> ComponentsMap;
typedef std::vector<Component*> ComponentsVector;

class Component : public ComponentBase
{
private:
    typedef std::map< d_STATESCOUNT, DPLR* > DPLRBaseMap;
    typedef std::map< d_STATESCOUNT, DPLRBaseMap* > DPLRMap;
    typedef std::map< ComponentMeasures::Type, ComponentIndex* > ComponentIndexMap;
    typedef MatrixWithBitmap< d_PROBABILITY > ProbabilitiesMatrix;
    typedef MatrixWithBitmap< d_SYSTEMCONFIGURATIONSCOUNT > SystemCongigurationsMatrix;

    typedef std::map< IComponentIndexChangedEventHandler*, IComponentIndexChangedEventHandler* > IndexChangedEventHandlerBaseMap;
    typedef std::map< ComponentMeasures::Type, IndexChangedEventHandlerBaseMap* > IndexChangedEventHandlerMap;


    System *system;

    DPLRMap dplrMap;
    ProbabilitiesMatrix conditionalReliabilities;
    SystemCongigurationsMatrix reliabilities;
    SystemCongigurationsMatrix criticalPathVectorsCounts;
    ProbabilitiesMatrix criticalPathVectorsProbabilities;

    ComponentIndexMap componentIndexMap;

    IndexChangedEventHandlerMap indexChangedEventHandlerMap;

    void defaultInit();
    void componentIndexChangedEvent(ComponentMeasures::Type paComponentMeasureType, ComponentIndex* paComponentIndex);
protected:
    Component(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, System &paSystem, Error &paError);
    Component(d_COMPONENTSCOUNT paId, d_STATESCOUNT paStatesCount, System &paSystem, Error &paError);
    Component(d_COMPONENTSCOUNT paId, std::vector<d_PROBABILITY> &paProbabilites, System &paSystem, Error &paError);
public:
    static Component* createNew(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, System &paSystem, Error &paError);
    static Component* createNew(d_COMPONENTSCOUNT paId, d_STATESCOUNT paStatesCount, System &paSystem, Error &paError);
    static Component* createNew(d_COMPONENTSCOUNT paId, std::vector<d_PROBABILITY> &paProbabilites, System &paSystem, Error &paError);

    virtual ~Component();

    void resetDataStructures();

    bool tryGetProbability(d_STATESCOUNT paComponentState, d_PROBABILITY &paProbability)
    {
        Error pomError;
        paProbability = getProbability(paComponentState, pomError);
        return (pomError);
    }
    d_PROBABILITY getProbability(d_STATESCOUNT paComponentState, Error &paError);

    void computeComponentIndex(ComponentMeasures::Type paType, Level paLevel, Error &paError);
    ComponentIndex* getComponentIndex(ComponentMeasures::Type paType, Error &paError);

    System* getSystem() { return system; }

    d_PROBABILITY getConditionalSystemReliability(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    d_SYSTEMCONFIGURATIONSCOUNT getConditionalSystemConfigurationsCount(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    d_SYSTEMCONFIGURATIONSCOUNT getConditionalSystemConfigurationsCount();

    d_SYSTEMCONFIGURATIONSCOUNT getCriticalPathVectorsCount(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    d_PROBABILITY getCriticalPathVectorsProbability(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

    void setDPLR(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, DPLR *paDPLR);
    DPLR* getDPLR(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

    void addComponentIndexChangedEventHandler(ComponentMeasures::Type paComponentMeasureType, IComponentIndexChangedEventHandler *paEventHandler);
    void removeComponentIndexChangedEventHandler(ComponentMeasures::Type paComponentMeasureType, IComponentIndexChangedEventHandler *paEventHandler);

    void exportToCSVStream(Level paLevel, std::vector<ComponentMeasures::Type> &paMeasures, char paSeparator, std::ostream &paCSVStream);
};

#endif // COMPONENT_H
