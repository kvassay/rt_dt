#ifndef DPLR_H
#define DPLR_H

#include <vector>

#include "Core/definitions.h"
#include "Core/error.h"

class DPLR
{
private:
    //d_SYSTEMCONFIGURATIONSCOUNT notZerosCount;
    d_SYSTEMCONFIGURATIONSCOUNT positiveDerivationsCount;
    d_SYSTEMCONFIGURATIONSCOUNT negativeDerivationsCount;
    d_SYSTEMCONFIGURATIONSCOUNT goodSystemConfigurationsCount;
    std::vector<d_LDVALUE> dplr;

    //d_PROBABILITY notZerosProbability;
    d_PROBABILITY positiveDerivationsProbability;
    d_PROBABILITY negativeDerivationsProbability;
    std::vector<d_PROBABILITY> probabilities;

    void reset();
public:
    DPLR();
    DPLR(std::vector<d_LDVALUE> &paDPLR, std::vector<d_PROBABILITY> &paDPLRProbabilities, d_SYSTEMCONFIGURATIONSCOUNT paGoodSystemConfigurationsCount, Error &paError);

    d_SYSTEMCONFIGURATIONSCOUNT getSize() { return dplr.size(); }
    /*d_SYSTEMCONFIGURATIONSCOUNT getNotZerosCount() { return notZerosCount; }
    d_SYSTEMCONFIGURATIONSCOUNT getZerosCount() { return dplr.size() - notZerosCount; }*/
    d_SYSTEMCONFIGURATIONSCOUNT getPositiveDerivationsCount() { return positiveDerivationsCount; }
    d_SYSTEMCONFIGURATIONSCOUNT getNegativeDerivationsCount() { return negativeDerivationsCount; }

    d_SYSTEMCONFIGURATIONSCOUNT getGoodSystemConfigurationsCount() { return goodSystemConfigurationsCount; }

    /*d_PROBABILITY getNotZerosPorbability() { return notZerosProbability; }
    d_PROBABILITY getZerosProbability() { return 1 - notZerosProbability; }*/
    d_PROBABILITY getPositiveDerivationsProbability() { return positiveDerivationsProbability; }
    d_PROBABILITY getNegativeDerivationsProbability() { return negativeDerivationsProbability; }

    void set(std::vector<d_LDVALUE> &paDPLR, std::vector<d_PROBABILITY> &paDPLRProbabilities, d_SYSTEMCONFIGURATIONSCOUNT paGoodSystemConfigurationsCount, Error &paError);
    /*bool tryGetValue(d_SYSTEMCONFIGURATIONSCOUNT paIndex, bool &paValue)
    {
        if (paIndex >= 0 && paIndex < dplr.size())
        {
            paValue = dplr[paIndex];
            return true;
        }
        else
            return false;
    }
    bool getValue(d_SYSTEMCONFIGURATIONSCOUNT paIndex, bool &paOK)
    {
        if (paIndex >= 0 && paIndex < dplr.size())
        {
            paOK = true;
            return dplr[paIndex];
        }
        else
        {
            paOK = false;
            return 0;
        }
    }*/
};

#endif // DPLR_H
