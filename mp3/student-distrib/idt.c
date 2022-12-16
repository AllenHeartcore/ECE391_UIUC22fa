#include "lib.h"
#include "x86_desc.h"
#include "idt.h"
#include "idt_lnk.h"
#include "signal.h"

/*  Since we are in the kernel level and idt[0x80] has USER_DPL,
 *  we cannot use the "movl $0, %eax; int $0x80" instruction.
 *  Instead, we call the unified "halt" syscall with a status
 *  code "1" to indicate an exception (contrary to a normal "0"). */

void exc_dv0() { printf("EXCEPTION 0x00: Divide by zero\n");                send_signal(SIG_DIV_ZERO);}
void exc_dbg() { printf("EXCEPTION 0x01: Debug\n");                         send_signal(SIG_SEGFAULT);}
void exc_nmi() { printf("EXCEPTION 0x02: Non-maskable interrupt\n");        send_signal(SIG_SEGFAULT);}
void exc_bpt() { printf("EXCEPTION 0x03: Breakpoint\n");                    send_signal(SIG_SEGFAULT);}
void exc_ovf() { printf("EXCEPTION 0x04: Overflow\n");                      send_signal(SIG_SEGFAULT);}
void exc_bre() { printf("EXCEPTION 0x05: Bound range exceeded\n");          send_signal(SIG_SEGFAULT);}
void exc_ivo() { printf("EXCEPTION 0x06: Invalid opcode\n");                send_signal(SIG_SEGFAULT);}
void exc_dna() { printf("EXCEPTION 0x07: Device not available\n");          send_signal(SIG_SEGFAULT);}
void exc_dbf() { printf("EXCEPTION 0x08: Double fault\n");                  send_signal(SIG_SEGFAULT);}
void exc_cso() { printf("EXCEPTION 0x09: Coprocessor segment overrun\n");   send_signal(SIG_SEGFAULT);}
void exc_its() { printf("EXCEPTION 0x0A: Invalid TSS\n");                   send_signal(SIG_SEGFAULT);}
void exc_snp() { printf("EXCEPTION 0x0B: Segment not present\n");           send_signal(SIG_SEGFAULT);}
void exc_ssf() { printf("EXCEPTION 0x0C: Stack-segment fault\n");           send_signal(SIG_SEGFAULT);}
void exc_gpf() { printf("EXCEPTION 0x0D: General protection fault\n");      send_signal(SIG_SEGFAULT);}
void exc_pft() { printf("EXCEPTION 0x0E: Page fault\n");                    send_signal(SIG_SEGFAULT);}
void exc_res() { printf("EXCEPTION 0x0F: Reserved\n");                      send_signal(SIG_SEGFAULT);}
void exc_fpe() { printf("EXCEPTION 0x10: Floating-point error\n");          send_signal(SIG_SEGFAULT);}
void exc_alc() { printf("EXCEPTION 0x11: Alignment check\n");               send_signal(SIG_SEGFAULT);}
void exc_mac() { printf("EXCEPTION 0x12: Machine check\n");                 send_signal(SIG_SEGFAULT);}
void exc_sfe() { printf("EXCEPTION 0x13: SIMD floating-point exception\n"); send_signal(SIG_SEGFAULT);}

void idt_init() {
    int i;
    for (i = 0; i < 256; i++) {
        idt[i].val[0] = 0;
        idt[i].val[1] = 0;
    }
    SET_IDT_ENTRY(idt[0x00], exc_dv0_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x01], exc_dbg_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x02], exc_nmi_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x03], exc_bpt_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x04], exc_ovf_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x05], exc_bre_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x06], exc_ivo_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x07], exc_dna_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x08], exc_dbf_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x09], exc_cso_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0A], exc_its_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0B], exc_snp_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0C], exc_ssf_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0D], exc_gpf_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0E], exc_pft_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x0F], exc_res_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x10], exc_fpe_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x11], exc_alc_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x12], exc_mac_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x13], exc_sfe_linkage, KERNEL_DPL, TYPE_TRAP);
    SET_IDT_ENTRY(idt[0x20], pit_handler_linkage, KERNEL_DPL, TYPE_INTR);
    SET_IDT_ENTRY(idt[0x21], int_kbd_lnk, KERNEL_DPL, TYPE_INTR);
    SET_IDT_ENTRY(idt[0x28], int_rtc_lnk, KERNEL_DPL, TYPE_INTR);
    SET_IDT_ENTRY(idt[0x80], syscall_lnk, USER_DPL,   TYPE_TRAP);
    lidt(idt_desc_ptr);
}
