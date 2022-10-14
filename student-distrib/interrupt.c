#include <stdio.h>

void exc_div0() { printf("EXCEPTION 0x00: Divide by zero"); }
void exc_dbg()  { printf("EXCEPTION 0x01: Debug"); }
void exc_nmi()  { printf("EXCEPTION 0x02: Non-maskable interrupt"); }
void exc_bp()   { printf("EXCEPTION 0x03: Breakpoint"); }
void exc_ovf()  { printf("EXCEPTION 0x04: Overflow"); }
void exc_bre()  { printf("EXCEPTION 0x05: Bound range exceeded"); }
void exc_ivo()  { printf("EXCEPTION 0x06: Invalid opcode"); }
void exc_dna()  { printf("EXCEPTION 0x07: Device not available"); }
void exc_dbf()  { printf("EXCEPTION 0x08: Double fault"); }
void exc_cso()  { printf("EXCEPTION 0x09: Coprocessor segment overrun"); }
void exc_itss() { printf("EXCEPTION 0x0A: Invalid TSS"); }
void exc_snp()  { printf("EXCEPTION 0x0B: Segment not present"); }
void exc_ssf()  { printf("EXCEPTION 0x0C: Stack-segment fault"); }
void exc_gpf()  { printf("EXCEPTION 0x0D: General protection fault"); }
void exc_pf()   { printf("EXCEPTION 0x0E: Page fault"); }
void exc_r1()   { printf("EXCEPTION 0x0F: Reserved"); }
void exc_fpe()  { printf("EXCEPTION 0x10: Floating-point error"); }
void exc_alc()  { printf("EXCEPTION 0x11: Alignment check"); }
void exc_mac()  { printf("EXCEPTION 0x12: Machine check"); }
void exc_sfpe() { printf("EXCEPTION 0x13: SIMD floating-point exception"); }
void exc_vte()  { printf("EXCEPTION 0x14: Virtualization exception"); }
void exc_cpe()  { printf("EXCEPTION 0x15: Control protection exception"); }
void exc_r3()   { printf("EXCEPTION 0x16: Reserved"); }
void exc_r4()   { printf("EXCEPTION 0x17: Reserved"); }
void exc_r5()   { printf("EXCEPTION 0x18: Reserved"); }
void exc_r6()   { printf("EXCEPTION 0x19: Reserved"); }
void exc_r7()   { printf("EXCEPTION 0x1A: Reserved"); }
void exc_r8()   { printf("EXCEPTION 0x1B: Reserved"); }
void exc_hie()  { printf("EXCEPTION 0x1C: Hypervisor injection exception"); }
void exc_vce()  { printf("EXCEPTION 0x1D: VMM communication exception"); }
void exc_se()   { printf("EXCEPTION 0x1E: Security exception"); }
void exc_r9()   { printf("EXCEPTION 0x1F: Reserved"); }
