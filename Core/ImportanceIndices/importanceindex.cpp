#include "importanceindex.h"

ImportanceIndex::ImportanceIndex()
{
    setNotDefined();
}

ImportanceIndex::ImportanceIndex(d_IMPORTANCEINDEX paValue)
{
    setValue(paValue);
}

ImportanceIndex::ImportanceIndex(const ImportanceIndex &paImportanceIndex)
{
    defined = paImportanceIndex.defined;
    value = paImportanceIndex.value;
}


ImportanceIndex & ImportanceIndex::operator =(const ImportanceIndex &paIndex)
{
    if (this != &paIndex)
    {
        defined = paIndex.defined;
        value = paIndex.value;
    }
    return *this;
}

/*ImportanceIndex & ImportanceIndex::operator =(ImportanceIndex paIndex)
{
    defined = paIndex.defined;
    value = paIndex.value;
    return *this;
}*/

ImportanceIndex & ImportanceIndex::operator +=(ImportanceIndex &paIndex)
{
    if (defined && paIndex.defined)
        value += paIndex.value;
    else
        setNotDefined();
    return *this;
}

ImportanceIndex & ImportanceIndex::operator -=(ImportanceIndex &paIndex)
{
    if (defined && paIndex.defined)
        value -= paIndex.value;
    else
        setNotDefined();
    return *this;
}

ImportanceIndex & ImportanceIndex::operator /=(d_STATESCOUNT paStatesCount)
{
    if (defined && paStatesCount != 0)
        value /= (d_IMPORTANCEINDEX)paStatesCount;
    else
        setNotDefined();
    return *this;
}

ImportanceIndex operator +(ImportanceIndex &paIndex1, ImportanceIndex &paIndex2)
{
    ImportanceIndex pomIndex;
    if (paIndex1.isNotDefined() || paIndex2.isNotDefined())
        pomIndex.setNotDefined();
    else
        pomIndex.setValue(paIndex1.getValue() + paIndex2.getValue());

    return pomIndex;
}

ImportanceIndex operator -(ImportanceIndex &paIndex1, ImportanceIndex &paIndex2)
{
    ImportanceIndex pomIndex;
    if (paIndex1.isNotDefined() || paIndex2.isNotDefined())
        pomIndex.setNotDefined();
    else
        pomIndex.setValue(paIndex1.getValue() - paIndex2.getValue());

    return pomIndex;
}

ImportanceIndex operator /(ImportanceIndex &paIndex1, d_STATESCOUNT paStatesCount)
{
    ImportanceIndex pomIndex;
    if (paIndex1.isNotDefined() || paStatesCount == 0)
        pomIndex.setNotDefined();
    else
        pomIndex.setValue(paIndex1.getValue() / (d_IMPORTANCEINDEX)paStatesCount);

    return pomIndex;
}

std::ostream & operator <<(std::ostream &paStream, ImportanceIndex &paIndex)
{
    if (paIndex.defined)
        paStream << paIndex.value;
    else
        paStream << "NaN";
    return paStream;
}

std::ostream & operator <<(std::ostream &paStream, const ImportanceIndex *paIndex)
{
    if (paIndex == NULL)
        return paStream;

    if (paIndex->defined)
        paStream << paIndex->value;
    else
        paStream << "NaN";
    return paStream;
}

std::ostream & operator <<(std::ostream &paStream, ImportanceIndex *paIndex)
{
    if (paIndex == NULL)
        return paStream;

    if (paIndex->defined)
        paStream << paIndex->value;
    else
        paStream << "NaN";
    return paStream;
}
