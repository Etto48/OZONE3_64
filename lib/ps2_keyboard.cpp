#include <ps2_keyboard.h>

namespace keyboard
{
    char getc()
    {
        uint64_t get_char_process = ozone::INVALID_PROCESS;
        while (get_char_process == ozone::INVALID_PROCESS)
            get_char_process = ozone::user::driver_call(1, 0); //get_char
        return (char)ozone::user::join(get_char_process);
    }
};