#include "rcbirnbaumindex.h"

RCBirnbaumIndex::RCBirnbaumIndex(Component &paComponent) : ComponentIndex(paComponent)
{

}

d_IMPORTANCEINDEX RCBirnbaumIndex::computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError)
{
    paError.reset();
    Component *pomComponent = getComponent();
    d_IMPORTANCEINDEX pomResult = ComponentMeasures::computeBaseRCBirnbaumIndex(*pomComponent, paComponentState, paSystemState, paError);

    if (paError)
    {
        paError.prependText("RCBirnbaumIndex::computeBaseIndex: ");
    }

    return pomResult;
}

void RCBirnbaumIndex::computeDataStructures(Level paLevel, ImportanceIndex &paIndex, ImportanceIndex &paIndexSum,
                                            ComponentIndex::IndicesVector &paComponentStatesIndicesSum, ComponentIndex::IndicesVector &paComponentStatesIndicesAvg,
                                            ComponentIndex::IndicesVector &paSystemStatesIndicesSum, ComponentIndex::IndicesVector &paSystemStatesIndicesAvg,
                                            ComponentIndex::IndicesMatrix &paIndicesMatrix, Error &paError)
{
    paError.reset();

    if (paLevel == None)
        return;

    d_STATESCOUNT pomComponentStatesCount = getComponentStatesCount();
    d_STATESCOUNT pomSystemStatesCount = getSystemStatesCount();

    if (paLevel > First)
    {
        for (d_STATESCOUNT i = 0; i < pomComponentStatesCount; i++)
        {
            paComponentStatesIndicesSum[i] = new ImportanceIndex(0);
            paComponentStatesIndicesAvg[i] = new ImportanceIndex(0);
        }

        paSystemStatesIndicesSum[0] = NULL;
        paSystemStatesIndicesAvg[0] = NULL;
        for (d_STATESCOUNT j = 1; j < pomSystemStatesCount; j++)
        {
            paSystemStatesIndicesSum[j] = new ImportanceIndex(0);
            paSystemStatesIndicesAvg[j] = new ImportanceIndex(0);
        }
    }

    paIndex.setValue(0);
    paIndexSum.setValue(0);
    for (d_STATESCOUNT i = 0; i < pomComponentStatesCount; i++)
    {
        for (d_STATESCOUNT j = 1; j < pomSystemStatesCount; j++)
        {
            d_IMPORTANCEINDEX pomIndexValue = computeBaseIndex(i, j, paError);
            ImportanceIndex *pomIndex;
            if (!paError)
            {
                pomIndex = new ImportanceIndex(pomIndexValue);
            }
            else
            {
                paError.prependText("RCBirnbaumIndex::computeDataStructures: ");
                if (paError == e_IMPORTANCE_INDEX_IS_NOT_DEFINED)
                {
                    pomIndex = new ImportanceIndex();
                }
                else
                {
                    return;
                }
            }

            paIndex += *pomIndex;
            if (paLevel > First)
            {
                *(paComponentStatesIndicesSum[i]) += *pomIndex;
                *(paSystemStatesIndicesSum[j]) += *pomIndex;
            }
            if (paLevel > Second)
            {
                paIndicesMatrix.trySet(i, j, pomIndex);
            }
        }
    }

    if (paLevel > First)
    {
        for (d_STATESCOUNT i = 0; i < pomComponentStatesCount; i++)
        {
            *(paComponentStatesIndicesAvg[i]) = *(paComponentStatesIndicesSum[i]) / (pomComponentStatesCount-1);
        }
        for (d_STATESCOUNT j = 1; j < pomSystemStatesCount; j++)
        {
            *(paSystemStatesIndicesAvg[j]) = *(paSystemStatesIndicesSum[j]) / (pomSystemStatesCount-1);
        }
    }

    paIndexSum = paIndex / (pomComponentStatesCount-1);
    paIndex /= ((pomComponentStatesCount-1) * (pomSystemStatesCount-1));
}
