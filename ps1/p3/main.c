/*
 * tab:2
 *
 * main.c - I/O and P3 setup
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
 * Author:        Aamir Hasan
 * Version:       1
 * Creation Date: Sun Aug 30 2020
 * Author:        Yan Miao
 * Version:       2
 * Modified Date: Sun Aug 29 2021
 * Author:        Xiang Li
 * Version:       2
 * Modified Date: Sun Aug 21 2022
 * History:
 *    AH    1    Sun Aug 30 2020
 *    YM    2    Sun Aug 29 2021
 *    XL    2    Sun Aug 21 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mystery.h"

#define MAX_LENGTH 500

uint32_t import(char* infile, uint32_t* a, uint32_t* b) {
  FILE* f = fopen(infile, "r");
  if (f == NULL) return -1;
  fscanf(f, "%u %u\n", a, b);
  fclose(f);
  return 0;
}

int main(int argc, char** argv) {
  if(argc < 2) {
    printf("Usage: ./mystery <input file>\n");
    return -1;
  }
  
  uint32_t x, y, out;

  import(argv[1], &x, &y);
  printf("\nRunning C Code\n");
  out = mystery_c(x, y);
  printf("Mystery(%u, %u) = %u\n\n", x, y, out);

  printf("Running x86 Assembly Code\n");
  out = mystery_asm(x, y);
  printf("Mystery(%u, %u) = %u\n\n", x, y, out);

  return 0;
}
