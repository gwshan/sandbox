/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "sandbox.h"

int main(int argc, char **argv)
{
	struct kvm_vm *vm;
	unsigned long entry_point;
	int ret;

	vm = kvm_vm_create();
	if (!vm)
		return -ENOMEM;

	ret = elf_load_file(vm, "/tmp/debug", &entry_point);
	if (ret)
		goto error;

	ret = kvm_vcpu_create(vm, entry_point);
	if (ret)
		goto error;

error:
	kvm_vm_destroy(vm);
	return ret;
}
