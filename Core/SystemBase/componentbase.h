#ifndef COMPONENTBASE_H
#define COMPONENTBASE_H

#include <string>
#include <vector>
#include <map>

#include "Core/definitions.h"
#include "Core/error.h"

class ComponentBase
{
private:
    std::string name;
    d_COMPONENTSCOUNT id;
    d_STATESCOUNT statesCount;
    std::vector<d_PROBABILITY> probabilities;
    d_PROBABILITY probabilitiesSum;

    void setName();
    void init(d_COMPONENTSCOUNT paId, d_STATESCOUNT paStatesCount);
protected:
    ComponentBase();

    void setId(d_STATESCOUNT paId) { id = paId; }

    bool tryLoadFromTxtStream(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, Error &paError);

    bool checkStatesCount(Error &paError);
    bool checkProbabilities(Error &paError);

    virtual void setStatesCount(d_STATESCOUNT paStatesCount);
    virtual bool trySetProbability(d_STATESCOUNT paState, d_PROBABILITY paProbability);

    d_PROBABILITY getProbabilitiesSum() { return probabilitiesSum; }
public:
    ComponentBase(d_COMPONENTSCOUNT paId);
    ComponentBase(d_COMPONENTSCOUNT paId, d_STATESCOUNT paStatesCount);
    ComponentBase(d_COMPONENTSCOUNT paId, std::vector<d_PROBABILITY> &paProbabilites);
    virtual ~ComponentBase() { }

    std::string getName() { return name; }
    d_COMPONENTSCOUNT getId() { return id; }
    d_STATESCOUNT getStatesCount() { return statesCount; }
    d_PROBABILITY getProbability(d_STATESCOUNT paState);
    bool hasSetProbabilities() { return statesCount == probabilities.size(); }

    virtual void saveToTxtStream(std::ostream &paTxtStream, bool paIncludeProbabilities);
};

#endif // COMPONENTBASE_H
