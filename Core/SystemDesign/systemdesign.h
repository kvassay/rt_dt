#ifndef SYSTEMDESIGN_H
#define SYSTEMDESIGN_H

#include <vector>
#include <string>
#include <map>

#include "Core/definitions.h"
#include "Core/error.h"
#include "Core/SystemDesign/componentdesign.h"
#include "Core/SystemDesign/structuralfunctiondesign.h"

class SystemDesign;

class ISystemDesignComponentsCountChangedEventHandler
{
public:
    virtual ~ISystemDesignComponentsCountChangedEventHandler() { }
    virtual void systemDesignComponentsCountChanged(SystemDesign &paSystemDesign, d_COMPONENTSCOUNT paComponentsCount) = 0;
};

class SystemDesign:
        IComponentDesignStatesCountChangedEventHandler,
        IComponentDesignProbabilityChangedEventHandler
{
private:
    typedef std::map< ISystemDesignComponentsCountChangedEventHandler*, ISystemDesignComponentsCountChangedEventHandler* > SystemDesignComponentsCountChangedMap;

    SystemDesignComponentsCountChangedMap systemDesignComponentsCountChangedMap;

    std::string name;
    bool isSaved;

    d_COMPONENTSCOUNT componentsCount;
    ComponentDesignsVector components;
    d_COMPONENTSCOUNT maxComponentsCount;
    ComponentDesignsVector maxComponents;
    d_COMPONENTSCOUNT lastComponentsCount;

    StructuralFunctionDesign *structuralFunction;

    void systemDesignComponentsCountChangedEvent(SystemDesign &paSystemDesign, d_COMPONENTSCOUNT paComponentsCount);

    virtual void componentDesignStatesCountChanged(ComponentDesign &paComponentDesign, d_STATESCOUNT paStatesCount) { isSaved = false; }
    virtual void componentDesignProbabilityChanged(ComponentDesign &paComponentDesign, d_STATESCOUNT paState, d_PROBABILITY paProbability) { isSaved = false; }
public:
    SystemDesign();
    virtual ~SystemDesign();

    ComponentDesign* getComponentByNumber(d_COMPONENTSCOUNT paComponentId);
    d_COMPONENTSCOUNT getComponentsCount() { return componentsCount; }
    void setComponentsCount(d_COMPONENTSCOUNT paComponentsCount, Error &paError);
    void clear();

    std::string getName() { return name; }
    d_STATESCOUNT getStatesCount() { return structuralFunction->getSystemStatesCount(); }
    d_SYSTEMCONFIGURATIONSCOUNT getConfigurationsCount() { return structuralFunction->getSystemConfigurationsCount(); }
    d_STATESCOUNT getConfigurationState(d_SYSTEMCONFIGURATIONSCOUNT paConfigurationNumber) { return structuralFunction->getSystemState(paConfigurationNumber); }
    void setConfigurationState(d_SYSTEMCONFIGURATIONSCOUNT paConfigurationNumber, d_STATESCOUNT paState);
    d_PROBABILITY getConfigurationProbability(d_SYSTEMCONFIGURATIONSCOUNT paConfigurationNumber) { return structuralFunction->getSystemConfigurationProbability(paConfigurationNumber); }
    void configurationNumberToStatesVector(d_SYSTEMCONFIGURATIONSCOUNT paConfigurationNumber, StateVector &paStatesVector);

    bool tryUpdateStructuralFunction(Error &paError);
    bool tryLoadFromTxtStream(const std::string &paSystemName, std::istream &paTxtStream, Error &paError);
    bool trySaveToTxtStream(const std::string &paSystemName, std::ostream &paTxtStream, Error &paError);

    bool isStructuralFunctionUpdated();
    bool isStructuralFunctionNonDecreasing(d_SYSTEMCONFIGURATIONSCOUNT &paSystemConfigurationNumberDown, d_SYSTEMCONFIGURATIONSCOUNT &paSystemConfigurationNumberUp, Error &paError);
    bool isChanged() { return !isSaved; }

    void addSystemDesignComponentsCountChangedEventHandler(ISystemDesignComponentsCountChangedEventHandler *paEventHandler);
    void removeSystemDesignComponentsCountChangedEventHandler(ISystemDesignComponentsCountChangedEventHandler *paEventHandler);
    void removeAllSystemDesignComponentsCountChangedEventHandler();
};

#endif // SYSTEMDESIGN_H
