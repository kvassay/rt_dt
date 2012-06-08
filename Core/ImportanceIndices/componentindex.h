#ifndef COMPONENTINDEX_H
#define COMPONENTINDEX_H

#include <vector>

#include "Core/Utilities/matrix.h"
#include "Core/ImportanceIndices/importanceindex.h"
#include "Core/System/component.h"
#include "Core/System/system.h"

class ComponentIndex
{
protected:
    typedef Matrix<ImportanceIndex*> IndicesMatrix;
    typedef std::vector<ImportanceIndex*> IndicesVector;

private:
    Component *component;
    Level computedLevel;

    ImportanceIndex importanceIndex;
    ImportanceIndex importanceIndexSum;
    IndicesVector componentStatesIndicesSum;
    IndicesVector componentStatesIndicesAvg;
    IndicesVector systemStatesIndicesSum;
    IndicesVector systemStatesIndicesAvg;
    IndicesMatrix indicesMatrix;

    void deleteComponentStatesIndices();
    void deleteSystemStatesIndices();
    void deleteIndicesMatrix();
    void deleteDataStructures();
protected:
    void createDataStructures(Level paLevel);

    virtual bool checkBeforeUpdate(Error &paError);

    virtual d_IMPORTANCEINDEX computeBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState, Error &paError) = 0;
    virtual void computeDataStructures(Level paLevel, ImportanceIndex &paIndex, ImportanceIndex &paIndexSum,
                                       ComponentIndex::IndicesVector &paComponentStatesIndicesSum, ComponentIndex::IndicesVector &paComponentStatesIndicesAvg,
                                       ComponentIndex::IndicesVector &paSystemStatesIndicesSum, ComponentIndex::IndicesVector &paSystemStatesIndicesAvg,
                                       ComponentIndex::IndicesMatrix &paIndicesMatrix, Error &paError);

    d_STATESCOUNT getComponentStatesCount() { return component->getStatesCount(); }
    d_STATESCOUNT getSystemStatesCount() { return component->getSystem()->getStatesCount(); }

public:
    ComponentIndex(Component &paComponent);
    Component *getComponent() { return component; }
    virtual ~ComponentIndex();
    void compute(Level paLevel, Error &paError);

    const ImportanceIndex* getMainIndexAvg() { return computedLevel == None ? NULL : &importanceIndex; }
    const ImportanceIndex* getMainIndexSum() { return computedLevel == None ? NULL : &importanceIndexSum; }
    const ImportanceIndex* getComponentStateIndexSum(d_STATESCOUNT paComponentState);
    const ImportanceIndex* getComponentStateIndexAvg(d_STATESCOUNT paComponentState);
    const ImportanceIndex* getSystemStateIndexSum(d_STATESCOUNT paSystemState);
    const ImportanceIndex* getSystemStateIndexAvg(d_STATESCOUNT paSystemState);
    const ImportanceIndex* getBaseIndex(d_STATESCOUNT paComponentState, d_STATESCOUNT paSystemState);
};

#endif // COMPONENTINDEX_H
