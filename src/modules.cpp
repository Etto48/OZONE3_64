#include "include/modules.h"

void* load_module(multiboot_module_t* mod)
{
    auto elf_h = (Elf64_Ehdr*)(uint64_t)mod->mod_start;

    //check if elf
    if (!(elf_h->e_ident[EI_MAG0] == ELFMAG0 &&
	      elf_h->e_ident[EI_MAG1] == ELFMAG1 &&
	      elf_h->e_ident[EI_MAG2] == ELFMAG2 &&
	      elf_h->e_ident[EI_MAG2] == ELFMAG2))
	{
		//module is not elf
		return 0;
	}

    if (!(elf_h->e_ident[EI_CLASS] == ELFCLASS64  &&
	      elf_h->e_ident[EI_DATA]  == ELFDATA2LSB &&
	      elf_h->e_type	       == ET_EXEC     &&
	      elf_h->e_machine 	       == EM_AMD64))
	{ 
		//module is not x86_64 executable
		return 0;
	}

    Elf64_Phdr* elf_ph = (Elf64_Phdr*)(mod->mod_start + elf_h->e_phoff);
    for (int i = 0; i < elf_h->e_phnum; i++) {
		if (elf_ph->p_type != PT_LOAD)
			continue;

		memory::memcpy((void*)elf_ph->p_vaddr,
		       (void*)(mod->mod_start + elf_ph->p_offset),
		       elf_ph->p_filesz);
        //copy segment into memory
		if (elf_ph->p_memsz != elf_ph->p_filesz) {
			memory::memset((void*)(elf_ph->p_vaddr + elf_ph->p_filesz), 0,
			       elf_ph->p_memsz - elf_ph->p_filesz);
			//clear memory
		}
		elf_ph = (Elf64_Phdr*)((uint64_t)elf_ph +
			elf_h->e_phentsize);
	}
	//loaded
	return (void*)elf_h->e_entry;

}