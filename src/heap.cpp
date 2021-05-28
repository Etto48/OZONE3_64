#include "include/heap.h"

heap system_heap{&memory::stack_bottom};

struct HB
{
    HB(size_t bytes,bool isFree):size(bytes),free(isFree),next(nullptr){}
    size_t size;
    bool free;
    HB* next;    
};

void* getDataLocation(HB* blockLocation)
{
	return (void*)((char*)blockLocation+sizeof(HB));
}

HB* getBlockLocation(void* dataLocation)
{
	return (HB*)((char*)dataLocation-sizeof(HB));
}

constexpr uint64_t HEAP_GRANULARITY = 32;

heap::heap(void* address):address(address)
{
    is_empty=true;
}
void* heap::malloc(uint64_t size)
{
    //allocations++;
	size_t chunks=size/HEAP_GRANULARITY+1;
	size_t effective_size=HEAP_GRANULARITY*chunks;
	HB* ret;
	if(is_empty)
	{
		is_empty=false;
		//head insert
		ret=(HB*)address;
	}
	else
	{
		HB* last=nullptr;
		for(HB* p=(HB*)address;p;p=p->next)
		{
			if(p->free&&p->size>=size)//if free and big enough
			{
				p->free=false;
				return getDataLocation(p);
			}
			else if(p->free&&!p->next)//if last but too small, enlarge it
			{
				p->free=false;
				p->size=effective_size;
				return getDataLocation(p);
			}
			last=p;
		}
		ret=(HB*)(((char*)&last[1])+last->size);
		last->next=ret;
	}

	*ret=HB(effective_size,false);
    return getDataLocation(ret);
}
void* heap::malloc(uint64_t size, uint64_t align)
{
    return nullptr;
}
void heap::free(void* loc)
{
    if(loc)
	{
		//allocations--;
    	getBlockLocation(loc)->free=true;
        if(getBlockLocation(loc)==address)
        {
            is_empty=true;
        }
	}
}
