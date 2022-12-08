#ifndef IDT_LNK_H
#define IDT_LNK_H

#include "keyboard.h"
#include "rtc.h"
#include "syscall.h"

extern void int_kbd_lnk(void);
extern void int_rtc_lnk(void);
extern void syscall_lnk(void);
extern void pit_handler_linkage(void);
extern void exc_pft_linkage(void);
extern void exc_dv0_linkage(void);
extern void exc_dbg_linkage(void);
extern void exc_nmi_linkage(void);
extern void exc_bpt_linkage(void);
extern void exc_ovf_linkage(void);
extern void exc_bre_linkage(void);
extern void exc_ivo_linkage(void);
extern void exc_dna_linkage(void);
extern void exc_dbf_linkage(void);
extern void exc_cso_linkage(void);
extern void exc_its_linkage(void);
extern void exc_snp_linkage(void);
extern void exc_ssf_linkage(void);
extern void exc_gpf_linkage(void);
extern void exc_res_linkage(void);
extern void exc_fpe_linkage(void);
extern void exc_alc_linkage(void);
extern void exc_mac_linkage(void);
extern void exc_sfe_linkage(void);
#endif
