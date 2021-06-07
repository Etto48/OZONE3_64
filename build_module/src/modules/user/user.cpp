#include <ozone.h>

void foo()
{
    for(uint64_t i =0;i<1000;i++);
}

int main()
{
    user::fork(foo);
    for(uint64_t i = 0; i<10000; i++);
    return 0;
}