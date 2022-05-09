/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_KVM_H
#define __SANDBOX_KVM_H

struct kvm_vm {
	int		fd_dev;		/* Device fd */
	int		fd;		/* KVM fd    */

};

/* APIs */
struct kvm_vm *kvm_vm_create(void);

#endif /* __SANDBOX_KVM_H */

