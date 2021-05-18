// sistema.s
#define ASM 1
#include "include/mboot.h"

/////////////////////////////////////////////////////////////////////////
// AVVIO                                                                //
//////////////////////////////////////////////////////////////////////////
// Il bootstrap loader attiva il modo protetto (per poter accedere agli
// indirizzi di memoria principale superiori a 1MiB) e carica il sistema
// in memoria, quindi salta alla prima
// istruzione del sistema. Il bootstrap loader puo' anche passare
// delle informazioni al sistema (tramite i registri e la memoria).
//
// In questo sistema usiamo lo standard multiboot,
// che definisce il formato che 
// il file contentente il sistema deve rispettare e
// il formato delle informazioni passate dal bootstrap loader al sistema.
// Il formato del file contenente il sistema deve essere quello di un
// normale file eseguibile, ma, nei primi 2*4KiB, deve contenere 
// la struttura multiboot_header, definita piu' avanti. 
// Il boot loader, prima di saltare alla prima istruzione del sistema
// (l'entry point specificato nel file eseguibile), lascia nel registro
// %eax un valore di riconoscimento e in %ebx l'indirizzo di una struttura
// dati, contentente varie informazioni (in particolare, la quantita'
// di memoria principale installata nel sistema, il dispositivo da cui
// e' stato eseguito il bootstrap e l'indirizzo di memoria in cui sono
// stati caricati gli eventuali moduli)
     .text

.set STACK_SIZE, 0x4000

.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.set FREE_SPACE, 0x9000	

.code32	
     // scavalchiamo la struttra richiesta
     				// dal bootstrap loader, che deve
				// trovarsi verso l'inizio del file

     .align  4
     // le seguenti informazioni sono richieste dal bootstrap loader
multiboot_header:
     .long   MAGIC 			
     // valore magico
     .long   FLAGS   				
     // flag
     .long   CHECKSUM
     // checksum
     .long   multiboot_header
     .long   _start
     .long   0
     .long   _end
     .long   multiboot_entry


.globl  _start, start
_start:				
// entry point
start:
multiboot_entry:

     cli
     movl    $(stack + STACK_SIZE), %esp 
     // inizializziamo lo stack

     pushl   %ebx			
     // parametri passati dal loader
     pushl   %eax	
     lea FREE_SPACE, %edi
     jmp SwitchToLongMode
     //call    cmain			
     // il resto dell'inizializzazione
     					// e' scritto in C++

.code32

.set PAGE_PRESENT, (1 << 0)
.set PAGE_WRITE, (1 << 1)

.set CODE_SEG, 0x08
.set DATA_SEG, 0x10
 
.align 4
IDT:
     IDT.Length:       
          .word 0
     IDT.Base:
          .long 0

// Function to switch directly to long mode from real mode.
// Identity maps the first 2MiB.
// Uses Intel syntax.
 
// es:edi    Should point to a valid page-aligned 16KiB buffer, for the PML4, PDPT, PD and a PT.
// ss:esp    Should point to memory that can be used as a small (1 uint32_t) stack
 
SwitchToLongMode:
    // Zero out the 16KiB buffer.
    // Since we are doing a rep stosd, count should be bytes/4.   
    push %di                           
    // REP STOSD alters DI.
    mov $0x1000, %ecx
    xor %eax, %eax
    cld
    rep stosl
    pop %di                            
    // Get DI back.
 
 
    // Build the Page Map Level 4.
    // es:di points to the Page Map Level 4 table.
    lea %es:0x1000(%di), %eax
    // Put the address of the Page Directory Pointer Table in to EAX.
    or $(PAGE_PRESENT | PAGE_WRITE), %eax
    // Or EAX with the flags - present flag, writable flag.
    mov %eax, %es:(%di)
    // Store the value of EAX as the first PML4E.
 
 
    // Build the Page Directory Pointer Table.
    lea %es:0x2000(%di), %eax
    // Put the address of the Page Directory in to EAX.
    or $(PAGE_PRESENT | PAGE_WRITE), %eax
    // Or EAX with the flags - present flag, writable flag.
    mov %eax, %es:0x1000(%di)
    // Store the value of EAX as the first PDPTE.
 
 
    // Build the Page Directory.
    lea %es:0x3000(%di), %eax
    // Put the address of the Page Table in to EAX.
    or $(PAGE_PRESENT | PAGE_WRITE), %eax
    // Or EAX with the flags - present flag, writeable flag.
    mov %eax, %es:0x2000(%di)
    // Store to value of EAX as the first PDE.
 
 
    push %di                           
    // Save DI for the time being.
    lea 0x3000(%di), %di
    // Point DI to the page table.
    mov $(PAGE_PRESENT | PAGE_WRITE), %eax
    // Move the flags into EAX - and point it to 0x0000.
 
 
    // Build the Page Table.
.LoopPageTable:
    mov %eax, %es:(%di)
    add $0x1000, %eax
    add $8, %di
    cmp $0x200000, %eax
    // If we did all 2MiB, end.
    jb .LoopPageTable
 
    pop %di
    // Restore DI.
 
    // Disable IRQs
    mov $0xFF, %al 
    // Out 0xFF to 0xA1 and 0x21 to disable all IRQs.
    out %al, $0xA1 
    out %al, $0x21 
 
    nop
    nop
 
    lidt IDT                        
    // Load a zero length IDT so that any NMI causes a triple fault.
 
    // Enter long mode.
    mov $0b10100000, %eax
    // Set the PAE and PGE bit.
    mov %eax, %cr4
 
    mov %edi, %edx                       
    // Point CR3 at the PML4.
    mov %edx, %cr3 
 
    mov $0xC0000080, %ecx
    // Read from the EFER MSR. 
    rdmsr    
 
    or $0x00000100, %eax                
    // Set the LME bit.
    wrmsr
 
    mov %cr0, %ebx                       
    // Activate long mode -
    or $0x80000001, %ebx
    // - by enabling paging and protection simultaneously.
    mov %ebx, %cr0
 
    lgdt GDT.Pointer                 
    //Load GDT.Pointer defined below.
 
    ljmp $CODE_SEG, $LongMode
    //CODE SEGMENT
    //Load CS with 64 bit segment and flush the instruction cache
 
 
    // Global Descriptor Table
GDT:
GDT.Null:
    .quad 0x0000000000000000             
    // Null Descriptor - should be present.
 
GDT.Code:
    .quad 0x00209A0000000000             
    // 64-bit code descriptor (exec/read).
    .quad 0x0000920000000000             
    // 64-bit data descriptor (read/write).
 
.align 4
    .word 0                              
    // Padding to make the "address of the GDT" field aligned on a 4-byte boundary
 
GDT.Pointer:
    .word . - GDT - 1                    
    // 16-bit Size (Limit) of GDT.
    .long GDT                            
    // 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)
 
 
.code64     
LongMode:
    mov $DATA_SEG, %ax
    mov %ax, %ds 
    mov %ax, %es 
    mov %ax, %fs 
    mov %ax, %gs 
    mov %ax, %ss 
 
    // Blank out the screen to a blue color.
    mov $0xB8000, %edi
    mov $500, %rcx                       
    // Since we are clearing uint64_t over here, we put the count as Count/4.
    mov $0x1F201F201F201F20, %rax        
    // Set the value to set the screen to: Blue background, white foreground, blank spaces.
    rep stosq                         
    // Clear the entire screen. 
 
    // Display "Hello World!"
    mov $0x00b8000, %edi
 
    mov $0x1F6C1F6C1F651F48, %rax
    mov %rax, (%edi)
 
    mov $0x1F6F1F571F201F6F, %rax
    mov %rax, 8(%edi)
 
    mov $0x1F211F641F6C1F72, %rax 
    mov %rax, 16(%edi)
 
    jmp Main.Long                     
    // You should replace this jump to wherever you want to jump to.

Main.Long:
     call cmain
////////////////////////////////////////////////////////////////
// sezione dati: tabelle e stack			      //
////////////////////////////////////////////////////////////////
.data
.balign 4096
gdt64:
	.quad 0		
    //segmento nullo
	.quad 0x209c0000000000 
    //segmento codice sistema
gdt64_end:
gdt64_ptr:
	.word gdt64_end - gdt64
	.long gdt64


	.global stack
stack:
	.space STACK_SIZE, 0
