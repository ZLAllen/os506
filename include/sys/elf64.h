#ifndef _ELF64_H
#define _ELF64_H

#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/task_pool.h>

#define EI_NIDENT 16
#define MAX_ARG 5


// elf data types
typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Lword;
typedef uint64_t Elf64_Off;
typedef uint32_t Elf64_Sword;
typedef uint64_t Elf64_Sxword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;

// elf header
typedef struct {
  unsigned char e_ident[EI_NIDENT]; //decode and interpre file content
  Elf64_Half    e_type; //identify object file type
  Elf64_Half    e_machine; //required architecture for the file
  Elf64_Word    e_version; //object file version
  Elf64_Addr    e_entry; //vistrual address starting the process
  Elf64_Off     e_phoff; //program header table file offset in bytes
  Elf64_Off     e_shoff; //section header table file offset in bytes
  Elf64_Word    e_flags;
  Elf64_Half    e_ehsize; //elf header size in bytes
  Elf64_Half    e_phentsize;//size of one entry in program header table; all sizes same
  Elf64_Half    e_phnum;//num entries in the program header table
  Elf64_Half    e_shentsize;//section header size in bytes; all same
  Elf64_Half    e_shnum;//num entries in section header table
  Elf64_Half    e_shstrndx;// section name table
} Elf64_Ehdr;

// elf program header
typedef struct {
  Elf64_Word    p_type;
  Elf64_Word    p_flags;
  Elf64_Off     p_offset;
  Elf64_Addr    p_vaddr;
  Elf64_Addr    p_paddr;
  Elf64_Xword   p_filesz;
  Elf64_Xword   p_memsz;
  Elf64_Xword   p_align;
} Elf64_Phdr;

//elf section header
typedef struct elf64_shdr 
{
    Elf64_Word sh_name;       
    Elf64_Word sh_type;       
    Elf64_Xword sh_flags;     
    Elf64_Addr sh_addr;       
    Elf64_Off sh_offset;      
    Elf64_Xword sh_size;      
    Elf64_Word sh_link;       
    Elf64_Word sh_info;       
    Elf64_Xword sh_addralign; 
    Elf64_Xword sh_entsize;   
} Elf64_Shdr;


struct task_struct *create_elf_process(char *fname, char *argv[]);//elf process

#endif
