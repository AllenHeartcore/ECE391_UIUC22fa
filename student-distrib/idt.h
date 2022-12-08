#ifndef IDT_H
#define IDT_H

#define KERNEL_DPL 0
#define USER_DPL   3
#define TYPE_INTR  0
#define TYPE_TRAP  1

extern void idt_init(void);
extern void exc_pft(void);
extern void exc_dbg(void);
extern void exc_nmi(void);
extern void exc_bpt(void);
extern void exc_ovf(void);
extern void exc_bre(void);
extern void exc_ivo(void);
extern void exc_dna(void);
extern void exc_dbf(void);
extern void exc_cso(void);
extern void exc_its(void);
extern void exc_snp(void);
extern void exc_ssf(void);
extern void exc_gpf(void);
extern void exc_res(void);
extern void exc_fpe(void);
extern void exc_alc(void);
extern void exc_mac(void);
extern void exc_sfe(void);


#endif
