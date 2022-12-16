
/*
 * tab:2
 *
 * mystery_asm.S - Assembly mystery
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
 * Author:        Yan Miao
 * Version:       1
 * Creation Date: Sun Aug 29 2021
 * Author:        Xiang Li
 * Version:       2
 * Modified Date: Sun Aug 21 2022
 * History:
 *    YM    1    Sun Aug 29 2021
 *    XL    2    Sun Aug 21 2022
 */

.data

.text
.global mystery_asm

# mystery (Assembly)

mystery_asm:
  pushl %ebp
  movl %esp, %ebp
  PUSHL %EBX
  PUSHL %ESI
  PUSHL %EDI

  MOVL 12(%EBP), %EBX
  CMPL $24, %EBX
  JA DONE1
  MOVL 8(%EBP), %EBX
  CMPL $42, %EBX
  JAE DONE1
  MOVL $0, %EDI
  MOVL $1, %ESI

LOOP1:
  CMPL %ESI, %EBX
  JL DONE2
  ADDL %ESI, %EDI
  INCL %ESI
  JMP LOOP1

DONE2:
  MOVL 12(%EBP), %EBX 
  MOVL $1, %EDX       
  MOVL $1, %ESI       

LOOP2:
  CMPL %ESI, %EBX
  JL DONE3
  IMULL %ESI, %EDX
  INCL %ESI
  JMP LOOP2

DONE3:
  ORL %EDI, %EDX 
  MOVL %EDX, %EAX
  JMP DONE
  
DONE1:
  MOVL $0, %EAX

DONE:
  POPL %EDI
  POPL %ESI
  POPL %EBX
  leave
  ret
