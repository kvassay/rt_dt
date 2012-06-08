#ifndef MATRIX_H
#define MATRIX_H

#include <cstddef>

template <class T>
class Matrix
{
private:
    size_t rowsCount;
    size_t columnsCount;
    T **items;

public:
    Matrix();
    Matrix(size_t paRowsCount, size_t paColumnsCount, T paValue = T());
    Matrix(const Matrix& paMatrix);
    ~Matrix();

    size_t getRowsCount();
    size_t getColumnsCount();

    void clear();
    void resize(size_t paRowsCount, size_t paColumnsCount, T paValue = T());
    bool trySet(size_t paRow, size_t paColumn, T paValue);
    bool tryGet(size_t paRow, size_t paColumn, T &paValue);
    T get(size_t paRow, size_t paColumn, bool &paOK);

    Matrix& operator=(const Matrix &paMatrix);
};


template <class T>
Matrix<T>::Matrix()
{
    rowsCount = 0;
    columnsCount = 0;
    items = NULL;
}

template <class T>
Matrix<T>::Matrix(size_t paRowsCount, size_t paColumnsCount, T paValue)
{
    rowsCount = 0;
    columnsCount = 0;
    items = NULL;

    resize(paRowsCount, paColumnsCount, paValue);
}

template <class T>
Matrix<T>::Matrix(const Matrix<T>& paMatrix)
{
    rowsCount = paMatrix.rowsCount;
    columnsCount = paMatrix.columnsCount;

    if (paMatrix.items != NULL)
    {
        items = new T*[rowsCount];
        for (size_t i = 0; i < rowsCount; i++)
        {
            items[i] = new T[columnsCount];
            for (size_t j = 0; j < columnsCount; j++)
            {
                items[i][j] = paMatrix.items[i][j];
            }
        }
    }
    else
    {
        items = NULL;
    }
}

template <class T>
Matrix<T>::~Matrix()
{
    clear();
}

template <class T>
size_t Matrix<T>::getRowsCount()
{
    return rowsCount;
}

template <class T>
size_t Matrix<T>::getColumnsCount()
{
    return columnsCount;
}

template <class T>
void Matrix<T>::clear()
{
    if (items != NULL)
    {
        for (size_t i = 0; i < rowsCount; i++)
        {
            delete items[i];
        }
        items = NULL;
    }
    rowsCount = 0;
    columnsCount = 0;
}

template <class T>
void Matrix<T>::resize(size_t paRowsCount, size_t paColumnsCount, T paValue)
{
    if (paRowsCount != rowsCount || paColumnsCount != columnsCount)
    {
        if (paRowsCount <= 0 || paColumnsCount <= 0)
        {
            clear();
            return;
        }

        size_t minRowsCount = paRowsCount < rowsCount ? paRowsCount : rowsCount;
        size_t minColumnsCount = paColumnsCount < columnsCount ? paColumnsCount : columnsCount;

        T **pomItems = new T*[paRowsCount];
        for (size_t i = 0; i < paRowsCount; i++)
        {
            pomItems[i] = new T[paColumnsCount];
            if (i < minRowsCount)
            {
                for (size_t j = 0; j < minColumnsCount; j++)
                {
                    pomItems[i][j] = items[i][j];
                }
                for (size_t j = minColumnsCount; j < paColumnsCount; j++)
                {
                    pomItems[i][j] = paValue;
                }
            }
            else
            {
                for (size_t j = 0; j < paColumnsCount; j++)
                {
                    pomItems[i][j] = paValue;
                }
            }
        }
        clear();
        items = pomItems;
        rowsCount = paRowsCount;
        columnsCount = paColumnsCount;
    }
}

template <class T>
bool Matrix<T>::trySet(size_t paRow, size_t paColumn, T paValue)
{
    if (paRow >= 0 && paRow < rowsCount && paColumn >= 0 && paColumn < columnsCount)
    {
        items[paRow][paColumn] = paValue;
        return true;
    }
    else
        return false;
}

template <class T>
bool Matrix<T>::tryGet(size_t paRow, size_t paColumn, T &paValue)
{
    if (paRow >= 0 && paRow < rowsCount && paColumn >= 0 && paColumn < columnsCount)
    {
        paValue = items[paRow][paColumn];
        return true;
    }
    else
        return false;
}

template <class T>
T Matrix<T>::get(size_t paRow, size_t paColumn, bool &paOK)
{
    if (paRow >= 0 && paRow < rowsCount && paColumn >= 0 && paColumn < columnsCount)
    {
        paOK = true;
        return items[paRow][paColumn];
    }
    else
    {
        paOK = false;
        return T();
    }
}

template <class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T> &paMatrix)
{
    if (this != &paMatrix)
    {
        clear();
        rowsCount = paMatrix.rowsCount;
        columnsCount = paMatrix.columnsCount;

        if (paMatrix.items != NULL)
        {
            items = new T*[rowsCount];
            for (size_t i = 0; i < rowsCount; i++)
            {
                items[i] = new T[columnsCount];
                for (size_t j = 0; j < columnsCount; j++)
                {
                    items[i][j] = paMatrix.items[i][j];
                }
            }
        }
        else
        {
            items = NULL;
        }
    }
    return *this;
}

#endif // MATRIX_H
