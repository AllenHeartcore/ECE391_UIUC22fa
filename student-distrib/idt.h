#ifndef IDT_H
#define IDT_H

#define KERNEL_DPL 0
#define USER_DPL   3
#define TYPE_INTR  0
#define TYPE_TRAP  1

extern void idt_init(void);

#endif
