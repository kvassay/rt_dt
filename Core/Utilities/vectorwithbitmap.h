#ifndef VECTORWITHBITMAP_H
#define VECTORWITHBITMAP_H

#include <cstddef>
#include <cstring>
#include <vector>

template <class T>
class VectorWithBitmap
{
private:
    std::vector<T> *data;
    std::vector<bool> *bitmap;

    void setWithoutClear(const VectorWithBitmap &paVectorWithBitmap);
public:
    VectorWithBitmap();
    VectorWithBitmap(size_t paSize, T paValue = T());
    VectorWithBitmap(const VectorWithBitmap& paVectorWithBitmap);
    ~VectorWithBitmap();

    size_t getSize();

    void clear();
    void resize(size_t paSize, T paValue = T());
    bool isSet(size_t paIndex);
    bool tryReset(size_t paIndex);
    bool trySet(size_t paIndex, T paValue);
    bool tryGet(size_t paIndex, T &paValue);
    T get(size_t paIndex, bool &paOK);

    VectorWithBitmap& operator=(const VectorWithBitmap &paMatrixWithBitmap);
};

template <class T>
void VectorWithBitmap<T>::setWithoutClear(const VectorWithBitmap<T> &paVectorWithBitmap)
{
    if (paVectorWithBitmap.data != NULL)
    {
        data = new std::vector<T>(paVectorWithBitmap.data);
    }
    else
    {
        data = NULL;
    }

    if (paVectorWithBitmap.bitmap != NULL)
    {
        bitmap = new std::vector<bool>(paVectorWithBitmap.bitmap);
    }
    else
    {
        bitmap = NULL;
    }
}


template <class T>
VectorWithBitmap<T>::VectorWithBitmap()
{
    data = NULL;
    bitmap = NULL;
}

template <class T>
VectorWithBitmap<T>::VectorWithBitmap(size_t paSize, T paValue)
{
    data = NULL;
    bitmap = NULL;

    resize(paSize, paValue);
}

template <class T>
VectorWithBitmap<T>::VectorWithBitmap(const VectorWithBitmap<T>& paVectorWithBitmap)
{
    setWithoutClear(paVectorWithBitmap);
}

template <class T>
VectorWithBitmap<T>::~VectorWithBitmap()
{
    clear();
}

template <class T>
size_t VectorWithBitmap<T>::getSize()
{
    if (data != NULL)
        return data->size();
    else
        return 0;
}

template <class T>
void VectorWithBitmap<T>::clear()
{
    if (bitmap != NULL)
    {
        delete bitmap;
        bitmap = NULL;
    }

    if (data != NULL)
    {
        delete data;
        data = NULL;
    }
}

template <class T>
void VectorWithBitmap<T>::resize(size_t paSize, T paValue)
{
    size_t pomOldSize = getSize();

    if (paSize != pomOldSize)
    {
        if (data == NULL)
        {
            data = new std::vector<T>(paSize, paValue);
            bitmap = new std::vector<bool>(paSize, false);
            return;
        }
        if (paSize <= 0)
        {
            clear();
            return;
        }

        data->resize(paSize, paValue);
        bitmap->resize(paSize, false);
    }
}

template <class T>
bool VectorWithBitmap<T>::isSet(size_t paIndex)
{
    if (paIndex >= 0 && paIndex < getSize())
    {
        return (*bitmap)[paIndex];
    }
    else
        return false;
}

template <class T>
bool VectorWithBitmap<T>::tryReset(size_t paIndex)
{
    if (paIndex >= 0 && paIndex < getSize())
    {
        (*bitmap)[paIndex] = false;

        return true;
    }
    else
        return false;
}

template <class T>
bool VectorWithBitmap<T>::trySet(size_t paIndex, T paValue)
{
    if (paIndex >= 0 && paIndex < getSize())
    {
        (*data)[paIndex] = paValue;
        (*bitmap)[paIndex] = true;

        return true;
    }
    else
        return false;
}

template <class T>
bool VectorWithBitmap<T>::tryGet(size_t paIndex, T &paValue)
{
    if (paIndex >= 0 && paIndex < getSize())
    {
        if ((*bitmap)[paIndex])
        {
            paValue = (*data)[paIndex];
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

template <class T>
T VectorWithBitmap<T>::get(size_t paIndex, bool &paOK)
{
    T pomValue;
    paOK = tryGet(paIndex, pomValue);
    return pomValue;
}

template <class T>
VectorWithBitmap<T>& VectorWithBitmap<T>::operator=(const VectorWithBitmap<T> &paVectorWithBitmap)
{
    if (this != &paVectorWithBitmap)
    {
        clear();
        setWithoutClear(paVectorWithBitmap);
    }
    return *this;
}

#endif // VECTORWITHBITMAP_H
