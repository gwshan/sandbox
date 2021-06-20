/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_ELF_H
#define __SANDBOX_ELF_H

/* ELF64 header */
#define ELF64_HDR_TYPE_NONE	0
#define ELF64_HDR_TYPE_REL	1
#define ELF64_HDR_TYPE_EXEC	2
#define ELF64_HDR_TYPE_DYN	3
#define ELF64_HDR_TYPE_CORE	4

struct elf64_hdr {
	uint8_t   e_ident[16];	/* ELF magic number               */
	uint16_t  e_type;	/* Object file type               */
	uint16_t  e_machine;	/* The required architecture      */
	uint32_t  e_version;	/* Object file version            */
	uint64_t  e_entry;	/* Entry point virtual address    */
	uint64_t  e_phoff;	/* Program header table offset    */
	uint64_t  e_shoff;	/* Section header table offset    */
	uint32_t  e_flags;	/* Processor specific flags       */
	uint16_t  e_ehsize;	/* Size of ELF header             */ 
	uint16_t  e_phentsize;	/* Program header size            */
	uint16_t  e_phnum;	/* Program header entries         */
	uint16_t  e_shentsize;	/* Section header size            */
	uint16_t  e_shnum;	/* Section header entries         */
	uint16_t  e_shstrndx;	/* Section header index for names */
};

/* Program header */
#define ELF64_PHDR_TYPE_NULL		0
#define ELF64_PHDR_TYPE_LOAD		1
#define ELF64_PHDR_TYPE_DYNAMIC		2
#define ELF64_PHDR_TYPE_INTERP		3
#define ELF64_PHDR_TYPE_NOTE		4
#define ELF64_PHDR_TYPE_SHLIB		5
#define ELF64_PHDR_TYPE_PHDR		6
#define ELF64_PHDR_TYPE_GNU_EH_FRAME	0x6474e550
#define ELF64_PHDR_TYPE_GNU_STACK	0x6474e551
#define ELF64_PHDR_TYPE_GNU_RELRO	0x6474e552
#define ELF64_PHDR_TYPE_GNU_PROPERTY	0x6474e553

struct elf64_phdr {
	uint32_t  p_type;	/* Segment type             */
	uint32_t  p_flags;	/* Segment flags            */
	uint64_t  p_offset;	/* Segment offset in file   */
	uint64_t  p_vaddr;	/* Segment virtual address  */
	uint64_t  p_paddr;	/* Segment physical address */
	uint64_t  p_filesz;	/* Segment size in file     */
	uint64_t  p_memsz;	/* Segment size in memory   */
	uint64_t  p_align;	/* Segment alignment        */
};

/* Section header */
#define ELF64_SHDR_TYPE_NULL		0
#define ELF64_SHDR_TYPE_PROGBITS	1
#define ELF64_SHDR_TYPE_SYMTAB		2
#define ELF64_SHDR_TYPE_STRTAB		3
#define ELF64_SHDR_TYPE_RELA		4
#define ELF64_SHDR_TYPE_HASH		5
#define ELF64_SHDR_TYPE_DYNAMIC		6
#define ELF64_SHDR_TYPE_NOTE		7
#define ELF64_SHDR_TYPE_NOBITS		8
#define ELF64_SHDR_TYPE_REL		9
#define ELF64_SHDR_TYPE_SHLIB		10
#define ELF64_SHDR_TYPE_DYNSYM		11

struct elf64_shdr {
	uint32_t  sh_name;	/* Section name, index in string table */
	uint32_t  sh_type;	/* Section type                        */
	uint64_t  sh_flags;	/* Section flags                       */
	uint64_t  sh_addr;	/* Section virtual address             */
	uint64_t  sh_offset;	/* Section offset in file              */
	uint64_t  sh_size;	/* Section size                        */
	uint32_t  sh_link;	/* Linked section index                */
	uint32_t  sh_info;	/* Additional section information      */
	uint64_t  sh_addralign;	/* Section alignment                   */
	uint64_t  sh_entsize;	/* Entry size if section holds table   */
};

/* ELF64_SHDR_TYPE_SYMTAB */
#define ELF64_SYM_BIND(st_info)		((st_info) >> 4)
#define ELF64_SYM_TYPE(st_info)		((st_info) & 0xF)
#define ELF64_SYM_BIND_LOCAL		0
#define ELF64_SYM_BIND_GLOBAL		1
#define ELF64_SYM_BIND_WEAK		2
#define ELF64_SYM_TYPE_NOTYPE		0
#define ELF64_SYM_TYPE_OBJECT		1
#define ELF64_SYM_TYPE_FUNC		2
#define ELF64_SYM_TYPE_SECTION		3
#define ELF64_SYM_TYPE_FILE		4
#define ELF64_SYM_TYPE_COMMON		5
#define ELF64_SYM_TYPE_TLS		6

struct elf64_sym {
	uint32_t  st_name;	/* Symbol name, index in string table  */
	uint8_t   st_info;	/* Type and binding attributes         */
	uint8_t   st_other;	/* No defined meaning, 0               */
	uint16_t  st_shndx;	/* Associated section index            */
	uint64_t  st_value;	/* Value of the symbol                 */
	uint64_t  st_size;	/* Associated symbol size              */
};

/* ELF64_SHDR_TYPE_RELA */
#define ELF64_RELA_SYM(r_info)	((r_info) >> 32)
#define ELF64_RELA_TYPE(r_info)	((r_info) & 0xFFFFFFFF)

struct elf64_rela {
	uint64_t  r_offset;	/* Location at which to apply the action */
	uint64_t  r_info;	/* Relocation index and type             */
	uint64_t  r_addend;	/* Constant addend used to compute value */
};

/* ELF64_SHDR_TYPE_DYNAMIC */
#define ELF64_DYN_TAG_NULL		0
#define ELF64_DYN_TAG_NEEDED		1
#define ELF64_DYN_TAG_PLTRELSZ		2
#define ELF64_DYN_TAG_PLTGOT		3
#define ELF64_DYN_TAG_HASH		4
#define ELF64_DYN_TAG_STRTAB		5
#define ELF64_DYN_TAG_SYMTAB		6
#define ELF64_DYN_TAG_RELA		7
#define ELF64_DYN_TAG_RELASZ		8
#define ELF64_DYN_TAG_RELAENT		9
#define ELF64_DYN_TAG_STRSZ		10
#define ELF64_DYN_TAG_SYMENT		11
#define ELF64_DYN_TAG_INIT		12
#define ELF64_DYN_TAG_FINI		13
#define ELF64_DYN_TAG_SONAME		14
#define ELF64_DYN_TAG_RPATH		15
#define ELF64_DYN_TAG_SYMBOLIC		16
#define ELF64_DYN_TAG_REL		17
#define ELF64_DYN_TAG_RELSZ		18
#define ELF64_DYN_TAG_RELENT		19
#define ELF64_DYN_TAG_PLTREL		20
#define ELF64_DYN_TAG_DEBUG		21
#define ELF64_DYN_TAG_TEXTREL		22
#define ELF64_DYN_TAG_JMPREL		23
#define ELF64_DYN_TAG_ENCODING		32
#define ELF64_DYN_TAG_VALRNG_LO		0x6ffffd00
#define ELF64_DYN_TAG_VALRNG_HI		0x6ffffdff
#define ELF64_DYN_TAG_ADDRRNG_LO	0x6ffffe00
#define ELF64_DYN_TAG_ADDRRNG_HI	0x6ffffeff
#define ELF64_DYN_TAG_VERSYM		0x6ffffff0
#define ELF64_DYN_TAG_RELACOUNT		0x6ffffff9
#define ELF64_DYN_TAG_RELCOUNT		0x6ffffffa
#define ELF64_DYN_TAG_FLAGS_1		0x6ffffffb
#define ELF64_DYN_TAG_VERDEF		0x6ffffffc
#define ELF64_DYN_TAG_VERDEFNUM		0x6ffffffd
#define ELF64_DYN_TAG_VERNEED		0x6ffffffe
#define ELF64_DYN_TAG_VERNEEDNUM	0x6fffffff

struct elf64_dyn {
	uint64_t          d_tag;	/* Entry tag value              */
	union {
		uint64_t  d_val;
		uint64_t  d_ptr;
	};
};

/* ELF64_SHDR_TYPE_NOTE */
struct elf64_note {
	uint32_t  n_namesz;	/* Name size                             */
	uint32_t  n_descsz;	/* Content size                          */
	uint32_t  n_type;	/* Content type                          */
};

/* ELF64_SHDR_TYPE_REL */
#define ELF64_REL_SYM(r_info)	ELF64_RELA_SYM(r_info)
#define ELF64_REL_TYPE(r_info)	ELF64_RELA_TYPE(r_info)

struct elf64_rel {
	uint64_t  r_offset;	/* Location at which to apply the action */
	uint64_t  r_info;	/* Reloation index and type              */
};

#endif /* __SANDBOX_ELF_H */
