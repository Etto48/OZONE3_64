#pragma once
#include <stdint.h>
#include <stdarg.h>
#include "memory.h"


template<class T>
class vector
{
protected:
    uint32_t length;
    uint32_t allocator;
    T* data_pointer;
public:
    vector(uint32_t len);
    vector();
    //template <typename... Args>
    //vector(uint32_t len, Args... argList);
    vector(const vector<T>& old);
    vector<T>& operator =(const vector<T>& v);

    void reserve(uint32_t size);
    void push_back(const T& item);
    T remove(uint32_t index);
    uint32_t get_length()const {return length;}
    uint32_t get_allocator()const {return allocator;}
    T& operator[](uint32_t index);
    T operator[](uint32_t index)const;

    T* begin();
    T* end();
    const T* cbegin()const;
    const T* cend()const;

    ~vector();
};

vector<int> range(uint32_t count,int from=0,int step = 1);
/*WIP
template<class T>
vector<T>::vector(uint32_t len)
{
    length = len;
    allocator = (length/16+1)*16;
    data_pointer = (T*)malloc(sizeof(T)*allocator);
    for(uint32_t i=0;i<len;i++)
    {
        data_pointer[i]=T();
    }
}

template<class T>
vector<T>::vector()
{
    length = 0;
    allocator = 16;
    data_pointer = (T*)malloc(sizeof(T)*allocator);
}*/

/*template<class T>
template<typename... Args>
vector<T>::vector(uint32_t len, Args... argList)
{
    length = len;
    allocator = (length/16+1)*16;
    data_pointer = (T*)malloc(sizeof(T)*allocator);
    va_list vl;
    if(sizeof... argList==0)
    {
        for(uint32_t i=0;i<len;i++)
        {
            data_pointer[i]=T();
        }
    }
    else
    {
        va_start(vl,len);
        for(uint32_t i=0;i<len;i++)
        {
            data_pointer[i]=va_arg(vl,T);
        }
        va_end(vl);
    }

}*/

/*WIP
template<class T>
vector<T>::vector(const vector<T>& old)
{
    length = old.length;
    allocator = old.allocator;
    data_pointer = (T*)malloc(sizeof(T)*allocator);
    for(uint32_t i=0;i<length;i++)
    {
        data_pointer[i]=old.data_pointer[i];
    }
}

template<class T>
vector<T>& vector<T>::operator =(const vector<T>& v)
{
    if(&v==this)return *this;
    if(allocator<v.allocator)
    {
        free(data_pointer);
        data_pointer = (T*)malloc(sizeof(T)*v.allocator);
    }
    length = v.length;
    allocator = v.allocator;
    for(uint32_t i=0;i<length;i++)
    {
        data_pointer[i]=v.data_pointer[i];
    }
    return *this;
}

template<class T>
void vector<T>::reserve(uint32_t len)
{
    uint32_t new_allocator = ((len/16+1)*16);
    if(allocator<new_allocator)
    {
        
        auto new_data_pointer = (T*)malloc(sizeof(T)*new_allocator);
        allocator = new_allocator;

        for(uint32_t i =0;i<length;i++)
        {
            new_data_pointer[i]=data_pointer[i];
        }
        free(data_pointer);
        data_pointer = new_data_pointer;
    }
}
*/

template<class T>
void vector<T>::push_back(const T& item)
{
    if(length+1>=allocator)
        reserve(length+1);
    data_pointer[length++]=item;
}

template<class T>
T vector<T>::remove(uint32_t index)
{
    T ret = operator[](index);
    for(uint32_t i = index;i<(length-1);i++)
    {
        data_pointer[i]=data_pointer[i+1];
    }
    length--;
    return ret;
}

template<class T>
T& vector<T>::operator[](uint32_t index)
{
    if(index>=length)
        asm volatile ("int $0x5");
    return data_pointer[index];
}

template<class T>
T vector<T>::operator[](uint32_t index)const
{
    if(index>=length)
        asm volatile ("int $0x5");
    return data_pointer[index];
}
template<class T>
T* vector<T>::begin()
{
    return &data_pointer[0];
}

template<class T>
T* vector<T>::end()
{
    return &data_pointer[length];
}

template<class T>
const T* vector<T>::cbegin()const
{
    return &data_pointer[0];
}

template<class T>
const T* vector<T>::cend()const
{
    return &data_pointer[length];
}

/*WIP
template<class T>
vector<T>::~vector()
{
    free(data_pointer);
}
*/
