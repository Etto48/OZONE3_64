#include <ps2_keyboard.h>

namespace keyboard
{
    char getc()
    {
        uint64_t get_char_process = user::INVALID_PROCESS;
        while (get_char_process == user::INVALID_PROCESS)
            get_char_process = user::driver_call(1, 0); //get_char
        return (char)user::join(get_char_process);
    }
};