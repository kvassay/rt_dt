#ifndef STRUCTURALFUNCTIONDESIGN_H
#define STRUCTURALFUNCTIONDESIGN_H

#include <vector>

#include "Core/Utilities/vectorwithbitmap.h"
#include "Core/definitions.h"
#include "Core/error.h"
#include "Core/SystemDesign/componentdesign.h"

class StructuralFunctionDesign
{
private:
    d_STATESCOUNT statesCount;
    d_SYSTEMCONFIGURATIONSCOUNT systemConfigurationsCount;

    StateVector *systemConfigurationsStates;
    std::vector<d_PROBABILITY> *systemConfigurationsProbabilities;
    std::vector<d_SYSTEMCONFIGURATIONSCOUNT> *systemStatesCounts;
    StateVector *componentsStatesCounts;
    std::vector<d_SYSTEMCONFIGURATIONSCOUNT> *multipleComponentsStatesCounts;

    void defaultInit();
    d_SYSTEMCONFIGURATIONSCOUNT computeSystemConfigurationsCount(ComponentDesignsVector &paComponents,
                                                                 StateVector &paComponentsStatesCountsVector,
                                                                 std::vector<d_SYSTEMCONFIGURATIONSCOUNT> &paMultipleComponentsStatesCountsVector,
                                                                 Error &paError);

protected:
    StructuralFunctionDesign();
    StructuralFunctionDesign(std::istream &paTxtStream, ComponentDesignsVector &paComponents, Error &paError);
    virtual bool tryLoadFromTxtStream(std::istream &paTxtStream, ComponentDesignsVector &paComponents, Error &paError);

    void computeProbabilities(ComponentDesignsVector &paComponents);

    d_SYSTEMCONFIGURATIONSCOUNT toConfigNumber(StateVector &paStatesVector, std::vector<d_SYSTEMCONFIGURATIONSCOUNT> &paMultipleVector);
    void toStatesVector(d_SYSTEMCONFIGURATIONSCOUNT paConfigNumber, StateVector &paComponentsStatesCountsVector, StateVector &paStatesVector);

    void initStatesVector(StateVector &paStatesVector, StateVector &paStatesCountsVector);
    bool isStatesVectorOk(StateVector &paStatesVector, StateVector &paStatesCountsVector);
    void incrementStatesVector(StateVector &paStatesVector, StateVector &paStatesCountsVector);
public:
    static StructuralFunctionDesign* createNew(std::istream &paTxtStream, ComponentDesignsVector &paComponents, Error &paError);
    static StructuralFunctionDesign* createNew();

    virtual ~StructuralFunctionDesign();
    void clear();

    void systemConfigurationNumberToStatesVector(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationNumber, StateVector &paStatesVector);
    bool trySet(ComponentDesignsVector &paComponents, Error &paError);
    bool tryUpdate(ComponentDesignsVector &paComponents, Error &paError);
    bool isSet(ComponentDesignsVector &paComponents);
    bool isNonDecreasing(d_SYSTEMCONFIGURATIONSCOUNT &paSystemConfigurationNumberDown, d_SYSTEMCONFIGURATIONSCOUNT &paSystemConfigurationNumberUp);

    d_SYSTEMCONFIGURATIONSCOUNT getSystemConfigurationsCount() { return systemConfigurationsCount; }
    d_STATESCOUNT getSystemStatesCount() { return statesCount; }
    d_STATESCOUNT getSystemState(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex);
    d_PROBABILITY getSystemConfigurationProbability(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex);
    void setSystemState(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex, d_STATESCOUNT paState);

    virtual void saveToTxtStream(std::ostream &paTxtStream);
};


inline
d_STATESCOUNT StructuralFunctionDesign::getSystemState(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex)
{
    if (paSystemConfigurationIndex >= 0 && paSystemConfigurationIndex < systemConfigurationsCount)
    {
        return (*systemConfigurationsStates)[paSystemConfigurationIndex];
    }
    else
    {
        return 0;
    }
}

inline
d_PROBABILITY StructuralFunctionDesign::getSystemConfigurationProbability(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex)
{
    if (paSystemConfigurationIndex >= 0 && paSystemConfigurationIndex < systemConfigurationsCount)
    {
        return (*systemConfigurationsProbabilities)[paSystemConfigurationIndex];
    }
    else
    {
        return 0;
    }
}


#endif // STRUCTURALFUNCTIONDESIGN_H
