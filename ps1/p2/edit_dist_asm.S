/*
 * tab:2
 *
 * edit_dist_asm.S - inplementation of computing edit distance between two (linked) lists
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE,
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:        Xiang Li
 * Version:       1
 * Creation Date: Sat Aug 20 2022
 * History:
 *    XL    1    Sat Aug 20 2022
 *        First written.
 */

.data
  UNUSED  = 0
  VAL   = 4
  NEXT = 8

.text
.global edit_dist_asm

# Edit Dist ASM (Assembly)
# Declaration is in edit_dist.h
#
# Registers:
#   EAX - Return Value
#   EBX - root1
#   BSI - root2
#   you may choose how to store your local/temp variables
#

edit_dist_asm:
  pushl %ebp
  movl %esp, %ebp

  # --- STUDENT CODE STARTS HERE ---

  pushl %ebx            # callee-save regs
  pushl %ecx
  pushl %edx
  pushl %esi
  pushl %edi
  movl  8(%ebp), %ebx   # link to input interface
  movl  12(%ebp), %esi
  xorl  %eax, %eax      # clear eax

  cmpl  $0, %ebx        # nested 4-way brancher
  je    .r1nul
  jne   .r1nnul
.r1nul:
  cmpl  $0, %esi
  je    .both           # (L37)
  jne   .r1only         # (L39)
.r1nnul:
  cmpl  $0, %esi
  je    .r2only         # (L41)
  jne   .neither        # (L44 onwards)

.both:
  movl  $0, %eax
  jmp   .return         # (L38)
.r1only:
  pushl NEXT(%esi)      # push args
  pushl %ebx
  call  edit_dist_asm   # recursive call
  add   $8, %esp        # discard args
  jmp   .returninc      # (L40)
.r2only:
  pushl %esi
  pushl NEXT(%ebx)
  call  edit_dist_asm
  add   $8, %esp
  jmp   .returninc      # (L42)
.neither:
  movl  VAL(%ebx), %edi
  cmpl  VAL(%esi), %edi # (L44)
  jne   .unequal
  pushl NEXT(%esi)
  pushl NEXT(%ebx)
  call  edit_dist_asm
  add   $8, %esp
  jmp   .return         # (L46)

.unequal:
  pushl %esi
  pushl NEXT(%ebx)
  call  edit_dist_asm
  add   $8, %esp
  movl  %eax, %edi      # (L49) EDI = insert
  pushl NEXT(%esi)
  pushl %ebx
  call  edit_dist_asm
  add   $8, %esp
  movl  %eax, %edx      # (L50) EDX = delete
  pushl NEXT(%esi)
  pushl NEXT(%ebx)
  call  edit_dist_asm
  add   $8, %esp
  movl  %eax, %ecx      # (L51) ECX = replace

  cmpl  %edx, %edi
  ja    .ncondl53       # notice the order
  cmpl  %ecx, %edi
  ja    .ncondl53       # (L53)
  movl  %edi, %eax
  jmp   .returninc      # (L54)
.ncondl53:
  cmpl  %ecx, %edx
  ja    .ncondl55       # (L55)
  movl  %edx, %eax
  jmp   .returninc      # (L56)
.ncondl55:
  movl  %ecx, %eax      # (L57)

.returninc:
  incl  %eax
.return:
  popl  %edi            # restore callee-saved regs
  popl  %esi
  popl  %edx
  popl  %ecx
  popl  %ebx

  # --- STUDENT CODE ENDS HERE ---

  leave
  ret

