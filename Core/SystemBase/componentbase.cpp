#include "componentbase.h"

ComponentBase::ComponentBase()
{
    init(0, 0);
}

ComponentBase::ComponentBase(d_COMPONENTSCOUNT paId)
{
    init(paId, 0);
}

ComponentBase::ComponentBase(d_COMPONENTSCOUNT paId, d_STATESCOUNT paStatesCount)
{
    init(paId, paStatesCount);
}

ComponentBase::ComponentBase(d_COMPONENTSCOUNT paId, std::vector<d_PROBABILITY> &paProbabilites)
{
    init(paId, paProbabilites.size());
    for (d_STATESCOUNT i = 0; i < statesCount; i++)
        trySetProbability(i, paProbabilites[i]);
}


void ComponentBase::setStatesCount(d_STATESCOUNT paStatesCount)
{
    d_STATESCOUNT pomStatesCount = paStatesCount; // < d_MIN_COMPONENT_STATES_COUNT ? d_MIN_COMPONENT_STATES_COUNT : paStatesCount;

    if (pomStatesCount == statesCount)
        return;

    if (pomStatesCount <= 0)
    {
        probabilities.resize(0);
        probabilitiesSum = 0;
    }
    else
    {
        probabilities.resize(pomStatesCount);
        probabilitiesSum = 0;
        for (d_STATESCOUNT i = 0; i < pomStatesCount; i++)
        {
            probabilitiesSum += probabilities[i];
        }
    }
    statesCount = pomStatesCount;
}

bool ComponentBase::trySetProbability(d_STATESCOUNT paState, d_PROBABILITY paProbability)
{
    if (paState >= 0 && paState < statesCount && probabilities[paState] != paProbability)
    {
        probabilitiesSum -= probabilities[paState];
        probabilities[paState] = paProbability;
        probabilitiesSum += paProbability;

        return true;
    }
    else
    {
        return false;
    }
}

d_PROBABILITY ComponentBase::getProbability(d_STATESCOUNT paState)
{
    if (paState >= 0 && paState < statesCount)
        return probabilities[paState];
    else
        return 0;
}

bool ComponentBase::checkStatesCount(Error &paError)
{
    paError.reset();
    if (statesCount < c_MIN_COMPONENT_STATES_COUNT || statesCount > c_MAX_COMPONENT_STATES_COUNT)
    {
        std::stringstream pomStr;
        pomStr << "ComponentBase::checkStatesCount: States count of component with id = " << id << " is: " << statesCount << ".";
        paError.set(e_STATES_COUNT_IS_NOT_VALID, &pomStr);
        return false;
    }
    return true;
}

bool ComponentBase::checkProbabilities(Error &paError)
{
    paError.reset();
    std::stringstream pomStr;

    if (probabilities.size() != statesCount)
    {
        pomStr << "ComponentBase::checkProbabilities: Count of probabilities of component with id = " << id << " is: "<<
                  probabilities.size() << " but count of states is: " << statesCount << ".";
        paError.set(e_PROBABILITIES_COUNT_IS_DIFEERENT_FROM_STATES_COUNT, &pomStr);
        return false;
    }

    if (probabilities.size() > 0)
    {
        for (d_STATESCOUNT i = 0; i < statesCount; i++)
        {
            if (probabilities[i] < 0)
            {
                pomStr << "ComponentBase::checkProbabilities: Probability of state " << i << " of component with id = " << id <<
                          " is: " << probabilities[i] << ".";
                paError.set(e_PROBABILITY_IS_LESS_THAN_0, &pomStr);
                return false;
            }
            else if (probabilities[i] > 1)
            {
                pomStr << "ComponentBase::checkProbabilities: Probability of state " << i << " of component with id = " << id <<
                          " is: " << probabilities[i] << ".";
                paError.set(e_PROBABILITY_IS_GREATER_THAN_1, &pomStr);
                return false;
            }
        }

        if (!CHECK_PROBABILITIES_SUM(probabilitiesSum))
        {
            pomStr << "ComponentBase::checkProbabilities: Sum of probabilities of component with id = " << id << " is: " << probabilitiesSum << ".";
            paError.set(e_PROBABILITIES_SUM_IS_NOT_1, &pomStr);
            return false;
        }
    }

    return true;
}

void ComponentBase::setName()
{
    std::stringstream pomStr;
    pomStr << "x" << id;
    name = pomStr.str();
}

bool ComponentBase::tryLoadFromTxtStream(d_COMPONENTSCOUNT paComponentNumber, std::istream &paTxtStream, bool paIncludeProbabilities, Error &paError)
{
    paError.reset();
    std::stringstream pomErrorString;

    d_COMPONENTSCOUNT pomNewId = id;
    d_STATESCOUNT pomNewStatesCount = statesCount;
    std::vector<d_PROBABILITY> pomNewProbabilities = probabilities;

    if (paTxtStream.eof())
    {
        pomErrorString << "ComponentBase::tryLoadFromTxtStream: Reach end of file before getting id of component with number = " << paComponentNumber << ".";
        paError.set(e_REACH_EOF, &pomErrorString);
        return false;
    }
    paTxtStream >> pomNewId;
    if (paTxtStream.fail())
    {
        pomErrorString << "ComponentBase::tryLoadFromTxtStream: Error when getting id of component with number = " << paComponentNumber << ".";
        paError.set(e_BAD_VALUE, &pomErrorString);
        return false;
    }

    if (paTxtStream.eof())
    {
        pomErrorString << "ComponentBase::tryLoadFromTxtStream: Reach end of file before getting number of states of component " <<
                          "with id = " << pomNewId << ".";
        paError.set(e_REACH_EOF, &pomErrorString);
        return false;
    }
    paTxtStream >> pomNewStatesCount;
    if (paTxtStream.fail())
    {
        pomErrorString << "ComponentBase::tryLoadFromTxtStream: Error when getting number of states of component " <<
                          "with id = " << pomNewId << ".";
        paError.set(e_BAD_VALUE, &pomErrorString);
        return false;
    }

    if (paIncludeProbabilities)
    {
        pomNewProbabilities.resize(pomNewStatesCount);
        for (d_STATESCOUNT j = 0; j < pomNewStatesCount; j++)
        {
            if (paTxtStream.eof())
            {
                pomErrorString << "ComponentBase::tryLoadFromTxtStream: Reach end of file before getting probability of state = " <<
                                  j << " of component with id = " << pomNewId << ".";
                paError.set(e_REACH_EOF, &pomErrorString);
                return false;
            }
            paTxtStream >> pomNewProbabilities[j];
            if (paTxtStream.fail())
            {
                pomErrorString << "ComponentBase::tryLoadFromTxtStream: Error when getting probability of state = " <<
                                  j << " of component with id = " << pomNewId << ".";
                paError.set(e_BAD_VALUE, &pomErrorString);
                return false;
            }
        }
    }

    init(pomNewId, pomNewStatesCount);
    if (paIncludeProbabilities)
    {
        for (d_STATESCOUNT i = 0; i < statesCount; i++)
            trySetProbability(i, pomNewProbabilities[i]);
    }

    return true;
}

void ComponentBase::init(d_COMPONENTSCOUNT paId, d_STATESCOUNT paStatesCount)
{
    id = paId;
    setName();
    setStatesCount(paStatesCount);

    probabilitiesSum = 0;
}

void ComponentBase::saveToTxtStream(std::ostream &paTxtStream, bool paIncludeProbabilities)
{
    paTxtStream << id << std::endl;
    paTxtStream << statesCount << std::endl;

    if (paIncludeProbabilities)
    {
        for (d_STATESCOUNT i = 0; i < statesCount; i++)
            paTxtStream << probabilities[i] << std::endl;
    }
}
