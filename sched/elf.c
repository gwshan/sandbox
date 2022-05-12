/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "sandbox.h"

static int elf_handle_header(int fd, struct elf64_hdr *hdr)
{
	off_t offset;
	int ret;

	offset = lseek(fd, 0, SEEK_SET);
	if (offset < 0) {
		fprintf(stderr, "%s: Unable to seek header\n",
			__func__);
		return -EIO;
	}

	ret = read(fd, hdr, sizeof(*hdr));
	if (ret != sizeof(*hdr)) {
		fprintf(stderr, "%s: Unable to read header\n",
			__func__);
		return -EIO;
	}

	if (hdr->e_type != ELF64_HDR_TYPE_EXEC) {
		fprintf(stderr, "%s: Not a executable file (0x%04x)\n",
			__func__, hdr->e_type);
		return -EINVAL;
	}

	return 0;
}

static int elf_load_segments(struct kvm_vm *vm,
			     int fd,
			     struct elf64_hdr *hdr,
			     struct elf64_phdr *phdr)
{
	struct vm_area *vma;
	unsigned long phys, addr;
	off_t offset;
	int i, ret;

	/*
	 * There is no program headers, which shouldn't happen.
	 * However, it'd better to double check.
	 */
	if (!hdr->e_phnum)
		return 0;

	for (i = 0; i < hdr->e_phnum; i++) {
		offset = lseek(fd, hdr->e_phoff + i * hdr->phentsize, SEEK_SET);
		if (offset < 0) {
			fprintf(stderr, "%s: Unable seek program header %d\n",
				__func__, i);
			return -EIO;
		}

		ret = read(fd, phdr, sizeof(*phdr));
		if (ret != sizeof(*phdr)) {
			fprintf(stderr, "%s: Unable to read program header %d\n",
				__func__, i);
			return -EIO;
		}

		if (phdr->p_type != ELF64_PHDR_TYPE_LOAD)
			continue;

		phys = kvm_mm_alloc_phys_pages(vm,
				DIV_ROUND_UP(phdr->p_memsz, vm->mm.page_size));
		vma = mm_alloc_vma(vm->mm.mm, MM_VMA_FLAG_FIXED,
				   ALIGN_DOWN(phdr->p_vaddr, vm->mm.page_size),
				   ALIGN(phdr->p_memsz, vm->mm.page_size));
		kvm_mm_map(vm, phys, vma->start,
			   ALIGN(phdr->pmemsz, vm->mm.page_size));

		addr = kvm_mm_gpa_to_hva(vm, phys) +
		       (phdr->p_vaddr & (vm->mm.page_size - 1));
		offset = lseek(fd, phdr->p_offset, SEEK_SET);
		if (offset < 0) {
			fprintf(stderr, "%s: Unable to seek program segment %d\n",
				__func__, i);
			return -EIO;
		}

		ret = read(fd, (void *)addr, phdr->memsz);
		if (ret != phdr->p_memsz) {
			fprintf(stderr, "%s: Unable to read program segment %d\n",
				__func__, i);
			return -EIO;
		}
	}

	return 0;
}

int elf_load_file(struct kvm_vm *vm,
		  char *filename,
		  unsigned long *pentry)
{
	struct elf64_hdr *hdr = NULL;
	struct elf64_phdr *phdr = NULL;
	struct elf64_shdr *shdr = NULL;
	off_t offset;
	int fd = 0, ret;

	/* Open the file */
	fd = open(filename, O_RDWR);
	if (fd < 0) {
		fprintf(stder, "%s: Unable to open <%s>\n",
			__func__, filename);
		ret = fd;
		goto out;
	}

	/* Alloc various headers */
	hdr = malloc(sizeof(*hdr));
	phdr = malloc(sizeof(*phdr));
	shdr = malloc(sizeof(*shdr));
	if (!hdr || !phdr || !shdr) {
		fprintf(stderr, "%s: Unable to alloc headers\n",
			__func__);
		ret = -ENOMEM;
		goto out;
	}

	/* Read header and ensure it's a executable program */
	ret = elf_handle_header(fd, hdr);
	if (ret)
		goto out;

	/* Load program segments */
	ret = elf_load_segments(vm, fd, hdr, phdr);
	if (ret)
		goto out;

	/* FIXME: Relocation and resolution */  

	*p_entry = hdr->e_entry;
out:
	if (shdr)
		free(shdr);
	if (phdr)
		free(phdr);
	if (hdr)
		free(hdr);
	if (fd > 0)
		close(fd);

	return ret;
}
