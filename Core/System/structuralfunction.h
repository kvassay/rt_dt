#ifndef STRUCTURALFUNCTION_H
#define STRUCTURALFUNCTION_H

#include<vector>

#include "Core/definitions.h"
#include "Core/error.h"
#include "Core/System/component.h"

class StructuralFunction
{
private:
    bool equalReliability;

    d_STATESCOUNT statesCount;
    d_SYSTEMCONFIGURATIONSCOUNT systemConfigurationsCount;

    StateVector systemConfigurationsStates;
    std::vector<d_PROBABILITY> systemConfigurationsProbabilities;
    std::vector<d_PROBABILITY> systemStatesProbabilities;
    std::vector<d_PROBABILITY> systemStatesReliabilities;

    void defaultInit();
    void computeProbabilities(std::vector<Component*> &paComponents);
    void computeReliabilities();
protected:
    StructuralFunction(std::istream &paTxtStream, bool paEqualReliability, bool paComputeProbabilities, ComponentsMap &paComponents, Error &paError);
    StructuralFunction(StateVector &paSystemConfigurationsStates, bool paEqualReliability);

    virtual bool tryLoadFromTxtStream(std::istream &paTxtStream, bool paEqualReliability, bool paComputeProbabilities, ComponentsMap &paComponents, Error &paError);
public:
    static StructuralFunction* createNew(std::istream &paTxtStream, bool paEqualReliability, bool paComputeProbabilities, ComponentsMap &paComponents, Error &paError);
    static StructuralFunction* createNew(StateVector &paSystemConfigurationsStates, bool paEqualReliability, Error &paError);

    virtual ~StructuralFunction() { }

    d_SYSTEMCONFIGURATIONSCOUNT getSystemConfigurationsCount() { return systemConfigurationsCount; }
    const d_STATESCOUNT getSystemStatesCount() { return statesCount; }
    bool tryGetSystemState(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex, d_STATESCOUNT &paSystemConfigurationState);
    d_STATESCOUNT getSystemState(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex, bool &paOK);
    d_PROBABILITY getSystemConfigurationProbability(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex, bool &paOK);
    d_PROBABILITY getSystemStateProbability(d_STATESCOUNT paSystemState, Error &paError);
    d_PROBABILITY getSystemStateReliability(d_STATESCOUNT paSystemState, Error &paError);
    bool trySetProbabilities(ComponentsMap &paComponents);
    bool hasSetSystemConfigurationsProbabilities() { return systemConfigurationsProbabilities.size() > 0; }
};


inline
bool StructuralFunction::tryGetSystemState(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex, d_STATESCOUNT &paSystemConfigurationState)
{
    if (paSystemConfigurationIndex >= 0 && paSystemConfigurationIndex < systemConfigurationsCount)
    {
        paSystemConfigurationState = systemConfigurationsStates[paSystemConfigurationIndex];
        return true;
    }
    else
        return false;
}

inline
d_STATESCOUNT StructuralFunction::getSystemState(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex, bool &paOK)
{
    if (paSystemConfigurationIndex >= 0 && paSystemConfigurationIndex < systemConfigurationsCount)
    {
        paOK = true;
        return systemConfigurationsStates[paSystemConfigurationIndex];
    }
    else
    {
        paOK = false;
        return 0;
    }
}

inline
d_PROBABILITY StructuralFunction::getSystemConfigurationProbability(d_SYSTEMCONFIGURATIONSCOUNT paSystemConfigurationIndex, bool &paOK)
{
    if (paSystemConfigurationIndex >= 0 && paSystemConfigurationIndex < systemConfigurationsCount && systemConfigurationsProbabilities.size() > 0)
    {
        paOK = true;
        return systemConfigurationsProbabilities[paSystemConfigurationIndex];
    }
    else
    {
        paOK = false;
        return 0;
    }
}


#endif // STRUCTURALFUNCTION_H
