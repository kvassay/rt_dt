#include "componentindex.h"

ComponentIndex::ComponentIndex(Component &paComponent)
{
    component = &paComponent;
    computedLevel = None;

    importanceIndex.setNotDefined();
    importanceIndexSum.setNotDefined();
    componentStatesIndicesSum.resize(0);
    componentStatesIndicesAvg.resize(0);
    systemStatesIndicesSum.resize(0);
    systemStatesIndicesAvg.resize(0);
    indicesMatrix.resize(0,0);
}

ComponentIndex::~ComponentIndex()
{
    deleteDataStructures();
}

void ComponentIndex::deleteComponentStatesIndices()
{
    d_STATESCOUNT pomCount = componentStatesIndicesSum.size();
    ImportanceIndex* pomIndex;
    for (d_STATESCOUNT i = 0; i < pomCount; i++)
    {
        pomIndex = componentStatesIndicesSum[i];
        if (pomIndex != NULL)
        {
            delete pomIndex;
        }
    }
    componentStatesIndicesSum.clear();

    pomCount = componentStatesIndicesAvg.size();
    for (d_STATESCOUNT i = 0; i < pomCount; i++)
    {
        pomIndex = componentStatesIndicesAvg[i];
        if (pomIndex != NULL)
        {
            delete pomIndex;
        }
    }
    componentStatesIndicesAvg.clear();
}

void ComponentIndex::deleteSystemStatesIndices()
{
    d_STATESCOUNT pomCount = systemStatesIndicesSum.size();
    ImportanceIndex* pomIndex;
    for (d_STATESCOUNT i = 0; i < pomCount; i++)
    {
        pomIndex = systemStatesIndicesSum[i];
        if (pomIndex != NULL)
        {
            delete pomIndex;
        }
    }
    componentStatesIndicesSum.clear();

    pomCount = systemStatesIndicesAvg.size();
    for (d_STATESCOUNT i = 0; i < pomCount; i++)
    {
        pomIndex = systemStatesIndicesAvg[i];
        if (pomIndex != NULL)
        {
            delete pomIndex;
        }
    }
    componentStatesIndicesAvg.clear();
}

void ComponentIndex::deleteIndicesMatrix()
{
     bool pomOK;
     ImportanceIndex* pomBaseIndex;

     d_STATESCOUNT pomRowsCount = indicesMatrix.getRowsCount();
     d_STATESCOUNT pomColumnsCount = indicesMatrix.getColumnsCount();

     for (d_STATESCOUNT i = 0; i < pomRowsCount; i++)
     {
         for (d_STATESCOUNT j = 0; j < pomColumnsCount; j++)
         {
             pomBaseIndex = indicesMatrix.get(i, j ,pomOK);
             if (pomBaseIndex != NULL)
             {
                 delete pomBaseIndex;
                 pomBaseIndex = NULL;
             }
         }
     }
     indicesMatrix.clear();
}

void ComponentIndex::deleteDataStructures()
{
    importanceIndex.setNotDefined();
    importanceIndexSum.setNotDefined();
    deleteComponentStatesIndices();
    deleteSystemStatesIndices();
    deleteIndicesMatrix();
}

void ComponentIndex::createDataStructures(Level paLevel)
{
    deleteDataStructures();

    d_STATESCOUNT pomComponentStatesCount = getComponentStatesCount();
    d_STATESCOUNT pomSystemStatesCount = getSystemStatesCount();

    if (paLevel > First)
    {
        componentStatesIndicesSum.resize(pomComponentStatesCount);
        componentStatesIndicesAvg.resize(pomComponentStatesCount);
        systemStatesIndicesSum.resize(pomSystemStatesCount);
        systemStatesIndicesAvg.resize(pomSystemStatesCount);
    }
    if (paLevel > Second)
    {
        indicesMatrix.resize(pomComponentStatesCount, pomSystemStatesCount);
    }
}

bool ComponentIndex::checkBeforeUpdate(Error &paError)
{
    paError.reset();

    d_STATESCOUNT pomComponentStatesCount = getComponentStatesCount();
    if (indicesMatrix.getRowsCount() != pomComponentStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "ComponentIndex::checkBeforeUpdate: Number of component states of component with id = " << component->getId() <<
                  " is = " << pomComponentStatesCount << ", but count of rows of indicesMatrix is = " << indicesMatrix.getRowsCount() << ".";
        paError.set(e_COMPONENT_STATES_COUNT_IS_DIFFERENT, &pomStr);
        return false;
    }

    d_STATESCOUNT pomSystemStatesCount = getSystemStatesCount();
    if (indicesMatrix.getColumnsCount() != pomSystemStatesCount)
    {
        std::stringstream pomStr;
        pomStr << "ComponentIndex::checkBeforeUpdate: Number of system states is = " << pomSystemStatesCount << ", but count of columns of indicesMatrix is = " <<
                  indicesMatrix.getColumnsCount() << ".";
        paError.set(e_SYSTEM_STATES_COUNT_IS_DIFFERENT, &pomStr);
        return false;
    }

    return true;
}

void ComponentIndex::compute(Level paLevel, Error &paError)
{
    computedLevel = None;

    createDataStructures(paLevel);
    computeDataStructures(paLevel, importanceIndex, importanceIndexSum, componentStatesIndicesSum, componentStatesIndicesAvg,
                          systemStatesIndicesSum, systemStatesIndicesAvg, indicesMatrix, paError);

    if (paError)
    {
        paError.prependText("ComponentIndex::compute: ");
    }
    else
    {
        computedLevel = paLevel;
    }
}

void ComponentIndex::computeDataStructures(Level paLevel, ImportanceIndex &paIndex, ImportanceIndex &paIndexSum,
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
        paComponentStatesIndicesSum[0] = NULL;
        paComponentStatesIndicesAvg[0] = NULL;
        for (d_STATESCOUNT i = 1; i < pomComponentStatesCount; i++)
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
    for (d_STATESCOUNT i = 1; i < pomComponentStatesCount; i++)
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
                paError.prependText("ComponentIndex::computeDataStructures: ");
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
        for (d_STATESCOUNT i = 1; i < pomComponentStatesCount; i++)
        {
            *(paComponentStatesIndicesAvg[i]) = *(paComponentStatesIndicesSum[i])/(pomComponentStatesCount-1);
        }
        for (d_STATESCOUNT j = 1; j < pomSystemStatesCount; j++)
        {
            *(paSystemStatesIndicesAvg[j]) = *(paSystemStatesIndicesSum[j])/(pomSystemStatesCount-1);
        }
    }

    paIndexSum = paIndex / (pomComponentStatesCount-1);
    paIndex /= ((pomComponentStatesCount-1) * (pomSystemStatesCount-1));
}

const ImportanceIndex * ComponentIndex::getComponentStateIndexSum(d_STATESCOUNT paComponentState)
{
    if (computedLevel > First &&
        paComponentState >= 0 && paComponentState < componentStatesIndicesSum.size())
        return componentStatesIndicesSum[paComponentState];
    else
        return NULL;
}

const ImportanceIndex * ComponentIndex::getComponentStateIndexAvg(d_STATESCOUNT paComponentState)
{
    if (computedLevel > First &&
        paComponentState >= 0 && paComponentState < componentStatesIndicesAvg.size())
        return componentStatesIndicesAvg[paComponentState];
    else
        return NULL;
}

const ImportanceIndex * ComponentIndex::getSystemStateIndexSum(d_STATESCOUNT paSystemState)
{
    if (computedLevel > First &&
        paSystemState >= 0 && paSystemState < systemStatesIndicesSum.size())
        return systemStatesIndicesSum[paSystemState];
    else
        return NULL;
}

const ImportanceIndex * ComponentIndex::getSystemStateIndexAvg(d_STATESCOUNT paSystemState)
{
    if (computedLevel > First &&
        paSystemState >= 0 && paSystemState < systemStatesIndicesAvg.size())
        return systemStatesIndicesAvg[paSystemState];
    else
        return NULL;
}

const ImportanceIndex * ComponentIndex::getBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState)
{
    if (computedLevel > Second &&
        paComponentState >= 0 && paComponentState < indicesMatrix.getRowsCount() &&
        paSystemState >= 0 && paSystemState < indicesMatrix.getColumnsCount())
    {
        bool pomOK;
        return indicesMatrix.get(paComponentState, paSystemState, pomOK);
    }
    else
        return NULL;
}







