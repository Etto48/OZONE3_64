ENTRY(_start)
SECTIONS
{
    MODULE_VMA = 0xFFFF800000000000;
    . += MODULE_VMA;
    . = ALIGN(4096);


    .text : AT(ADDR(.text) - MODULE_VMA)
    {   
        _code = .;
        PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);

        PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);

        PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT(.fini_array.*)))
		KEEP (*(.fini_array*))
		PROVIDE_HIDDEN (__fini_array_end = .);

        *(.rodata*)
        . = ALIGN(4096);
    }

   .data : AT(ADDR(.data) - MODULE_VMA)
   {
        _data = .;
        *(.data)
        . = ALIGN(4096);
   }

   .eh_frame : AT(ADDR(.eh_frame) - MODULE_VMA)
   {
       _ehframe = .;
       *(.eh_frame)
        . = ALIGN(4096);
   }

    _edata = .;

   .bss : AT(ADDR(.bss) - MODULE_VMA)
   {
       _bss = .;
       *(.bss)

       /*
        * You usually need to include generated COMMON symbols
        * under kernel BSS section or use gcc's -fno-common
        */

        *(COMMON)
       . = ALIGN(4096);
   }

   _end = .;

   /DISCARD/ :
   {
        *(.comment)
   }
}