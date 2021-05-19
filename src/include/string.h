#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "math.h"
#include "memory.h"
#include "vector.h"

size_t strlen(const char* str);

class string
{
    private:
    char* data;
    size_t len;

    public:
    string(size_t strlen,char fill=0);
    string();
    string(const char* str);
    string(const string& old);
    ~string();
    
    size_t getLen();
    int to_int(int base=10);
    
    operator char*();
    explicit operator int();
    string operator +(const string& str)const;
    string operator *(const size_t times)const;
    string& operator =(const string& str);
    bool operator ==(const string& str)const;
    bool operator ==(const char* str)const;
    bool operator !=(const string& str)const;
    bool operator !=(const char* str)const;

    char* begin();
    char* end();
    const char* cbegin()const;
    const char* cend()const;

    vector<string> split(string chars=" ");
};
string operator +(const char* str1,const string& str2);

string to_string(int number,int base=10,size_t min_chars=0);
string to_string(unsigned long long number,int base=10,size_t min_chars=0);
