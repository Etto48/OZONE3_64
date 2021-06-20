#include <ozone.h>

ozone::shmid_t shmid = ozone::INVALID_SHAREDMEMORY;

int p1()
{
    auto addr = (uint64_t *)ozone::user::shm_attach(shmid);
    *addr = 48;
    return 0;
}

int p2()
{
    auto addr = (uint64_t *)ozone::user::shm_attach(shmid);
    while (*addr != 48)
        ;
    ozone::user::sleep(4000);
    return 0;
}

int main()
{
    ozone::user::sleep(4000);
    shmid = ozone::user::shm_get(true, 0x100000);
    auto pid1 = ozone::user::fork(p1);
    auto pid2 = ozone::user::fork(p2);

    ozone::user::join(pid1);
    ozone::user::join(pid2);

    return 0;
}