#include "include/string.h"
#include "include/math.h"

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
/*WIP
string::string()
{
    this->len=0;
	this->data=(char*)malloc(this->len+1);
	this->data[this->len]='\0';
}
string::string(const char* str)
{
	this->len=strlen(str);
	this->data=(char*)malloc(this->len+1);
	for(size_t i=0;i<this->len&&str[i];i++)
	{
		this->data[i]=str[i];
	}
	this->data[this->len]='\0';
}
string::string(const string& old)
{
    this->len=old.len;
	this->data=(char*)malloc(this->len+1);
	for(size_t i=0;i<this->len&&old.data[i];i++)
	{
		this->data[i]=old.data[i];
	}
	this->data[this->len]='\0';
}
string::string(size_t strlen,char fill)
{
    this->len=strlen;
	this->data=(char*)malloc(this->len+1);
	for(size_t i=0;i<this->len;i++)
	{
		this->data[i]=fill;
	}
	this->data[this->len]='\0';
}

string::~string()
{
	free(this->data);
}

size_t string::getLen()
{
	return this->len;
}

int string::to_int(int base)
{
    return atoi(this->data,base);
}

string::operator char*()
{
	return this->data;
}
string::operator int()
{
    return this->to_int(10);
}

string string::operator +(const string& str)const
{
    string ret(this->len+str.len);
    for(size_t i=0;i<this->len;i++)
    {
        ret.data[i]=this->data[i];
    }
    for(size_t i=0;i<str.len;i++)
    {
        ret.data[i+this->len]=str.data[i];
    }
    ret.data[str.len+this->len]='\0';
    return ret;
}

string string::operator *(const size_t times)const
{
    if(times==0)return "";
    string ret(this->data);
    for(size_t i=1; i<times;i++)
    {
        ret=ret+string(this->data);
    }
    return ret;
}

string& string::operator =(const string& str)
{
    if(this==&str)return *this;
    if(str.len!=this->len)
    {
        free(this->data);
        this->len=str.len;
	    this->data=(char*)malloc(this->len+1);
    }
	for(size_t i=0;i<this->len&&str.data[i];i++)
	{
		this->data[i]=str.data[i];
	}
	this->data[this->len]='\0';
    return *this;
}

bool string::operator ==(const string& str)const
{
    if(this->len!=str.len)return false;
    for(size_t i=0; i<this->len; i++)
    {
        if(this->data[i]!=str.data[i])return false;
    }
    return true;
}
bool string::operator ==(const char* str)const
{
    return (*this)==string(str);
}
bool string::operator !=(const string& str)const
{
    return !((*this)==str);
}
bool string::operator !=(const char* str)const
{
    return (*this)!=string(str);
}

char* string::begin()
{
    return &data[0];
}
char* string::end()
{
    return &data[len];
}
const char* string::cbegin()const
{
    return &data[0];
}
const char* string::cend()const
{
    return &data[len];
}

vector<string> string::split(string chars)
{
    vector<string> ret;
    uint32_t from=0;
    for(uint32_t to = 0;to<len;to++)
    {
        bool is_truncating = false;
        for(auto& c:chars)
        {
            if(c==data[to])
            {
                is_truncating=true;
                break;
            }
        }
        //"cavallo bianco   di napoleone"
        //                ^
        //"cavallo bianco di napoleone"
        // ^      ^
        if(is_truncating)
        {
            if(from==to)
            {
                from++;
            }else
            {
                auto to_push = string(to-from);
                for(auto j=0;j<int32_t(to-from);j++)
                {
                    to_push.data[j]=data[j+from];
                }
                ret.push_back(to_push);
                from=to+1;
            }
        }
    }
    if(from!=len)
    {
        auto to_push = string(len-from);
        for(auto j=0;j<int32_t(len-from);j++)
        {
            to_push.data[j]=data[j+from];
        }
        ret.push_back(to_push);
    }
    return ret;
}

string operator +(const char* str1,const string& str2)
{
    return string(str1)+str2;
}

string to_string(int number,int base,size_t min_chars)
{
    char buf[34];
    itoa(number,buf,base);
    size_t len=max(strlen(buf),min_chars);
    
    string ret(len,'0');
    size_t delta=(size_t)max(0,(int)min_chars-(int)strlen(buf));
    for(size_t i=0; i<=strlen(buf);i++)
    {
        ret[i+delta]=buf[i];
    }
    return ret;
}
string to_string(unsigned long long int number,int base,size_t min_chars)
{
    char buf[34];
    utoa(number,buf,base);
    size_t len=max(strlen(buf),min_chars);
    
    string ret(len,'0');
    size_t delta=(size_t)max(0,(int)min_chars-(int)strlen(buf));
    for(size_t i=0; i<=strlen(buf);i++)
    {
        ret[i+delta]=buf[i];
    }
    return ret;
}
*/

