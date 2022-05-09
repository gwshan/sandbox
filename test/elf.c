/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "sandbox.h"

#define TEST_ELF_FILENAME		"/tmp/debug"
#define ELF64_SHDR_TYPE_DEFAULT		ELF64_SHDR_TYPE_RELA

static uint32_t elf64_shdr_type = ELF64_SHDR_TYPE_DEFAULT;

static void dump_elf64_hdr(struct elf64_hdr *hdr)
{
	fprintf(stdout, "\n");
	fprintf(stdout, "========== ELF64 Header ==========\n");
	fprintf(stdout, "\n");

	fprintf(stdout, "e_ident:        %02x %02x %02x %02x\n",
		hdr->e_ident[0], hdr->e_ident[1],
		hdr->e_ident[2], hdr->e_ident[3]);
	fprintf(stdout, "                %02x %02x %02x %02x\n",
		hdr->e_ident[4], hdr->e_ident[5],
		hdr->e_ident[6], hdr->e_ident[7]);
	fprintf(stdout, "                %02x %02x %02x %02x\n",
		hdr->e_ident[8], hdr->e_ident[9],
		hdr->e_ident[10], hdr->e_ident[11]);
	fprintf(stdout, "                %02x %02x %02x %02x\n",
		hdr->e_ident[12], hdr->e_ident[13],
		hdr->e_ident[14], hdr->e_ident[15]);
	fprintf(stdout, "e_type:         %04x\n",    hdr->e_type);
	fprintf(stdout, "e_machine:      %04x\n",    hdr->e_machine);
	fprintf(stdout, "e_version:      %08x\n",    hdr->e_version);
	fprintf(stdout, "e_entry:        %016llx\n", hdr->e_entry);
	fprintf(stdout, "e_phoff:        %016llx\n", hdr->e_phoff);
	fprintf(stdout, "e_shoff:        %016llx\n", hdr->e_shoff);
	fprintf(stdout, "e_flags:        %08x\n",    hdr->e_flags);
	fprintf(stdout, "e_ehsize:       %04x\n",    hdr->e_ehsize);
	fprintf(stdout, "e_phentsize:    %04x\n",    hdr->e_phentsize);
	fprintf(stdout, "e_phnum:        %04x\n",    hdr->e_phnum);
	fprintf(stdout, "e_shentsize:    %04x\n",    hdr->e_shentsize);
	fprintf(stdout, "e_shnum:        %04x\n",    hdr->e_shnum);
	fprintf(stdout, "e_shstrndx:     %04x\n",    hdr->e_shstrndx);

	fprintf(stdout, "\n");
}

static void dump_elf64_shdr(struct elf64_hdr *hdr,
			    struct elf64_shdr *shdr)
{
	struct elf64_shdr *str_shdr;
	char *name;

	str_shdr = (void *)hdr + hdr->e_shoff +
		   hdr->e_shentsize * hdr->e_shstrndx;
	name = (char *)hdr + str_shdr->sh_offset + shdr->sh_name;

	fprintf(stdout, "\n");
	fprintf(stdout, "========== ELF64 Section Header ==========\n");
	fprintf(stdout, "\n");

	fprintf(stdout, "sh_name:        %08x          %s\n",
		shdr->sh_name, name);
	fprintf(stdout, "sh_type:        %08x\n",    shdr->sh_type);
	fprintf(stdout, "sh_flags:       %016llx\n", shdr->sh_flags);
	fprintf(stdout, "sh_addr:        %016llx\n", shdr->sh_addr);
	fprintf(stdout, "sh_offsset:     %016llx\n", shdr->sh_offset);
	fprintf(stdout, "sh_size:        %016llx\n", shdr->sh_size);
	fprintf(stdout, "sh_link:        %08x\n",    shdr->sh_link);
	fprintf(stdout, "sh_info:        %08x\n",    shdr->sh_info);
	fprintf(stdout, "sh_addralign:   %016llx\n", shdr->sh_addralign);
	fprintf(stdout, "sh_entsize:     %016llx\n", shdr->sh_entsize);

	fprintf(stdout, "\n");
}

static void dump_elf64_section_symtab(struct elf64_hdr *hdr,
				      struct elf64_shdr *shdr)
{
	struct elf64_shdr *str_shdr;
	struct elf64_sym *start_sym, *end_sym, *sym;
	const char *binds[] = { "Local ", "Global", "Weak  " };
	const char *types[] = { "Notype ", "Object ", "Func   ",
                                "Section", "File   ", "Common ",
                                "TLS    " };
	char *name;
	int i;

	/*
	 * The symbol's name needs to be searched in the linked section,
	 * which is usually ".strtab".
	 */
	str_shdr = (void *)hdr + hdr->e_shoff +
		   hdr->e_shentsize * shdr->sh_link;
	name = (char *)hdr + str_shdr->sh_offset;

	start_sym = (void *)hdr + shdr->sh_offset;
	end_sym = (void *)hdr + shdr->sh_offset + shdr->sh_size;
	sym = start_sym;

	/* @st_name is the index of character, instead of string */
	while (sym < end_sym) {
		fprintf(stdout, "st_name:        %08x  %s\n",
			sym->st_name, &name[sym->st_name]);
		fprintf(stdout, "st_info:        %02x        %s  %s\n",
			sym->st_info,
			binds[ELF64_SYM_BIND(sym->st_info)],
			types[ELF64_SYM_TYPE(sym->st_info)]);
		fprintf(stdout, "st_other:       %02x\n",
			sym->st_other);
		fprintf(stdout, "st_shndx:       %04x\n",
			sym->st_shndx);
		fprintf(stdout, "st_value:       %016llx\n",
			sym->st_value);
		fprintf(stdout, "st_size:        %016llx\n",
			sym->st_size);
		fprintf(stdout, "\n");

		sym = (void *)sym + shdr->sh_entsize;
	}
}

static void dump_elf64_section_rela(struct elf64_hdr *hdr,
				    struct elf64_shdr *shdr)
{
	struct elf64_shdr *sym_shdr, *str_shdr;
	struct elf64_rela *rela, *end;
	struct elf64_sym *sym;
	char *name;

	/* The ".dynsym" and its linked string (".dynstr") section header */
	sym_shdr = (void *)hdr + hdr->e_shoff +
		   shdr->sh_link * hdr->e_shentsize;
	str_shdr = (void *)hdr + hdr->e_shoff +
		   sym_shdr->sh_link * hdr->e_shentsize;

	rela = (void *)hdr + shdr->sh_offset;
	end   = (void *)rela + shdr->sh_size;

	/*
	 * The name is searched from the linked section of ".dynsym",
	 * instead of the one specified by struct elf64_sym::st_shndx.
	 */
	while (rela < end) {
		sym = (void *)hdr + sym_shdr->sh_offset +
		      ELF64_RELA_SYM(rela->r_info) * sym_shdr->sh_entsize;
		name = (void *)hdr + str_shdr->sh_offset + sym->st_name;

		fprintf(stdout, "r_offset:       %016llx\n",
			rela->r_offset);
		fprintf(stdout, "r_info:         %016llx  %08x  %08x  %s\n",
			rela->r_info,
			ELF64_RELA_TYPE(rela->r_info),
			ELF64_RELA_SYM(rela->r_info),
			name);	
		fprintf(stdout, "r_addend:       %016llx\n",
			rela->r_addend);
		fprintf(stdout, "\n");

		rela = (void *)rela + shdr->sh_entsize;
	}
}

static void dump_elf64_section_dynamic(struct elf64_hdr *hdr,
				       struct elf64_shdr *shdr)
{
	struct elf64_shdr *str_shdr;
	struct elf64_dyn *dyn, *end;

	str_shdr = (void *)hdr + hdr->e_shoff +
		   shdr->sh_link * hdr->e_shentsize;
	dyn = (void *)hdr + shdr->sh_offset;
	end = (void *)dyn + shdr->sh_size;

	while (dyn < end) {
		fprintf(stdout, "d_tag:          %016llx\n", dyn->d_tag);
		fprintf(stdout, "d_val:          %016llx\n", dyn->d_val);
		fprintf(stdout, "\n");

		dyn = (void *)dyn + shdr->sh_entsize;
	}
}

static void dump_elf64_section_rel(struct elf64_hdr *hdr,
				   struct elf64_shdr *shdr)
{
	struct elf64_shdr *sym_shdr, *str_shdr;
	struct elf64_rel *rel, *end;
	struct elf64_sym *sym;
	char *name;

	/* The ".dynsym" and its linked string (".dynstr") section header */
	sym_shdr = (void *)hdr + hdr->e_shoff +
		   shdr->sh_link * hdr->e_shentsize;
	str_shdr = (void *)hdr + hdr->e_shoff +
		   sym_shdr->sh_link * hdr->e_shentsize;

	rel = (void *)hdr + shdr->sh_offset;
	end = (void *)rel + shdr->sh_size;

	/*
	 * The name is searched from the linked section of ".dynsym",
	 * instead of the one specified by struct elf64_sym::st_shndx.
	 */
	while (rel < end) {
		sym = (void *)hdr + sym_shdr->sh_offset +
		      ELF64_RELA_SYM(rel->r_info) * sym_shdr->sh_entsize;
		name = (void *)hdr + str_shdr->sh_offset + sym->st_name;

		fprintf(stdout, "r_offset:       %016llx\n",
			rel->r_offset);
		fprintf(stdout, "r_info:         %016llx  %08x  %08x  %s\n",
			rel->r_info,
			ELF64_RELA_TYPE(rel->r_info),
			ELF64_RELA_SYM(rel->r_info),
			name);	
		fprintf(stdout, "\n");

		rel = (void *)rel + shdr->sh_entsize;
	}
}

static void dump_elf64_section_dyndym(struct elf64_hdr *hdr,
				      struct elf64_shdr *shdr)
{
	struct elf64_shdr *str_shdr;
	struct elf64_sym *sym, *end;
	char *name;
	const char *binds[] = { "Local ", "Global", "Weak  " };
	const char *types[] = { "Notype ", "Object ", "Func   ",
				"Section", "File   ", "Common ",
				"TLS    " };

	str_shdr = (void *)hdr + hdr->e_shoff +
		   shdr->sh_link * hdr->e_shentsize;
	name = (void *)hdr + str_shdr->sh_offset;
	sym = (void *)hdr + shdr->sh_offset;
	end = (void *)sym + shdr->sh_size;

	while (sym < end) {
		fprintf(stdout, "st_name:        %08x  %s\n",
			sym->st_name,
			&name[sym->st_name]);
		fprintf(stdout, "st_info:        %02x        %s  %s\n",
			sym->st_info,
			binds[ELF64_SYM_BIND(sym->st_info)],
			types[ELF64_SYM_TYPE(sym->st_info)]);
		fprintf(stdout, "st_other:       %02x\n",
			sym->st_other);
		fprintf(stdout, "st_shndx:       %04x\n",
			sym->st_shndx);
		fprintf(stdout, "st_value:       %016llx\n",
			sym->st_value);
		fprintf(stdout, "st_size:        %016llx\n",
			sym->st_size);
		fprintf(stdout, "\n");

		sym = (void *)sym + shdr->sh_entsize;
	}
}

int main(int argc, char **argv)
{
	struct elf64_hdr *hdr;
	struct elf64_shdr *shdr;
	struct stat st;
	int fd, i, ret;

	/* Parse the requested section header */
	if (argc > 1) {
		elf64_shdr_type = atoi(argv[1]);
		if (elf64_shdr_type > ELF64_SHDR_TYPE_DYNSYM) {
			fprintf(stderr, "%s: Invalid section header type (%d)\n",
				__func__, elf64_shdr_type);
			elf64_shdr_type = ELF64_SHDR_TYPE_DEFAULT;
		}
	}

	/* Open file and map it */
	fd = open(TEST_ELF_FILENAME, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "%s: Unable to open <%s>\n",
			__func__, TEST_ELF_FILENAME);
		return fd;
	}

	ret = fstat(fd, &st);
	if (ret) {
		fprintf(stderr, "%s: Error %d from fstat()\n",
			__func__, ret);
		return ret;
	}

	hdr = (struct elf64_hdr *)mmap(NULL, st.st_size, PROT_READ,
				       MAP_SHARED, fd, 0);
	if (hdr == MAP_FAILED) {
		fprintf(stderr, "%s: Unable to map file <%s>\n",
			__func__, TEST_ELF_FILENAME);
		return -ENOMEM;
	}

	/* Dump header */
	dump_elf64_hdr(hdr);

	/* Iterate section headers */
	shdr = (void *)hdr + hdr->e_shoff;
	for (i = 0; i < hdr->e_shnum; shdr = (void *)shdr + hdr->e_shentsize, i++) {
		if (shdr->sh_type != elf64_shdr_type)
			continue;

		/* Dump section header */
		dump_elf64_shdr(hdr, shdr);

		/* Dump section */
		switch (shdr->sh_type) {
		case ELF64_SHDR_TYPE_NULL:
		case ELF64_SHDR_TYPE_PROGBITS:
		case ELF64_SHDR_TYPE_HASH:
		case ELF64_SHDR_TYPE_NOTE:
		case ELF64_SHDR_TYPE_SHLIB:
		case ELF64_SHDR_TYPE_NOBITS:
			break;
		case ELF64_SHDR_TYPE_SYMTAB:
			dump_elf64_section_symtab(hdr, shdr);
			break;
		case ELF64_SHDR_TYPE_RELA:
			dump_elf64_section_rela(hdr, shdr);
			break;
		case ELF64_SHDR_TYPE_DYNAMIC:
			dump_elf64_section_dynamic(hdr, shdr);
			break;
		case ELF64_SHDR_TYPE_REL:
			dump_elf64_section_rel(hdr, shdr);
			break;
		case ELF64_SHDR_TYPE_DYNSYM:
			dump_elf64_section_dyndym(hdr, shdr);
			break;
		default:
			break;
		}
	}

	return 0;
}
