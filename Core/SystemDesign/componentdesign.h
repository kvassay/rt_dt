#ifndef COMPONENTDESIGN_H
#define COMPONENTDESIGN_H

#include <string>
#include <vector>
#include <map>

#include "Core/SystemBase/componentbase.h"
#include "Core/definitions.h"
#include "Core/error.h"

class ComponentDesign;

class IComponentDesignStatesCountChangedEventHandler
{
public:
    virtual ~IComponentDesignStatesCountChangedEventHandler() { }
    virtual void componentDesignStatesCountChanged(ComponentDesign &paComponentDesign, d_STATESCOUNT paStatesCount) = 0;
};

class IComponentDesignProbabilityChangedEventHandler
{
public:
    virtual ~IComponentDesignProbabilityChangedEventHandler() { }
    virtual void componentDesignProbabilityChanged(ComponentDesign &paComponentDesign, d_STATESCOUNT paState, d_PROBABILITY paProbability) = 0;
};

typedef std::vector<ComponentDesign*> ComponentDesignsVector;
typedef std::map<d_STATESCOUNT, ComponentDesign*> ComponentDesignsMap;

class ComponentDesign : public ComponentBase
{
private:
    typedef std::map< IComponentDesignStatesCountChangedEventHandler*, IComponentDesignStatesCountChangedEventHandler* > ComponentDesignStatesCountChangedMap;
    typedef std::map< IComponentDesignProbabilityChangedEventHandler*, IComponentDesignProbabilityChangedEventHandler* > ComponentDesignProbabilityChangedMap;

    ComponentDesignStatesCountChangedMap componentDesignStatesCountChangedMap;
    ComponentDesignProbabilityChangedMap componentDesignProbabilityChangedMap;

    d_STATESCOUNT maxStatesCount;
    std::vector<d_PROBABILITY> maxProbabilities;

    void componentDesignStatesCountChangedEvent(ComponentDesign &paComponentDesign, d_STATESCOUNT paStatesCount);
    void componentDesignProbabilityChangedEvent(ComponentDesign &paComponentDesign, d_STATESCOUNT paState, d_PROBABILITY paProbability);
protected:
    ComponentDesign(d_COMPONENTSCOUNT paId);
    ComponentDesign(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, Error &paError);
public:
    static ComponentDesign* createNew(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, Error &paError);
    static ComponentDesign* createNew(d_COMPONENTSCOUNT paId);

    virtual void setStatesCount(d_STATESCOUNT paStatesCount);
    virtual bool trySetProbability(d_STATESCOUNT paState, d_PROBABILITY paProbability);

    d_PROBABILITY getProbabilitiesSum() { return ComponentBase::getProbabilitiesSum(); }

    void addComponentDesignStatesCountChangedEventHandler(IComponentDesignStatesCountChangedEventHandler *paEventHandler);
    void removeComponentDesignStatesCountChangedEventHandler(IComponentDesignStatesCountChangedEventHandler *paEventHandler);
    void removeAllComponentDesignStatesCountChangedEventHandler();
    void addComponentDesignProbabilityChangedEventHandler(IComponentDesignProbabilityChangedEventHandler *paEventHandler);
    void removeComponentDesignProbabilityChangedEventHandler(IComponentDesignProbabilityChangedEventHandler *paEventHandler);
    void removeAllComponentDesignProbabilityChangedEventHandler();
};

#endif // COMPONENTDESIGN_H
