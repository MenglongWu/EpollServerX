OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
SECTIONS
{
    .text 0x100000:
    {
        *(.text)
    }
 
    .data :
    {
        *(.data)
    }
}
