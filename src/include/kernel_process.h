#pragma once
#include "printing.h"
#include "interrupt.h"
#include "apic.h"
#include "clock.h"
#include "ozone/sys.h"

namespace kernel
{
    void test_process5();
    void test_process4();
    void test_process3();
    void test_process2();
    void test_process();

    void init();
};