#include "lib.h"
#include "x86_desc.h"
#include "idt.h"
#include "idt_lnk.h"

void exc_dv0() { printf("EXCEPTION 0x00: Divide by zero\n"); }
void exc_dbg() { printf("EXCEPTION 0x01: Debug\n"); }
void exc_nmi() { printf("EXCEPTION 0x02: Non-maskable interrupt\n"); }
void exc_bpt() { printf("EXCEPTION 0x03: Breakpoint\n"); }
void exc_ovf() { printf("EXCEPTION 0x04: Overflow\n"); }
void exc_bre() { printf("EXCEPTION 0x05: Bound range exceeded\n"); }
void exc_ivo() { printf("EXCEPTION 0x06: Invalid opcode\n"); }
void exc_dna() { printf("EXCEPTION 0x07: Device not available\n"); }
void exc_dbf() { printf("EXCEPTION 0x08: Double fault\n"); }
void exc_cso() { printf("EXCEPTION 0x09: Coprocessor segment overrun\n"); }
void exc_its() { printf("EXCEPTION 0x0A: Invalid TSS\n"); }
void exc_snp() { printf("EXCEPTION 0x0B: Segment not present\n"); }
void exc_ssf() { printf("EXCEPTION 0x0C: Stack-segment fault\n"); }
void exc_gpf() { printf("EXCEPTION 0x0D: General protection fault\n"); }
void exc_pft() { printf("EXCEPTION 0x0E: Page fault\n"); }
void exc_fpe() { printf("EXCEPTION 0x10: Floating-point error\n"); }
void exc_alc() { printf("EXCEPTION 0x11: Alignment check\n"); }
void exc_mac() { printf("EXCEPTION 0x12: Machine check\n"); }
void exc_sfe() { printf("EXCEPTION 0x13: SIMD floating-point exception\n"); }

void idt_init() {
	lidt(idt_desc_ptr);
    SET_IDT_ENTRY(idt[0x00], exc_dv0, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x01], exc_dbg, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x02], exc_nmi, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x03], exc_bpt, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x04], exc_ovf, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x05], exc_bre, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x06], exc_ivo, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x07], exc_dna, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x08], exc_dbf, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x09], exc_cso, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0A], exc_its, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0B], exc_snp, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0C], exc_ssf, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0D], exc_gpf, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0E], exc_pft, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x10], exc_fpe, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x11], exc_alc, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x12], exc_mac, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x13], exc_sfe, KERNEL_DPL, TYPE_TRAP);
    // SET_IDT_ENTRY(idt[0x21], int_kbd_lnk, KERNEL_DPL, TYPE_INTR);
    // SET_IDT_ENTRY(idt[0x28], int_rtc_lnk, KERNEL_DPL, TYPE_INTR);
    // SET_IDT_ENTRY(idt[0x80], syscall_lnk, USER_DPL,   TYPE_INTR);
}
