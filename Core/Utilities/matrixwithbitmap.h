#ifndef MATRIXWITHBITMAP_H
#define MATRIXWITHBITMAP_H

#include <cstddef>
#include <cstring>

#include "Core/Utilities/matrix.h"

template <class T>
class MatrixWithBitmap
{
private:
    Matrix<T> *matrix;
    char *bitmap;
    size_t bitmapSize;

    bool isBitmapSet(size_t paRow, size_t paColumn);
    void setBitmap(size_t paRow, size_t paColumn, bool paValue);
    void setWithoutClear(const MatrixWithBitmap &paMatrixWithBitmap);
public:
    MatrixWithBitmap();
    MatrixWithBitmap(size_t paRowsCount, size_t paColumnsCount, T paValue = T());
    MatrixWithBitmap(const MatrixWithBitmap& paMatrixWithBitmap);
    ~MatrixWithBitmap();

    size_t getRowsCount();
    size_t getColumnsCount();

    void clear();
    void resize(size_t paRowsCount, size_t paColumnsCount, T paValue = T());
    bool isSet(size_t paRow, size_t paColumn);
    bool tryReset(size_t paRow, size_t paColumn);
    bool trySet(size_t paRow, size_t paColumn, T paValue);
    bool tryGet(size_t paRow, size_t paColumn, T &paValue);
    T get(size_t paRow, size_t paColumn, bool &paOK);

    MatrixWithBitmap& operator=(const MatrixWithBitmap &paMatrixWithBitmap);
};


template <class T>
bool MatrixWithBitmap<T>::isBitmapSet(size_t paRow, size_t paColumn)
{
    size_t pomBitIndex = (paRow * getColumnsCount()) + paColumn;
    size_t pomCharIndex = pomBitIndex / (8*sizeof(char));
    size_t pomCharOffset = pomBitIndex % (8*sizeof(char));
    char pomChar = 1 << (8*sizeof(char) - 1 - pomCharOffset);

    return (bitmap[pomCharIndex] & pomChar) != 0;
}

template <class T>
void MatrixWithBitmap<T>::setBitmap(size_t paRow, size_t paColumn, bool paValue)
{
    size_t pomBitIndex = (paRow * getColumnsCount()) + paColumn;
    size_t pomCharIndex = pomBitIndex / (8*sizeof(char));
    size_t pomCharOffset = pomBitIndex % (8*sizeof(char));
    char pomChar = 1 << (8*sizeof(char) - 1 - pomCharOffset);

    if (paValue)
    {
        bitmap[pomCharIndex] = bitmap[pomCharIndex] | pomChar;
    }
    else
    {
        pomChar = ~pomChar;
        bitmap[pomCharIndex] = bitmap[pomCharIndex] & pomChar;
    }
}

template <class T>
void MatrixWithBitmap<T>::setWithoutClear(const MatrixWithBitmap<T> &paMatrixWithBitmap)
{
    if (paMatrixWithBitmap.matrix != NULL)
    {
        matrix = new Matrix<T>(paMatrixWithBitmap.matrix);
    }
    else
    {
        matrix = NULL;
    }

    bitmapSize = paMatrixWithBitmap.bitmapSize;
    if (paMatrixWithBitmap.bitmap != NULL)
    {
        bitmap = new char[bitmapSize];
        for (size_t i = 0; i < bitmapSize; i++)
            bitmap[i] = paMatrixWithBitmap.bitmap[i];
    }
    else
    {
        bitmap = NULL;
    }
}


template <class T>
MatrixWithBitmap<T>::MatrixWithBitmap()
{
    matrix = NULL;
    bitmap = NULL;
    bitmapSize = 0;
}

template <class T>
MatrixWithBitmap<T>::MatrixWithBitmap(size_t paRowsCount, size_t paColumnsCount, T paValue)
{
    matrix = NULL;
    bitmap = NULL;
    bitmapSize = 0;

    resize(paRowsCount, paColumnsCount, paValue);
}

template <class T>
MatrixWithBitmap<T>::MatrixWithBitmap(const MatrixWithBitmap<T>& paMatrixWithBitmap)
{
    setWithoutClear(paMatrixWithBitmap);
}

template <class T>
MatrixWithBitmap<T>::~MatrixWithBitmap()
{
    clear();
}

template <class T>
size_t MatrixWithBitmap<T>::getRowsCount()
{
    if (matrix != NULL)
        return matrix->getRowsCount();
    else
        return 0;
}

template <class T>
size_t MatrixWithBitmap<T>::getColumnsCount()
{
    if (matrix != NULL)
        return matrix->getColumnsCount();
    else
        return 0;
}

template <class T>
void MatrixWithBitmap<T>::clear()
{
    if (bitmap != NULL)
    {
        delete bitmap;
        bitmap = NULL;
    }
    bitmapSize = 0;

    if (matrix != NULL)
    {
        delete matrix;
        matrix = NULL;
    }
}

template <class T>
void MatrixWithBitmap<T>::resize(size_t paRowsCount, size_t paColumnsCount, T paValue)
{
    size_t pomOldRowsCount = getRowsCount();
    size_t pomOldColumnsCount = getColumnsCount();

    if (paRowsCount != pomOldRowsCount || paColumnsCount != pomOldColumnsCount)
    {
        if (matrix == NULL)
        {
            matrix = new Matrix<T>(paRowsCount, paColumnsCount, paValue);
            bitmapSize = (paRowsCount * paColumnsCount) / (8*sizeof(char)) + 1;
            bitmap = new char[bitmapSize];
            memset(bitmap, 0, bitmapSize);
            return;
        }
        if (paRowsCount <= 0 || paColumnsCount <= 0)
        {
            clear();
            return;
        }


        matrix->resize(paRowsCount, paColumnsCount, paValue);

        size_t pomBlockSize = 8*sizeof(char);
        size_t pomNewBitmapSize = (paRowsCount * paColumnsCount) / (pomBlockSize) + 1;
        char *pomNewBitmap = new char[pomNewBitmapSize];
        memset(pomNewBitmap, 0, pomNewBitmapSize);

        for (size_t i = 0; i < bitmapSize; i++)
        {
            for (size_t j = 0; j < pomBlockSize; j++)
            {
                size_t pomOldBitIndex = i * (pomBlockSize) + j;
                size_t pomOldRow = pomOldBitIndex / pomOldColumnsCount;
                size_t pomOldColumn = pomOldBitIndex % pomOldColumnsCount;

                if (pomOldRow < paRowsCount && pomOldColumn < paColumnsCount)
                {
                    char pomOldChar = 1 << (pomBlockSize - 1 - j);
                    pomOldChar = bitmap[i] & pomOldChar;
                    if (pomOldChar != 0)
                    {
                        size_t pomNewBitIndex = pomOldRow * paColumnsCount + pomOldColumn;
                        size_t pomNewCharIndex = pomNewBitIndex / (pomBlockSize);
                        size_t pomNewCharOffset = pomNewBitIndex % (pomBlockSize);
                        char pomNewChar = 1 << (pomBlockSize - 1 - pomNewCharOffset);
                        pomNewBitmap[pomNewCharIndex] = pomNewBitmap[pomNewCharIndex] | pomNewChar;
                    }
                }
            }
        }

        delete bitmap;
        bitmap = pomNewBitmap;
        bitmapSize = pomNewBitmapSize;
    }
}

template <class T>
bool MatrixWithBitmap<T>::isSet(size_t paRow, size_t paColumn)
{
    if (paRow >= 0 && paRow < getRowsCount() && paColumn >= 0 && paColumn < getColumnsCount())
    {
        return isBitmapSet(paRow, paColumn);
    }
    else
        return false;
}

template <class T>
bool MatrixWithBitmap<T>::tryReset(size_t paRow, size_t paColumn)
{
    if (paRow >= 0 && paRow < getRowsCount() && paColumn >= 0 && paColumn < getColumnsCount())
    {
        setBitmap(paRow, paColumn, false);
        return true;
    }
    else
        return false;
}

template <class T>
bool MatrixWithBitmap<T>::trySet(size_t paRow, size_t paColumn, T paValue)
{
    if (paRow >= 0 && paRow < getRowsCount() && paColumn >= 0 && paColumn < getColumnsCount())
    {
        bool pomResult = matrix->trySet(paRow, paColumn, paValue);
        if (pomResult)
        {
            setBitmap(paRow, paColumn, true);
        }

        return pomResult;
    }
    else
        return false;
}

template <class T>
bool MatrixWithBitmap<T>::tryGet(size_t paRow, size_t paColumn, T &paValue)
{
    if (paRow >= 0 && paRow < getRowsCount() && paColumn >= 0 && paColumn < getColumnsCount())
    {
        if (isBitmapSet(paRow, paColumn))
            return matrix->tryGet(paRow, paColumn, paValue);
        else
            return false;
    }
    else
        return false;
}

template <class T>
T MatrixWithBitmap<T>::get(size_t paRow, size_t paColumn, bool &paOK)
{
    T pomValue;
    paOK = tryGet(paRow, paColumn, pomValue);
    return pomValue;
}

template <class T>
MatrixWithBitmap<T>& MatrixWithBitmap<T>::operator=(const MatrixWithBitmap<T> &paMatrixWithBitmap)
{
    if (this != &paMatrixWithBitmap)
    {
        clear();
        setWithoutClear(paMatrixWithBitmap);
    }
    return *this;
}

#endif // MATRIXWITHBITMAP_H
