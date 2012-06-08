#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <string>
#include <map>

#include "Core/definitions.h"
#include "Core/error.h"
#include "Core/Utilities/vectorwithbitmap.h"
#include "Core/System/component.h"
#include "Core/System/structuralfunction.h"
#include "Core/System/dplr.h"
#include "Core/System/componentcomputationmap.h"

class System
{
public:

private:
    bool equalReliability;
    std::string name;

    d_COMPONENTSCOUNT componentsCount;
    ComponentsMap componentsMap;
    ComponentsVector componentsVector;
    StructuralFunction* structuralFunction;

    StateVector statesCountsVector;
    std::vector<d_SYSTEMCONFIGURATIONSCOUNT> multipleStatesCountsVector;

    bool isInit;

    void updateOtherStructures();
    void initStatesVector(Component &paComponent, VectorWithBitmap<d_STATESCOUNT> &paStatesVector, d_COMPONENTSCOUNT &paFixedComponentIndex);
    void incrementStatesVector(VectorWithBitmap<d_STATESCOUNT> &paStatesVector);
    d_SYSTEMCONFIGURATIONSCOUNT getSystemConfigurationNumber(VectorWithBitmap<d_STATESCOUNT> &paStatesVector);
    d_PROBABILITY getProbability(VectorWithBitmap<d_STATESCOUNT> &paStatesVector, Error &paError);
    bool isSystemStateReliable(d_STATESCOUNT paSystemState, d_STATESCOUNT paReliableSystemState);
public:
    System(bool paEqualReliability);
    virtual ~System();
    void set(std::string &paSystemName, std::vector<Component*> &paComponents, StructuralFunction &paStructuralFunction, Error &paError);
    bool tryLoadFromTxtStream(const std::string &paSystemName, std::istream &paTxtStream, bool paIncludeProbabilities, Error &paError);
    void clear();

    bool hasEqualReliability() { return equalReliability; }

    d_STATESCOUNT getStatesCount() { return structuralFunction->getSystemStatesCount(); }
    d_SYSTEMCONFIGURATIONSCOUNT getConfigurationsCount() { return structuralFunction->getSystemConfigurationsCount(); }
    d_PROBABILITY getStateProbability(d_STATESCOUNT paSystemState, Error &paError);
    d_PROBABILITY getStateReliability(d_STATESCOUNT paSystemState, Error &paError);

    d_PROBABILITY calculateConditionalSystemReliability(Component &paComponent,
                                                        d_STATESCOUNT paComponentState,
                                                        d_STATESCOUNT paSystemState,
                                                        Error &paError);
    d_PROBABILITY calculateConditionalSystemConfigurationsCount(Component &paComponent,
                                                                d_STATESCOUNT paComponentState,
                                                                d_STATESCOUNT paSystemState,
                                                                Error &paError);
    bool tryCalculateCriticalPathVectorsCountAndProbability(Component &paComponent,
                                                            d_STATESCOUNT paComponentState,
                                                            d_STATESCOUNT paSystemState,
                                                            d_SYSTEMCONFIGURATIONSCOUNT &paCoherentCriticalPathVectorsCount,
                                                            d_PROBABILITY &paCoherentCriticalPathVectorsProbability,
                                                            d_SYSTEMCONFIGURATIONSCOUNT &paNonCoherentCriticalPathVectorsCount,
                                                            d_PROBABILITY &paNonCoherentCriticalPathVectorsProbability,
                                                            Error &paError);
    void calculateDPLR(Component &paComponent,
                       d_STATESCOUNT paComponentState,
                       d_STATESCOUNT paSystemState,
                       DPLR &paDPLR,
                       Error &paError);


    bool tryCalculate(Error &paError);
    bool tryCalculate(const ComponentComputationMap &paComponentComputationMap, Error &paError);

    void getComponents(ComponentsMap &paComponents);
    void getComponents(ComponentsVector &paComponents);
    d_COMPONENTSCOUNT getComponentsCount() { return componentsCount; }
    std::string getName() { return name; }

    void exportToCSVStream(Level paLevel, std::vector<ComponentMeasures::Type> &paMeasures, char paSeparator, std::ostream &paCSVStream);
};

#endif // SYSTEM_H
