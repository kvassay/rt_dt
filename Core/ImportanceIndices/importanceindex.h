#ifndef IMPORTANCEINDEX_H
#define IMPORTANCEINDEX_H

#include <limits>
#include <ostream>

#include "Core/definitions.h"

class ImportanceIndex
{
private:
    bool defined;
    d_IMPORTANCEINDEX value;

public:
    ImportanceIndex();
    ImportanceIndex(const ImportanceIndex &paImportanceIndex);
    ImportanceIndex(d_IMPORTANCEINDEX paValue);
    virtual ~ImportanceIndex() { }

    d_IMPORTANCEINDEX getValue() const { return value; }
    bool isDefined() const { return defined; }
    bool isNotDefined() const { return !defined; }
    void setNotDefined()
    {
        defined = false;
        value = std::numeric_limits<d_IMPORTANCEINDEX>::quiet_NaN();
    }

    virtual void setValue(d_IMPORTANCEINDEX paValue)
    {
        defined = true;
        value = paValue;
    }

    ImportanceIndex& operator=(const ImportanceIndex &paIndex);
    //ImportanceIndex& operator=(ImportanceIndex paIndex);

    ImportanceIndex& operator+=(ImportanceIndex &paIndex);
    ImportanceIndex& operator-=(ImportanceIndex &paIndex);
    ImportanceIndex& operator/=(d_STATESCOUNT paStatesCount);


    friend ImportanceIndex operator+(ImportanceIndex &paIndex1, ImportanceIndex &paIndex2);
    friend ImportanceIndex operator-(ImportanceIndex &paIndex1, ImportanceIndex &paIndex2);
    friend ImportanceIndex operator/(ImportanceIndex &paIndex1, d_STATESCOUNT paStatesCount);

    friend std::ostream& operator<<(std::ostream &paStream, ImportanceIndex &paIndex);
    friend std::ostream& operator<<(std::ostream &paStream, const ImportanceIndex *paIndex);
    friend std::ostream& operator<<(std::ostream &paStream, ImportanceIndex *paIndex);
};

#endif // IMPORTANCEINDEX_H
