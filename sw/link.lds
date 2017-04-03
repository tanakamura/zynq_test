/* Default linker script, for normal executables */
ENTRY(_start)

SECTIONS
{
   _ocm_start = .;
   . = 0x00008000;
   .text_ocm : { *(.text_ocm) }

   _ocm_end = .;

   . = 0x00100000;
   .text : { *(.text) }
   .rodata : {
    *(.rodata .rodata.*)
   }
   .data : {
    *(.data .data.*)
   }

   .bss : { *(.bss) }

   _end = .;
   end = .;

   . = ALIGN(32);
   . = . + 1M;
   _stack = .;

   . = ALIGN(1024*1024);
   _loaded = .;
}

