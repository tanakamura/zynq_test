/* Default linker script, for normal executables */
ENTRY(_start)

SECTIONS
{
   . = 0x00008000;
   .text_ocm : { *(.text_ocm) }

   . = 0x00100000;
   .text : { *(.text) }
   .rodata : {
    *(.rodata .rodata.*)
   }
   .data : {
    *(.data .data.*)
   }

   .bss : { *(.bss) }

   . = ALIGN(32);
   . = . + 0x10000;
   _stack = .;
}

