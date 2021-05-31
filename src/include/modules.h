#pragma once
#include <stdint.h>
#include "multiboot.h"
#include "elf64.h"
#include "printing.h"
#include "memory.h"

//loads a module into memory and returns its entrypoint
void* load_module(multiboot_module_t* mod);