#include "dplr.h"

#include <cmath>

DPLR::DPLR()
{
    reset();
}

DPLR::DPLR(std::vector<d_LDVALUE> &paDPLR, std::vector<d_PROBABILITY> &paDPLRProbabilities, d_SYSTEMCONFIGURATIONSCOUNT paGoodSystemConfigurationsCount, Error &paError)
{
    paError.reset();
    set(paDPLR, paDPLRProbabilities, paGoodSystemConfigurationsCount, paError);
    if (paError)
    {
        paError.prependText("DPLR::DPLR: ");
    }
}

void DPLR::reset()
{
    positiveDerivationsCount = 0;
    negativeDerivationsCount = 0;
    goodSystemConfigurationsCount = 0;
    dplr.resize(0);

    positiveDerivationsProbability = 0;
    negativeDerivationsProbability = 0;
    probabilities.resize(0);
}

void DPLR::set(std::vector<d_LDVALUE> &paDPLR, std::vector<d_PROBABILITY> &paDPLRProbabilities, d_SYSTEMCONFIGURATIONSCOUNT paGoodSystemConfigurationsCount, Error &paError)
{
    paError.reset();

    if (paDPLR.size() != paDPLRProbabilities.size())
    {
        std::stringstream pomStr;
        pomStr << "DPLR::set: Size of paDPLR vector = " << paDPLR.size() <<", but size of paDPLRProbabilities vector = " <<
                  paDPLRProbabilities.size() << ".";
        paError.set(e_DPLR_PROBLEM, &pomStr);

        reset();
        return;
    }

    goodSystemConfigurationsCount = paGoodSystemConfigurationsCount;

    d_SYSTEMCONFIGURATIONSCOUNT pomCount = paDPLR.size();
    d_PROBABILITY pomSumProbability = 0;
    d_PROBABILITY pomProbability = 0;

    positiveDerivationsCount = 0;
    negativeDerivationsCount = 0;
    positiveDerivationsProbability = 0;
    negativeDerivationsProbability = 0;
    dplr.resize(pomCount);
    probabilities.resize(pomCount);
    for (d_SYSTEMCONFIGURATIONSCOUNT i = 0; i < pomCount; i++)
    {
        dplr[i] = paDPLR[i];
        pomProbability = paDPLRProbabilities[i];
        probabilities[i] = pomProbability;
        if (dplr[i] < 0)
        {
            negativeDerivationsCount ++;
            negativeDerivationsProbability += pomProbability;
        }
        else if (dplr[i] > 0)
        {
            positiveDerivationsCount ++;
            positiveDerivationsProbability += pomProbability;
        }

        if (pomProbability < 0)
        {
            std::stringstream pomStr;
            pomStr << "DPLR::set: Probability " << i << " in paDPLRProbabilities vector is = " << pomProbability << ".";
            paError.set(e_PROBABILITY_IS_LESS_THAN_0, &pomStr);

            reset();
            return;
        }
        else if (pomProbability > 1)
        {
            std::stringstream pomStr;
            pomStr << "DPLR::set: Probability " << i << " in paDPLRProbabilities vector is = " << pomProbability << ".";
            paError.set(e_PROBABILITY_IS_GREATER_THAN_1, &pomStr);

            reset();
            return;
        }

        pomSumProbability += probabilities[i];
    }

    if (!CHECK_PROBABILITIES_SUM(pomSumProbability))
    {
        std::stringstream pomStr;
        pomStr << "DPLR::set: Sum of probabilities in paDPLRProbabilities vector is = " << pomSumProbability << ".";
        paError.set(e_PROBABILITIES_SUM_IS_NOT_1, &pomStr);

        reset();
    }
}

