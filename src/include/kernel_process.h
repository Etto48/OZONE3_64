#pragma once
#include "printing.h"
#include "interrupt.h"
#include "apic.h"
#include "clock.h"
#include <ozone.h>
#include "boot_info.h"
#include "modules.h"

namespace kernel
{
    void init();
};