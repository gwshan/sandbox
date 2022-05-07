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

/*
 * AARCH64: ELF64_{RELA, REL}_TYPE
 */
#define R_AARCH64_NONE            0   /* No relocation  */
/* ILP32 AArch64 relocs */
#define R_AARCH64_P32_ABS32       1   /* Direct 32 bit                  */
#define R_AARCH64_P32_COPY        180 /* Copy symbol at runtime         */
#define R_AARCH64_P32_GLOB_DAT	  181 /* Create GOT entry               */
#define R_AARCH64_P32_JUMP_SLOT	  182 /* Create PLT entry               */
#define R_AARCH64_P32_RELATIVE	  183 /* Adjust by program base         */
#define R_AARCH64_P32_TLS_DTPMOD  184 /* Module number, 32 bit          */
#define R_AARCH64_P32_TLS_DTPREL  185 /* Module-relative offset, 32 bit */
#define R_AARCH64_P32_TLS_TPREL	  186 /* TP-relative offset, 32 bit     */
#define R_AARCH64_P32_TLSDESC     187 /* TLS Descriptor                 */
#define R_AARCH64_P32_IRELATIVE	  188 /* STT_GNU_IFUNC relocation       */
/* LP64 AArch64 relocs */
#define R_AARCH64_ABS64               257 /* Direct 64 bit                     */
#define R_AARCH64_ABS32               258 /* Direct 32 bit                     */
#define R_AARCH64_ABS16               259 /* Direct 16-bit                     */
#define R_AARCH64_PREL64              260 /* PC-relative 64-bit                */
#define R_AARCH64_PREL32              261 /* PC-relative 32-bit                */
#define R_AARCH64_PREL16              262 /* PC-relative 16-bit                */
#define R_AARCH64_MOVW_UABS_G0        263 /* Dir. MOVZ imm. from bits 15:0     */
#define R_AARCH64_MOVW_UABS_G0_NC     264 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_UABS_G1        265 /* Dir. MOVZ imm. from bits 31:16    */
#define R_AARCH64_MOVW_UABS_G1_NC     266 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_UABS_G2        267 /* Dir. MOVZ imm. from bits 47:32    */
#define R_AARCH64_MOVW_UABS_G2_NC     268 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_UABS_G3        269 /* Dir. MOV{K,Z} imm. from 63:48     */
#define R_AARCH64_MOVW_SABS_G0        270 /* Dir. MOV{N,Z} imm. from 15:0      */
#define R_AARCH64_MOVW_SABS_G1        271 /* Dir. MOV{N,Z} imm. from 31:16     */
#define R_AARCH64_MOVW_SABS_G2        272 /* Dir. MOV{N,Z} imm. from 47:32     */
#define R_AARCH64_LD_PREL_LO19        273 /* PC-rel. LD imm. from bits 20:2    */
#define R_AARCH64_ADR_PREL_LO21       274 /* PC-rel. ADR imm. from bits 20:0   */
#define R_AARCH64_ADR_PREL_PG_HI21    275 /* Page-rel. ADRP imm. from 32:12    */
#define R_AARCH64_ADR_PREL_PG_HI21_NC 276 /* Likewise; no overflow check       */
#define R_AARCH64_ADD_ABS_LO12_NC     277 /* Dir. ADD imm. from bits 11:0      */
#define R_AARCH64_LDST8_ABS_LO12_NC   278 /* Likewise for LD/ST; no check      */
#define R_AARCH64_TSTBR14             279 /* PC-rel. TBZ/TBNZ imm. from 15:2   */
#define R_AARCH64_CONDBR19            280 /* PC-rel. cond. br. imm. from 20:2  */
#define R_AARCH64_JUMP26              282 /* PC-rel. B imm. from bits 27:2     */
#define R_AARCH64_CALL26              283 /* Likewise for CALL                 */
#define R_AARCH64_LDST16_ABS_LO12_NC  284 /* Dir. ADD imm. from bits 11:1      */
#define R_AARCH64_LDST32_ABS_LO12_NC  285 /* Likewise for bits 11:2            */
#define R_AARCH64_LDST64_ABS_LO12_NC  286 /* Likewise for bits 11:3            */
#define R_AARCH64_MOVW_PREL_G0        287 /* PC-rel. MOV{N,Z} imm. from 15:0   */
#define R_AARCH64_MOVW_PREL_G0_NC     288 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_PREL_G1        289 /* PC-rel. MOV{N,Z} imm. from 31:16  */
#define R_AARCH64_MOVW_PREL_G1_NC     290 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_PREL_G2        291 /* PC-rel. MOV{N,Z} imm. from 47:32  */
#define R_AARCH64_MOVW_PREL_G2_NC     292 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_PREL_G3        293 /* PC-rel. MOV{N,Z} imm. from 63:48  */
#define R_AARCH64_LDST128_ABS_LO12_NC 299 /* Dir. ADD imm. from bits 11:4      */
#define R_AARCH64_MOVW_GOTOFF_G0      300 /* GOT-rel. off. MOV{N,Z} imm. 15:0  */
#define R_AARCH64_MOVW_GOTOFF_G0_NC   301 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_GOTOFF_G1      302 /* GOT-rel. o. MOV{N,Z} imm. 31:16   */
#define R_AARCH64_MOVW_GOTOFF_G1_NC   303 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_GOTOFF_G2      304 /* GOT-rel. o. MOV{N,Z} imm. 47:32   */
#define R_AARCH64_MOVW_GOTOFF_G2_NC   305 /* Likewise for MOVK; no check       */
#define R_AARCH64_MOVW_GOTOFF_G3      306 /* GOT-rel. o. MOV{N,Z} imm. 63:48   */
#define R_AARCH64_GOTREL64            307 /* GOT-relative 64-bit               */
#define R_AARCH64_GOTREL32            308 /* GOT-relative 32-bit               */
#define R_AARCH64_GOT_LD_PREL19       309 /* PC-rel. GOT off. load imm. 20:2   */
#define R_AARCH64_LD64_GOTOFF_LO15    310 /* GOT-rel. off. LD/ST imm. 14:3     */
#define R_AARCH64_ADR_GOT_PAGE        311 /* P-page-rel. GOT off. ADRP 32:12   */
#define R_AARCH64_LD64_GOT_LO12_NC    312 /* Dir. GOT off. LD/ST imm. 11:3     */
#define R_AARCH64_LD64_GOTPAGE_LO15   313 /* GOT-page-rel. GOT off. LD/ST 14:3 */
#define R_AARCH64_TLSGD_ADR_PREL21             512 /* PC-relative ADR imm. 20:0     */
#define R_AARCH64_TLSGD_ADR_PAGE21             513 /* page-rel. ADRP imm. 32:12     */
#define R_AARCH64_TLSGD_ADD_LO12_NC            514 /* direct ADD imm. from 11:0     */
#define R_AARCH64_TLSGD_MOVW_G1                515 /* GOT-rel. MOV{N,Z} 31:16       */
#define R_AARCH64_TLSGD_MOVW_G0_NC             516 /* GOT-rel. MOVK imm. 15:0       */
#define R_AARCH64_TLSLD_ADR_PREL21             517 /* Like 512; local dynamic model */
#define R_AARCH64_TLSLD_ADR_PAGE21             518 /* Like 513; local dynamic model */
#define R_AARCH64_TLSLD_ADD_LO12_NC            519 /* Like 514; local dynamic model */
#define R_AARCH64_TLSLD_MOVW_G1                520 /* Like 515; local dynamic model */
#define R_AARCH64_TLSLD_MOVW_G0_NC             521 /* Like 516; local dynamic model */
#define R_AARCH64_TLSLD_LD_PREL19              522 /* TLS PC-rel. load imm. 20:2    */
#define R_AARCH64_TLSLD_MOVW_DTPREL_G2         523 /* TLS DTP-rel. MOV{N,Z} 47:32   */
#define R_AARCH64_TLSLD_MOVW_DTPREL_G1         524 /* TLS DTP-rel. MOV{N,Z} 31:16   */
#define R_AARCH64_TLSLD_MOVW_DTPREL_G1_NC      525 /* Likewise; MOVK; no check      */
#define R_AARCH64_TLSLD_MOVW_DTPREL_G0         526 /* TLS DTP-rel. MOV{N,Z} 15:0    */
#define R_AARCH64_TLSLD_MOVW_DTPREL_G0_NC      527 /* Likewise; MOVK; no check      */
#define R_AARCH64_TLSLD_ADD_DTPREL_HI12        528 /* DTP-rel. ADD imm. from 23:12  */
#define R_AARCH64_TLSLD_ADD_DTPREL_LO12        529 /* DTP-rel. ADD imm. from 11:0   */
#define R_AARCH64_TLSLD_ADD_DTPREL_LO12_NC     530 /* Likewise; no ovfl. check      */
#define R_AARCH64_TLSLD_LDST8_DTPREL_LO12      531 /* DTP-rel. LD/ST imm. 11:0      */
#define R_AARCH64_TLSLD_LDST8_DTPREL_LO12_NC   532 /* Likewise; no check            */
#define R_AARCH64_TLSLD_LDST16_DTPREL_LO12     533 /* DTP-rel. LD/ST imm. 11:1      */
#define R_AARCH64_TLSLD_LDST16_DTPREL_LO12_NC  534 /* Likewise; no check            */
#define R_AARCH64_TLSLD_LDST32_DTPREL_LO12     535 /* DTP-rel. LD/ST imm. 11:2      */
#define R_AARCH64_TLSLD_LDST32_DTPREL_LO12_NC  536 /* Likewise; no check            */
#define R_AARCH64_TLSLD_LDST64_DTPREL_LO12     537 /* DTP-rel. LD/ST imm. 11:3      */
#define R_AARCH64_TLSLD_LDST64_DTPREL_LO12_NC  538 /* Likewise; no check            */
#define R_AARCH64_TLSIE_MOVW_GOTTPREL_G1       539 /* GOT-rel. MOV{N,Z} 31:16       */
#define R_AARCH64_TLSIE_MOVW_GOTTPREL_G0_NC    540 /* GOT-rel. MOVK 15:0            */
#define R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21    541 /* Page-rel. ADRP 32:12          */
#define R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC  542 /* Direct LD off. 11:3           */
#define R_AARCH64_TLSIE_LD_GOTTPREL_PREL19     543 /* PC-rel. load imm. 20:2        */
#define R_AARCH64_TLSLE_MOVW_TPREL_G2          544 /* TLS TP-rel. MOV{N,Z} 47:32    */
#define R_AARCH64_TLSLE_MOVW_TPREL_G1          545 /* TLS TP-rel. MOV{N,Z} 31:16    */
#define R_AARCH64_TLSLE_MOVW_TPREL_G1_NC       546 /* Likewise; MOVK; no check      */
#define R_AARCH64_TLSLE_MOVW_TPREL_G0          547 /* TLS TP-rel. MOV{N,Z} 15:0     */
#define R_AARCH64_TLSLE_MOVW_TPREL_G0_NC       548 /* Likewise; MOVK; no check      */
#define R_AARCH64_TLSLE_ADD_TPREL_HI12         549 /* TP-rel. ADD imm. 23:12        */
#define R_AARCH64_TLSLE_ADD_TPREL_LO12         550 /* TP-rel. ADD imm. 11:0         */
#define R_AARCH64_TLSLE_ADD_TPREL_LO12_NC      551 /* Likewise; no ovfl. check      */
#define R_AARCH64_TLSLE_LDST8_TPREL_LO12       552 /* TP-rel. LD/ST off. 11:0       */
#define R_AARCH64_TLSLE_LDST8_TPREL_LO12_NC    553 /* Likewise; no ovfl. check      */
#define R_AARCH64_TLSLE_LDST16_TPREL_LO12      554 /* TP-rel. LD/ST off. 11:1       */
#define R_AARCH64_TLSLE_LDST16_TPREL_LO12_NC   555 /* Likewise; no check            */
#define R_AARCH64_TLSLE_LDST32_TPREL_LO12      556 /* TP-rel. LD/ST off. 11:2       */
#define R_AARCH64_TLSLE_LDST32_TPREL_LO12_NC   557 /* Likewise; no check            */
#define R_AARCH64_TLSLE_LDST64_TPREL_LO12      558 /* TP-rel. LD/ST off. 11:3       */
#define R_AARCH64_TLSLE_LDST64_TPREL_LO12_NC   559 /* Likewise; no check            */
#define R_AARCH64_TLSDESC_LD_PREL19            560 /* PC-rel. load immediate 20:2   */
#define R_AARCH64_TLSDESC_ADR_PREL21           561 /* PC-rel. ADR immediate 20:0    */
#define R_AARCH64_TLSDESC_ADR_PAGE21           562 /* Page-rel. ADRP imm. 32:12     */
#define R_AARCH64_TLSDESC_LD64_LO12            563 /* Direct LD off. from 11:3      */
#define R_AARCH64_TLSDESC_ADD_LO12             564 /* Direct ADD imm. from 11:0     */
#define R_AARCH64_TLSDESC_OFF_G1               565 /* GOT-rel. MOV{N,Z} imm. 31:16  */
#define R_AARCH64_TLSDESC_OFF_G0_NC            566 /* GOT-rel. MOVK imm. 15:0; no ck */
#define R_AARCH64_TLSDESC_LDR                  567 /* Relax LDR                     */
#define R_AARCH64_TLSDESC_ADD                  568 /* Relax ADD                     */
#define R_AARCH64_TLSDESC_CALL                 569 /* Relax BLR                     */
#define R_AARCH64_TLSLE_LDST128_TPREL_LO12     570 /* TP-rel. LD/ST off. 11:4       */
#define R_AARCH64_TLSLE_LDST128_TPREL_LO12_NC  571 /* Likewise; no check            */
#define R_AARCH64_TLSLD_LDST128_DTPREL_LO12    572 /* DTP-rel. LD/ST imm. 11:4      */
#define R_AARCH64_TLSLD_LDST128_DTPREL_LO12_NC 573 /* Likewise; no check            */
#define R_AARCH64_COPY       1024 /* Copy symbol at runtime         */
#define R_AARCH64_GLOB_DAT   1025 /* Create GOT entry               */
#define R_AARCH64_JUMP_SLOT  1026 /* Create PLT entry               */
#define R_AARCH64_RELATIVE   1027 /* Adjust by program base         */
#define R_AARCH64_TLS_DTPMOD 1028 /* Module number, 64 bit          */
#define R_AARCH64_TLS_DTPREL 1029 /* Module-relative offset, 64 bit */
#define R_AARCH64_TLS_TPREL  1030 /* TP-relative offset, 64 bit     */
#define R_AARCH64_TLSDESC    1031 /* TLS Descriptor                 */
#define R_AARCH64_IRELATIVE  1032 /* STT_GNU_IFUNC relocation       */

#endif /* __SANDBOX_ELF_H */
