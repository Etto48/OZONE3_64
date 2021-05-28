#pragma once
#include "printing.h"
#include "interrupt.h"
#include "apic.h"
#include "clock.h"
#include "sys.h"

namespace kernel
{
    void test_process3();
    void test_process2();
    void test_process();

    void init();
};