/*
 * Copyright 2022 Gavin Shan <gshan@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __SANDBOX_ARM64_SYSREG_H
#define __SANDBOX_ARM64_SYSREG_H

/*
 * According to the ARMv8 specification, the system registers are encoded
 * like below. More details can be found from the section "System Instruction
 * class encoding overview".
 *
 * [20-19] : Op0
 * [18-16] : Op1
 * [15-12] : CRn
 * [11-8 ] : CRm
 * [ 7-5 ] : Op2
 */
#define SYS_REG_OP0_SHIFT	19
#define SYS_REG_OP0_MASK	0x3
#define SYS_REG_OP1_SHIFT	16
#define SYS_REG_OP1_MASK	0x7
#define SYS_REG_CRN_SHIFT	12
#define SYS_REG_CRN_MASK	0xf
#define SYS_REG_CRM_SHIFT	8
#define SYS_REG_CRM_MASK	0xf
#define SYS_REG_OP2_SHIFT	5
#define SYS_REG_OP2_MASK	0x7

#define SYS_REG_READ(r)		({			\
	uint64_t __val__;				\
	__asm__ volatile ("mrs %0, " __stringify(r) :	\
			  "=r" (__val__));		\
	__val__;					\
})
#define SYS_REG_WRITE(r, v)	do {			\
	uint64_t __val__ = (uint64_t)(v);		\
	__asm__ volatile ("msr " __stringify(r) ", %x0"	\
			  : : "rZ" (__val__));		\
} while (0)

#define SYS_REG_OP0(id)	(((id) >> SYS_REG_OP0_SHIFT) & SYS_REG_OP0_MASK)
#define SYS_REG_OP1(id)	(((id) >> SYS_REG_OP1_SHIFT) & SYS_REG_OP1_MASK)
#define SYS_REG_CRN(id)	(((id) >> SYS_REG_CRN_SHIFT) & SYS_REG_CRN_MASK)
#define SYS_REG_CRM(id)	(((id) >> SYS_REG_CRM_SHIFT) & SYS_REG_CRM_MASK)
#define SYS_REG_OP2(id)	(((id) >> SYS_REG_OP2_SHIFT) & SYS_REG_OP2_MASK)

#define SYS_REG(op0, op1, crn, crm, op2)			\
	((((op0) & SYS_REG_OP0_MASK) << SYS_REG_OP0_SHIFT) |	\
	 (((op1) & SYS_REG_OP1_MASK) << SYS_REG_OP1_SHIFT) |	\
	 (((crn) & SYS_REG_CRN_MASK) << SYS_REG_CRN_SHIFT) |	\
	 (((crm) & SYS_REG_CRM_MASK) << SYS_REG_CRM_SHIFT) |	\
	 (((op2) & SYS_REG_OP2_MASK) << SYS_REG_OP2_SHIFT))

#define SYS_REG_OSDTRRX_EL1		SYS_REG(2, 0, 0, 0, 2)
#define SYS_REG_MDCCINT_EL1		SYS_REG(2, 0, 0, 2, 0)
#define SYS_REG_MDSCR_EL1		SYS_REG(2, 0, 0, 2, 2)
#define SYS_REG_OSDTRTX_EL1		SYS_REG(2, 0, 0, 3, 2)
#define SYS_REG_OSECCR_EL1		SYS_REG(2, 0, 0, 6, 2)
#define SYS_REG_DBGBVRn_EL1(n)		SYS_REG(2, 0, 0, n, 4)
#define SYS_REG_DBGBCRn_EL1(n)		SYS_REG(2, 0, 0, n, 5)
#define SYS_REG_DBGWVRn_EL1(n)		SYS_REG(2, 0, 0, n, 6)
#define SYS_REG_DBGWCRn_EL1(n)		SYS_REG(2, 0, 0, n, 7)
#define SYS_REG_MDRAR_EL1		SYS_REG(2, 0, 1, 0, 0)
#define SYS_REG_OSLAR_EL1		SYS_ERG(2, 0, 1, 0, 4)
#define SYS_REG_OSLSR_EL1		SYS_REG(2, 0, 1, 1, 4)
#define SYS_REG_OSDLR_EL1		SYS_REG(2, 0, 1, 3, 4)
#define SYS_REG_DBGPRCR_EL1		SYS_REG(2, 0, 1, 4, 4)
#define SYS_REG_DBGCLAIMSET_EL1		SYS_REG(2, 0, 7, 8, 6)
#define SYS_REG_DBGCLAIMCLR_EL1		SYS_REG(2, 0, 7, 9, 6)
#define SYS_REG_DBGAUTHSTATUS_EL1	SYS_REG(2, 0, 7, 14, 6)
#define SYS_REG_MDCCSR_EL0		SYS_REG(2, 3, 0, 1, 0)
#define SYS_REG_DBGDTR_EL0		SYS_REG(2, 3, 0, 4, 0)
#define SYS_REG_DBGDTRRX_EL0		SYS_REG(2, 3, 0, 5, 0)
#define SYS_REG_DBGDTRTX_EL0		SYS_REG(2, 3, 0, 5, 0)
#define SYS_REG_DBGVCR32_EL2		SYS_REG(2, 4, 0, 7, 0)

#define SYS_REG_MIDR_EL1		SYS_REG(3, 0, 0, 0, 0)
#define SYS_REG_MPIDR_EL1		SYS_REG(3, 0, 0, 0, 5)
#define SYS_REG_REVIDR_EL1		SYS_REG(3, 0, 0, 0, 6)
#define SYS_REG_ID_PFR0_EL1		SYS_REG(3, 0, 0, 1, 0)
#define SYS_REG_ID_PFR1_EL1		SYS_REG(3, 0, 0, 1, 1)
#define SYS_REG_ID_PFR2_EL1		SYS_REG(3, 0, 0, 3, 4)
#define SYS_REG_ID_DFR0_EL1  		SYS_REG(3, 0, 0, 1, 2)
#define SYS_REG_ID_DFR1_EL1		SYS_REG(3, 0, 0, 3, 5)
#define SYS_REG_ID_AFR0_EL1		SYS_REG(3, 0, 0, 1, 3)
#define SYS_REG_ID_MMFR0_EL1		SYS_REG(3, 0, 0, 1, 4)
#define SYS_REG_ID_MMFR1_EL1		SYS_REG(3, 0, 0, 1, 5)
#define SYS_REG_ID_MMFR2_EL1		SYS_REG(3, 0, 0, 1, 6)
#define SYS_REG_ID_MMFR3_EL1		SYS_REG(3, 0, 0, 1, 7)
#define SYS_REG_ID_MMFR4_EL1		SYS_REG(3, 0, 0, 2, 6)
#define SYS_REG_ID_MMFR5_EL1		SYS_REG(3, 0, 0, 3, 6)
#define SYS_REG_ID_ISAR0_EL1		SYS_REG(3, 0, 0, 2, 0)
#define SYS_REG_ID_ISAR1_EL1		SYS_REG(3, 0, 0, 2, 1)
#define SYS_REG_ID_ISAR2_EL1		SYS_REG(3, 0, 0, 2, 2)
#define SYS_REG_ID_ISAR3_EL1		SYS_REG(3, 0, 0, 2, 3)
#define SYS_REG_ID_ISAR4_EL1		SYS_REG(3, 0, 0, 2, 4)
#define SYS_REG_ID_ISAR5_EL1		SYS_REG(3, 0, 0, 2, 5)
#define SYS_REG_ID_ISAR6_EL1		SYS_REG(3, 0, 0, 2, 7)
#define SYS_REG_MVFR0_EL1		SYS_REG(3, 0, 0, 3, 0)
#define SYS_REG_MVFR1_EL1		SYS_REG(3, 0, 0, 3, 1)
#define SYS_REG_MVFR2_EL1		SYS_REG(3, 0, 0, 3, 2)
#define SYS_REG_ID_AA64PFR0_EL1		SYS_REG(3, 0, 0, 4, 0)
#define SYS_REG_ID_AA64PFR1_EL1		SYS_REG(3, 0, 0, 4, 1)
#define SYS_REG_ID_AA64ZFR0_EL1		SYS_REG(3, 0, 0, 4, 4)
#define SYS_REG_ID_AA64DFR0_EL1		SYS_REG(3, 0, 0, 5, 0)
#define SYS_REG_ID_AA64DFR1_EL1		SYS_REG(3, 0, 0, 5, 1)
#define SYS_REG_ID_AA64AFR0_EL1		SYS_REG(3, 0, 0, 5, 4)
#define SYS_REG_ID_AA64AFR1_EL1		SYS_REG(3, 0, 0, 5, 5)
#define SYS_REG_ID_AA64ISAR0_EL1	SYS_REG(3, 0, 0, 6, 0)
#define SYS_REG_ID_AA64ISAR1_EL1	SYS_REG(3, 0, 0, 6, 1)
#define SYS_REG_ID_AA64MMFR0_EL1	SYS_REG(3, 0, 0, 7, 0)
#define SYS_REG_ID_AA64MMFR1_EL1	SYS_REG(3, 0, 0, 7, 1)
#define SYS_REG_ID_AA64MMFR2_EL1	SYS_REG(3, 0, 0, 7, 2)
#define SYS_REG_SCTLR_EL1		SYS_REG(3, 0, 1, 0, 0)
#define SYS_REG_ACTLR_EL1		SYS_REG(3, 0, 1, 0, 1)
#define SYS_REG_CPACR_EL1		SYS_REG(3, 0, 1, 0, 2)
#define SYS_REG_RGSR_EL1		SYS_REG(3, 0, 1, 0, 5)
#define SYS_REG_GCR_EL1			SYS_REG(3, 0, 1, 0, 6)
#define SYS_REG_ZCR_EL1			SYS_REG(3, 0, 1, 2, 0)
#define SYS_REG_TRFCR_EL1		SYS_REG(3, 0, 1, 2, 1)
#define SYS_REG_TTBR0_EL1		SYS_REG(3, 0, 2, 0, 0)
#define SYS_REG_TTBR1_EL1		SYS_REG(3, 0, 2, 0, 1)
#define SYS_REG_TCR_EL1			SYS_REG(3, 0, 2, 0, 2)
#define SYS_REG_APIAKEYLO_EL1		SYS_REG(3, 0, 2, 1, 0)
#define SYS_REG_APIAKEYHI_EL1		SYS_REG(3, 0, 2, 1, 1)
#define SYS_REG_APIBKEYLO_EL1		SYS_REG(3, 0, 2, 1, 2)
#define SYS_REG_APIBKEYHI_EL1		SYS_REG(3, 0, 2, 1, 3)
#define SYS_REG_APDAKEYLO_EL1		SYS_REG(3, 0, 2, 2, 0)
#define SYS_REG_APDAKEYHI_EL1		SYS_REG(3, 0, 2, 2, 1)
#define SYS_REG_APDBKEYLO_EL1		SYS_REG(3, 0, 2, 2, 2)
#define SYS_REG_APDBKEYHI_EL1		SYS_REG(3, 0, 2, 2, 3)
#define SYS_REG_APGAKEYLO_EL1		SYS_REG(3, 0, 2, 3, 0)
#define SYS_REG_APGAKEYHI_EL1		SYS_REG(3, 0, 2, 3, 1)
#define SYS_REG_SPSR_EL1		SYS_REG(3, 0, 4, 0, 0)
#define SYS_REG_ELR_EL1			SYS_REG(3, 0, 4, 0, 1)
#define SYS_REG_ICC_PMR_EL1		SYS_REG(3, 0, 4, 6, 0)
#define SYS_REG_AFSR0_EL1		SYS_REG(3, 0, 5, 1, 0)
#define SYS_REG_AFSR1_EL1		SYS_REG(3, 0, 5, 1, 1)
#define SYS_REG_ESR_EL1			SYS_REG(3, 0, 5, 2, 0)
#define SYS_REG_ERRIDR_EL1		SYS_REG(3, 0, 5, 3, 0)
#define SYS_REG_ERRSELR_EL1		SYS_REG(3, 0, 5, 3, 1)
#define SYS_REG_ERXFR_EL1		SYS_REG(3, 0, 5, 4, 0)
#define SYS_REG_ERXCTLR_EL1		SYS_REG(3, 0, 5, 4, 1)
#define SYS_REG_ERXSTATUS_EL1		SYS_REG(3, 0, 5, 4, 2)
#define SYS_REG_ERXADDR_EL1		SYS_REG(3, 0, 5, 4, 3)
#define SYS_REG_ERXMISC0_EL1		SYS_REG(3, 0, 5, 5, 0)
#define SYS_REG_ERXMISC1_EL1		SYS_REG(3, 0, 5, 5, 1)
#define SYS_REG_TFSR_EL1		SYS_REG(3, 0, 5, 6, 0)
#define SYS_REG_TFSRE0_EL1		SYS_REG(3, 0, 5, 6, 1)
#define SYS_REG_FAR_EL1			SYS_REG(3, 0, 6, 0, 0)
#define SYS_REG_PAR_EL1			SYS_REG(3, 0, 7, 4, 0)
#define SYS_REG_PMSIDR_EL1		SYS_REG(3, 0, 9, 9, 7)
#define SYS_REG_PMBIDR_EL1		SYS_REG(3, 0, 9, 10, 7)
#define SYS_REG_PMSCR_EL1		SYS_REG(3, 0, 9, 9, 0)
#define SYS_REG_PMSCR_EL2		SYS_REG(3, 4, 9, 9, 0)
#define SYS_REG_PMSICR_EL1		SYS_REG(3, 0, 9, 9, 2)
#define SYS_REG_PMSIRR_EL1		SYS_REG(3, 0, 9, 9, 3)
#define SYS_REG_PMSNEVFR_EL1		SYS_REG(3, 0, 9, 9, 1)
#define SYS_REG_PMSFCR_EL1		SYS_REG(3, 0, 9, 9, 4)
#define SYS_REG_PMSEVFR_EL1		SYS_REG(3, 0, 9, 9, 5)
#define SYS_REG_PMSLATFR_EL1		SYS_REG(3, 0, 9, 9, 6)
#define SYS_REG_PMBLIMITR_EL1		SYS_REG(3, 0, 9, 10, 0)
#define SYS_REG_PMBPTR_EL1		SYS_REG(3, 0, 9, 10, 1)
#define SYS_REG_PMBSR_EL1		SYS_REG(3, 0, 9, 10, 3)
#define SYS_REG_TRBLIMITR_EL1		SYS_REG(3, 0, 9, 11, 0)
#define SYS_REG_TRBPTR_EL1		SYS_REG(3, 0, 9, 11, 1)
#define SYS_REG_TRBBASER_EL1		SYS_REG(3, 0, 9, 11, 2)
#define SYS_REG_TRBSR_EL1		SYS_REG(3, 0, 9, 11, 3)
#define SYS_REG_TRBMAR_EL1		SYS_REG(3, 0, 9, 11, 4)
#define SYS_REG_TRBTRG_EL1		SYS_REG(3, 0, 9, 11, 6)
#define SYS_REG_TRBIDR_EL1		SYS_REG(3, 0, 9, 11, 7)
#define SYS_REG_PMINTENSET_EL1		SYS_REG(3, 0, 9, 14, 1)
#define SYS_REG_PMINTENCLR_EL1		SYS_REG(3, 0, 9, 14, 2)
#define SYS_REG_PMMIR_EL1		SYS_REG(3, 0, 9, 14, 6)
#define SYS_REG_MAIR_EL1		SYS_REG(3, 0, 10, 2, 0)
#define SYS_REG_AMAIR_EL1		SYS_REG(3, 0, 10, 3, 0)
#define SYS_REG_LORSA_EL1		SYS_REG(3, 0, 10, 4, 0)
#define SYS_REG_LOREA_EL1		SYS_REG(3, 0, 10, 4, 1)
#define SYS_REG_LORN_EL1		SYS_REG(3, 0, 10, 4, 2)
#define SYS_REG_LORC_EL1		SYS_REG(3, 0, 10, 4, 3)
#define SYS_REG_LORID_EL1		SYS_REG(3, 0, 10, 4, 7)
#define SYS_REG_VBAR_EL1		SYS_REG(3, 0, 12, 0, 0)
#define SYS_REG_DISR_EL1		SYS_REG(3, 0, 12, 1, 1)
#define SYS_REG_ICC_IAR0_EL1		SYS_REG(3, 0, 12, 8, 0)
#define SYS_REG_ICC_EOIR0_EL1		SYS_REG(3, 0, 12, 8, 1)
#define SYS_REG_ICC_HPPIR0_EL1		SYS_REG(3, 0, 12, 8, 2)
#define SYS_REG_ICC_BPR0_EL1		SYS_REG(3, 0, 12, 8, 3)
#define SYS_REG_ICC_AP0R0_EL1		SYS_REG(3, 0, 12, 8, 4)
#define SYS_REG_ICC_AP0R1_EL1		SYS_REG(3, 0, 12, 8, 5)
#define SYS_REG_ICC_AP0R2_EL1		SYS_REG(3, 0, 12, 8, 6)
#define SYS_REG_ICC_AP0R3_EL1		SYS_REG(3, 0, 12, 8, 7)
#define SYS_REG_ICC_AP1R0_EL1		SYS_REG(3, 0, 12, 9, 0)
#define SYS_REG_ICC_AP1R1_EL1		SYS_REG(3, 0, 12, 9, 1)
#define SYS_REG_ICC_AP1R2_EL1		SYS_REG(3, 0, 12, 9, 2)
#define SYS_REG_ICC_AP1R3_EL1		SYS_REG(3, 0, 12, 9, 3)
#define SYS_REG_ICC_DIR_EL1		SYS_REG(3, 0, 12, 11, 1)
#define SYS_REG_ICC_RPR_EL1		SYS_REG(3, 0, 12, 11, 3)
#define SYS_REG_ICC_SGI1R_EL1		SYS_REG(3, 0, 12, 11, 5)
#define SYS_REG_ICC_ASGI1R_EL1		SYS_REG(3, 0, 12, 11, 6)
#define SYS_REG_ICC_SGI0R_EL1		SYS_REG(3, 0, 12, 11, 7)
#define SYS_REG_ICC_IAR1_EL1		SYS_REG(3, 0, 12, 12, 0)
#define SYS_REG_ICC_EOIR1_EL1		SYS_REG(3, 0, 12, 12, 1)
#define SYS_REG_ICC_HPPIR1_EL1		SYS_REG(3, 0, 12, 12, 2)
#define SYS_REG_ICC_BPR1_EL1		SYS_REG(3, 0, 12, 12, 3)
#define SYS_REG_ICC_CTLR_EL1		SYS_REG(3, 0, 12, 12, 4)
#define SYS_REG_ICC_SRE_EL1		SYS_REG(3, 0, 12, 12, 5)
#define SYS_REG_ICC_IGRPEN0_EL1		SYS_REG(3, 0, 12, 12, 6)
#define SYS_REG_ICC_IGRPEN1_EL1		SYS_REG(3, 0, 12, 12, 7)
#define SYS_REG_CONTEXTIDR_EL1		SYS_REG(3, 0, 13, 0, 1)
#define SYS_REG_TPIDR_EL1		SYS_REG(3, 0, 13, 0, 4)
#define SYS_REG_SCXTNUM_EL1		SYS_REG(3, 0, 13, 0, 7)
#define SYS_REG_CNTKCTL_EL1		SYS_REG(3, 0, 14, 1, 0)
#define SYS_REG_CCSIDR_EL1		SYS_REG(3, 1, 0, 0, 0)
#define SYS_REG_CLIDR_EL1		SYS_REG(3, 1, 0, 0, 1)
#define SYS_REG_GMID_EL1		SYS_REG(3, 1, 0, 0, 4)
#define SYS_REG_AIDR_EL1		SYS_REG(3, 1, 0, 0, 7)
#define SYS_REG_CSSELR_EL1		SYS_REG(3, 2, 0, 0, 0)

#define SYS_REG_CTR_EL0			SYS_REG(3, 3, 0, 0, 1)
#define SYS_REG_DCZID_EL0		SYS_REG(3, 3, 0, 0, 7)
#define SYS_REG_RNDR_EL0		SYS_REG(3, 3, 2, 4, 0)
#define SYS_REG_RNDRRS_EL0		SYS_REG(3, 3, 2, 4, 1)
#define SYS_REG_PMCR_EL0		SYS_REG(3, 3, 9, 12, 0)
#define SYS_REG_PMCNTENSET_EL0		SYS_REG(3, 3, 9, 12, 1)
#define SYS_REG_PMCNTENCLR_EL0		SYS_REG(3, 3, 9, 12, 2)
#define SYS_REG_PMOVSCLR_EL0		SYS_REG(3, 3, 9, 12, 3)
#define SYS_REG_PMSWINC_EL0		SYS_REG(3, 3, 9, 12, 4)
#define SYS_REG_PMSELR_EL0		SYS_REG(3, 3, 9, 12, 5)
#define SYS_REG_PMCEID0_EL0		SYS_REG(3, 3, 9, 12, 6)
#define SYS_REG_PMCEID1_EL0		SYS_REG(3, 3, 9, 12, 7)
#define SYS_REG_PMCCNTR_EL0		SYS_REG(3, 3, 9, 13, 0)
#define SYS_REG_PMXEVTYPER_EL0		SYS_REG(3, 3, 9, 13, 1)
#define SYS_REG_PMXEVCNTR_EL0		SYS_REG(3, 3, 9, 13, 2)
#define SYS_REG_PMUSERENR_EL0		SYS_REG(3, 3, 9, 14, 0)
#define SYS_REG_PMOVSSET_EL0		SYS_REG(3, 3, 9, 14, 3)
#define SYS_REG_TPIDR_EL0		SYS_REG(3, 3, 13, 0, 2)
#define SYS_REG_TPIDRRO_EL0		SYS_REG(3, 3, 13, 0, 3)
#define SYS_REG_SCXTNUM_EL0		SYS_REG(3, 3, 13, 0, 7)
#define SYS_REG_AMCR_EL0		SYS_REG(3, 3, 13, 2, 0)
#define SYS_REG_AMCFGR_EL0		SYS_REG(3, 3, 13, 2, 1)
#define SYS_REG_AMCGCR_EL0		SYS_REG(3, 3, 13, 2, 2)
#define SYS_REG_AMUSERENR_EL0		SYS_REG(3, 3, 13, 2, 3)
#define SYS_REG_AMCNTENCLR0_EL0		SYS_REG(3, 3, 13, 2, 4)
#define SYS_REG_AMCNTENSET0_EL0		SYS_REG(3, 3, 13, 2, 5)
#define SYS_REG_AMCNTENCLR1_EL0		SYS_REG(3, 3, 13, 3, 0)
#define SYS_REG_AMCNTENSET1_EL0		SYS_REG(3, 3, 13, 3, 1)
#define SYS_REG_AMEVCNTR0_EL0(n)	SYS_REG(3, 3, 13, 4 + ((n) >> 3), (n) & 7)
#define SYS_REG_AMEVTYPER0_EL0(n)	SYS_REG(3, 3, 13, 6 + ((n) >> 3), (n) & 7)
#define SYS_REG_AMEVCNTR1_EL0(n)	SYS_REG(3, 3, 13, 12 + ((n) >> 3), (n) & 7)
#define SYS_REG_AMEVTYPER1_EL0(n)	SYS_REG(3, 3, 13, 14 + ((n) >> 3), (n) & 7)
#define SYS_REG_AMEVCNTR0_CORE_EL0	SYS_REG_AMEVCNTR0_EL0(0)
#define SYS_REG_AMEVCNTR0_CONST_EL0	SYS_REG_AMEVCNTR0_EL0(1)
#define SYS_REG_AMEVCNTR0_INST_RET_EL0	SYS_REG_AMEVCNTR0_EL0(2)
#define SYS_REG_AMEVCNTR0_MEM_STALL	SYS_REG_AMEVCNTR0_EL0(3)
#define SYS_REG_CNTFRQ_EL0		SYS_REG(3, 3, 14, 0, 0)
#define SYS_REG_CNTP_TVAL_EL0		SYS_REG(3, 3, 14, 2, 0)
#define SYS_REG_CNTP_CTL_EL0		SYS_REG(3, 3, 14, 2, 1)
#define SYS_REG_CNTP_CVAL_EL0		SYS_REG(3, 3, 14, 2, 2)
#define SYS_REG_CNTV_CTL_EL0		SYS_REG(3, 3, 14, 3, 1)
#define SYS_REG_CNTV_CVAL_EL0		SYS_REG(3, 3, 14, 3, 2)
#define SYS_REG_AARCH32_CNTP_TVAL	SYS_REG(0, 0, 14, 2, 0)
#define SYS_REG_AARCH32_CNTP_CTL	SYS_REG(0, 0, 14, 2, 1)
#define SYS_REG_AARCH32_CNTP_CVAL	SYS_REG(0, 2, 0, 14, 0)
#define SYS_REG_PMEVCNTRn_EL0(n)	SYS_REG(3, 3, 14, 0x8 | (((n) >> 3) & 0x3), (n) & 0x7)
#define SYS_REG_PMEVTYPERn_EL0(n)	SYS_REG(3, 3, 14, 0xc | (((n) >> 3) & 0x3), (n) & 0x7)
#define SYS_REG_PMCCFILTR_EL0		SYS_REG(3, 3, 14, 15, 7)

#define SYS_REG_SCTLR_EL2		SYS_REG(3, 4, 1, 0, 0)
#define SYS_REG_HFGRTR_EL2		SYS_REG(3, 4, 1, 1, 4)
#define SYS_REG_HFGWTR_EL2		SYS_REG(3, 4, 1, 1, 5)
#define SYS_REG_HFGITR_EL2		SYS_REG(3, 4, 1, 1, 6)
#define SYS_REG_ZCR_EL2			SYS_REG(3, 4, 1, 2, 0)
#define SYS_REG_TRFCR_EL2		SYS_REG(3, 4, 1, 2, 1)
#define SYS_REG_DACR32_EL2		SYS_REG(3, 4, 3, 0, 0)
#define SYS_REG_HDFGRTR_EL2		SYS_REG(3, 4, 3, 1, 4)
#define SYS_REG_HDFGWTR_EL2		SYS_REG(3, 4, 3, 1, 5)
#define SYS_REG_HAFGRTR_EL2		SYS_REG(3, 4, 3, 1, 6)
#define SYS_REG_SPSR_EL2		SYS_REG(3, 4, 4, 0, 0)
#define SYS_REG_ELR_EL2			SYS_REG(3, 4, 4, 0, 1)
#define SYS_REG_IFSR32_EL2		SYS_REG(3, 4, 5, 0, 1)
#define SYS_REG_ESR_EL2			SYS_REG(3, 4, 5, 2, 0)
#define SYS_REG_VSESR_EL2		SYS_REG(3, 4, 5, 2, 3)
#define SYS_REG_FPEXC32_EL2		SYS_REG(3, 4, 5, 3, 0)
#define SYS_REG_TFSR_EL2		SYS_REG(3, 4, 5, 6, 0)
#define SYS_REG_FAR_EL2			SYS_REG(3, 4, 6, 0, 0)
#define SYS_REG_VDISR_EL2		SYS_REG(3, 4, 12, 1,  1)
#define SYS_REG_ICH_AP0R0_EL2		SYS_REG(3, 4, 12, 8, 0)
#define SYS_REG_ICH_AP0R1_EL2		SYS_REG(3, 4, 12, 8, 1)
#define SYS_REG_ICH_AP0R2_EL2		SYS_REG(3, 4, 12, 8, 2)
#define SYS_REG_ICH_AP0R3_EL2		SYS_REG(3, 4, 12, 8, 3)
#define SYS_REG_ICH_AP1R0_EL2		SYS_REG(3, 4, 12, 9, 0)
#define SYS_REG_ICH_AP1R1_EL2		SYS_REG(3, 4, 12, 9, 1)
#define SYS_REG_ICH_AP1R2_EL2		SYS_REG(3, 4, 12, 9, 2)
#define SYS_REG_ICH_AP1R3_EL2		SYS_REG(3, 4, 12, 9, 3)
#define SYS_REG_ICH_VSEIR_EL2		SYS_REG(3, 4, 12, 9, 4)
#define SYS_REG_ICC_SRE_EL2		SYS_REG(3, 4, 12, 9, 5)
#define SYS_REG_ICH_HCR_EL2		SYS_REG(3, 4, 12, 11, 0)
#define SYS_REG_ICH_VTR_EL2		SYS_REG(3, 4, 12, 11, 1)
#define SYS_REG_ICH_MISR_EL2		SYS_REG(3, 4, 12, 11, 2)
#define SYS_REG_ICH_EISR_EL2		SYS_REG(3, 4, 12, 11, 3)
#define SYS_REG_ICH_ELRSR_EL2		SYS_REG(3, 4, 12, 11, 5)
#define SYS_REG_ICH_VMCR_EL2		SYS_REG(3, 4, 12, 11, 7)
#define SYS_REG_ICH_LR0_EL2		SYS_REG(3, 4, 12, 12, 0)
#define SYS_REG_ICH_LR1_EL2		SYS_REG(3, 4, 12, 12, 1)
#define SYS_REG_ICH_LR2_EL2		SYS_REG(3, 4, 12, 12, 2)
#define SYS_REG_ICH_LR3_EL2		SYS_REG(3, 4, 12, 12, 3)
#define SYS_REG_ICH_LR4_EL2		SYS_REG(3, 4, 12, 12, 4)
#define SYS_REG_ICH_LR5_EL2		SYS_REG(3, 4, 12, 12, 5)
#define SYS_REG_ICH_LR6_EL2		SYS_REG(3, 4, 12, 12, 6)
#define SYS_REG_ICH_LR7_EL2		SYS_REG(3, 4, 12, 12, 7)
#define SYS_REG_ICH_LR8_EL2		SYS_REG(3, 4, 12, 13, 0)
#define SYS_REG_ICH_LR9_EL2		SYS_REG(3, 4, 12, 13, 1)
#define SYS_REG_ICH_LR10_EL2		SYS_REG(3, 4, 12, 13, 2)
#define SYS_REG_ICH_LR11_EL2		SYS_REG(3, 4, 12, 13, 3)
#define SYS_REG_ICH_LR12_EL2		SYS_REG(3, 4, 12, 13, 4)
#define SYS_REG_ICH_LR13_EL2		SYS_REG(3, 4, 12, 13, 5)
#define SYS_REG_ICH_LR14_EL2		SYS_REG(3, 4, 12, 13, 6)
#define SYS_REG_ICH_LR15_EL2		SYS_REG(3, 4, 12, 13, 7)

/* VHE encodings for architectural EL0/1 system registers */
#define SYS_REG_SCTLR_EL12		SYS_REG(3, 5, 1, 0, 0)
#define SYS_REG_CPACR_EL12		SYS_REG(3, 5, 1, 0, 2)
#define SYS_REG_ZCR_EL12		SYS_REG(3, 5, 1, 2, 0)
#define SYS_REG_TTBR0_EL12		SYS_REG(3, 5, 2, 0, 0)
#define SYS_REG_TTBR1_EL12		SYS_REG(3, 5, 2, 0, 1)
#define SYS_REG_TCR_EL12		SYS_REG(3, 5, 2, 0, 2)
#define SYS_REG_SPSR_EL12		SYS_REG(3, 5, 4, 0, 0)
#define SYS_REG_ELR_EL12		SYS_REG(3, 5, 4, 0, 1)
#define SYS_REG_AFSR0_EL12		SYS_REG(3, 5, 5, 1, 0)
#define SYS_REG_AFSR1_EL12		SYS_REG(3, 5, 5, 1, 1)
#define SYS_REG_ESR_EL12		SYS_REG(3, 5, 5, 2, 0)
#define SYS_REG_TFSR_EL12		SYS_REG(3, 5, 5, 6, 0)
#define SYS_REG_FAR_EL12		SYS_REG(3, 5, 6, 0, 0)
#define SYS_REG_MAIR_EL12		SYS_REG(3, 5, 10, 2, 0)
#define SYS_REG_AMAIR_EL12		SYS_REG(3, 5, 10, 3, 0)
#define SYS_REG_VBAR_EL12		SYS_REG(3, 5, 12, 0, 0)
#define SYS_REG_CONTEXTIDR_EL12		SYS_REG(3, 5, 13, 0, 1)
#define SYS_REG_CNTKCTL_EL12		SYS_REG(3, 5, 14, 1, 0)
#define SYS_REG_CNTP_TVAL_EL02		SYS_REG(3, 5, 14, 2, 0)
#define SYS_REG_CNTP_CTL_EL02		SYS_REG(3, 5, 14, 2, 1)
#define SYS_REG_CNTP_CVAL_EL02		SYS_REG(3, 5, 14, 2, 2)
#define SYS_REG_CNTV_TVAL_EL02		SYS_REG(3, 5, 14, 3, 0)
#define SYS_REG_CNTV_CTL_EL02		SYS_REG(3, 5, 14, 3, 1)
#define SYS_REG_CNTV_CVAL_EL02		SYS_REG(3, 5, 14, 3, 2)

#endif /* __SANDBOX_ARM64_SYSREG_H */

