#pragma once
#include "printing.h"
#include "interrupt.h"
#include "apic.h"
#include "clock.h"
#include <ozone.h>
#include <ps2_keyboard.h>
#include "boot_info.h"
#include "modules.h"
#include "pci.h"

namespace kernel
{
    void init();
};