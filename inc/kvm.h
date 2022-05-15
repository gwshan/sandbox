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

#include "linux-headers/kvm.h"

#define KVM_ARM64_CORE_REG(x)				\
	(KVM_REG_ARM64 | KVM_REG_SIZE_U64 |		\
	 KVM_REG_ARM_CORE | KVM_REG_ARM_CORE_REG(x))
#define KVM_ARM64_SYS_REG(id)				\
	ARM64_SYS_REG(SYS_REG_OP0(id), SYS_REG_OP1(id),	\
		      SYS_REG_CRN(id), SYS_REG_CRM(id),	\
		      SYS_REG_OP2(id))

struct kvm_vm_mm {
	struct mm	*mm;		/* Memory management struct	*/
	unsigned long	pa_bits;	/* Physical memory bits		*/
	unsigned long	va_bits;	/* Virtual memory bits		*/
	unsigned long	page_size;	/* Page size			*/
	unsigned long	page_shift;	/* Page shift			*/

	unsigned long	pgtable;	/* Page table			*/
	unsigned long	pgtable_levels;	/* Page table levels		*/

	unsigned long	phys_page_base;	/* Start PFN			*/
	unsigned long	phys_page_num;	/* Number of physical pages	*/
	unsigned long	*phys_page_bits; /* Free page bitmap		*/
	void		*host_virt_addr; /* Host virtual address		*/
};

struct kvm_vcpu {
	struct kvm_vm		*vm;		/* Associated VM	*/
	int			fd;		/* FD                   */
	unsigned int		id;		/* vCPU ID		*/	
	struct kvm_run		*state;		/* Running state	*/
	unsigned long		state_size;	/* Running state size	*/

	unsigned long		entry_point;	/* PC for execution	*/
	unsigned long		stack_base;	/* Stack base address	*/
	unsigned long		stack_end;	/* Stack end address	*/
	struct list_head	link;
};

struct kvm_vm {
	int			fd_dev;		/* Device fd		*/
	int			fd;		/* KVM fd		*/

	struct kvm_vm_mm	mm;		/* Memory management	*/
	struct list_head	vcpu_list;	/* List of vCPUs	*/
};

/* APIs */
struct kvm_vm *kvm_vm_create(void);
int kvm_vcpu_create(struct kvm_vm *vm, unsigned long entry_point);
int kvm_vcpu_get_reg(struct kvm_vcpu *vcpu, unsigned long id,
		     unsigned long *val);
int kvm_vcpu_set_reg(struct kvm_vcpu *vcpu, unsigned long id,
		     unsigned long val);
void kvm_vcpu_destroy(struct kvm_vcpu *vcpu);
void kvm_vm_destroy(struct kvm_vm *vm);

/* Memory management */
unsigned long kvm_mm_gpa_to_hva(struct kvm_vm *vm, unsigned long gpa);
unsigned long kvm_mm_alloc_phys_pages(struct kvm_vm *vm,
				      unsigned long npages);
void kvm_mm_map(struct kvm_vm *vm, unsigned long phys,
		unsigned long virt, unsigned long len);

#endif /* __SANDBOX_KVM_H */

