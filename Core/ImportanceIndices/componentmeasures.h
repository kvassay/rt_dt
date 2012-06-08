#ifndef COMPONENTMEASURES_H
#define COMPONENTMEASURES_H

#include <map>
#include <vector>
#include <string>

#include "Core/definitions.h"
#include "Core/error.h"

class ComponentIndex;
class Component;

class ComponentMeasures
{
public:
    enum Type {RTStructural_v1, RTStructural_vF, DPLRStructural,
               RTStructuralModified_v1, RTStructuralModified_vF, DPLRStructuralModified,
               RTBirnbaum_v1, RTBirnbaum_v2, RTBirnbaum_vF, DPLRBirnbaum, RCBirnbaum,
               RTCriticality_v1, RTCriticality_vF, DPLRCriticality};

    typedef d_IMPORTANCEINDEX (*ComputeBaseIndex)(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

private:
    typedef ComponentIndex* (*CreateComponentIndexFunction)(Component &paComponent);
    typedef std::map< ComponentMeasures::Type, ComponentMeasures::CreateComponentIndexFunction > CreateComponentIndexFunctionsMap;
    typedef std::vector<ComponentMeasures::Type> TypesVector;

    static CreateComponentIndexFunctionsMap createComponentIndexMap;
    static TypesVector types;

    static void init();

    static ComponentIndex* createRTStructuralIndex_v1(Component &paComponent);
    static ComponentIndex* createRTStructuralModifiedIndex_v1(Component &paComponent);
    static ComponentIndex* createRTBirnbaumIndex_v1(Component &paComponent);
    static ComponentIndex* createRTBirnbaumIndex_v2(Component &paComponent);
    static ComponentIndex* createRTCriticalityIndex_v1(Component &paComponent);

    static ComponentIndex* createRTStructuralIndex_vF(Component &paComponent);
    static ComponentIndex* createRTStructuralModifiedIndex_vF(Component &paComponent);
    static ComponentIndex* createRTBirnbaumIndex_vF(Component &paComponent);
    static ComponentIndex* createRTCriticalityIndex_vF(Component &paComponent);

    static ComponentIndex* createDPLRStructuralIndex(Component &paComponent);
    static ComponentIndex* createDPLRStructuralModifiedIndex(Component &paComponent);
    static ComponentIndex* createDPLRBirnbaumIndex(Component &paComponent);
    static ComponentIndex* createDPLRCriticalityIndex(Component &paComponent);

    static ComponentIndex* createRCBirnbaumIndex(Component &paComponent);

public:
    static ComponentIndex* createComponentIndex(ComponentMeasures::Type paType, Component& paComponent);

    static d_IMPORTANCEINDEX computeBaseRTStructuralIndex_v1(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseRTStructuralModifiedIndex_v1(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseRTBirnbaumIndex_v1(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseRTBirnbaumIndex_v2(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseRTCriticalityIndex_v1(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

    static d_IMPORTANCEINDEX computeBaseRTStructuralIndex_vF(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseRTStructuralModifiedIndex_vF(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseRTBirnbaumIndex_vF(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseRTCriticalityIndex_vF(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

    static d_IMPORTANCEINDEX computeBaseDPLRStructuralIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseDPLRStructuralModifiedIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseDPLRBirnbaumIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);
    static d_IMPORTANCEINDEX computeBaseDPLRCriticalityIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);

    static d_IMPORTANCEINDEX computeBaseRCBirnbaumIndex(Component &paComponent, d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError);


    static d_MEASURESCOUNT getCount() { return types.size(); }
    static const std::vector<ComponentMeasures::Type>& getAllTypes();
    static void getAllTypes(std::vector<ComponentMeasures::Type> &paTypes);
    static std::string toString(ComponentMeasures::Type paType);
};

inline
const std::vector<ComponentMeasures::Type>& ComponentMeasures::getAllTypes()
{
    if (types.size() <= 0)
        init();
    return types;
}

inline
void ComponentMeasures::getAllTypes(std::vector<ComponentMeasures::Type> &paTypes)
{
    if (types.size() <= 0)
        init();
    paTypes = types;
}

inline
std::string ComponentMeasures::toString(ComponentMeasures::Type paType)
{
    std::string pomStr;
    switch(paType)
    {
    case RTStructural_v1:
        pomStr = "Structural (v1)";
        break;
    case RTStructural_vF:
        pomStr = "Structural";
        break;
    case RTStructuralModified_v1:
        pomStr = "Structural modified (v1)";
        break;
    case RTStructuralModified_vF:
        pomStr = "Structural modified";
        break;
    case RTBirnbaum_v1:
        pomStr = "Birnbaum (v1)";
        break;
    case RTBirnbaum_v2:
        pomStr = "Birnbaum (v2)";
        break;
    case RTBirnbaum_vF:
        pomStr = "Birnbaum";
        break;
    case RTCriticality_v1:
        pomStr = "Criticality (v1)";
        break;
    case RTCriticality_vF:
        pomStr = "Criticality";
        break;
    case DPLRStructural:
        pomStr = "Structural (DPLD)";
        break;
    case DPLRStructuralModified:
        pomStr = "Structural modified (DPLD)";
        break;
    case DPLRBirnbaum:
        pomStr = "Birnbaum (DPLD)";
        break;
    case DPLRCriticality:
        pomStr = "Criticality (DPLD)";
        break;
    case RCBirnbaum:
        pomStr = "Birnbaum (RC)";
        break;
    }
    return pomStr;
}

#endif // COMPONENTMEASURES_H
