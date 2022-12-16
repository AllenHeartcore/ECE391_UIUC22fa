/*
 * tab:2
 *
 * main.c - I/O and BST problem setup
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
 * Creation Date: Fri Aug 30 2020
 * Author:        Yan Miao
 * Version:       2
 * Modification Date: Sun Aug 29 2021
 * Author:        Xiang Li
 * Version:       3
 * Modification Date: Sat Aug 20 2022
 * History:
 *    AS    1    Fri Aug 30 2020
 *    YM    2    Sun Aug 29 2021
 *	  XL    3    Sat Aug 20 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "edit_dist.h"

node_t* import_linked_list(char* fname) {
  FILE* f = fopen(fname, "r");
  uint32_t i, val, length;

  fscanf(f, "%d\n", &length);
  
  node_t * head = NULL;
  node_t * cur = NULL;
  for (i = 0; i < length; i++) {
    fscanf(f, "%d\n", &val);
    if (i==0){
      head = create_node(val);
      cur = head;
    }else{
      cur->next = create_node(val);
	    cur = cur->next;
    }
  }

  fclose(f);
  return head;
}

int main(int argc, char** argv) {
  if(argc < 3) {
    printf("Usage: ./merge ./file1 ./file2\n");
    return -1;
  }
  node_t* root1 = NULL;
  node_t* root2 = NULL;
  uint32_t result = 0;
  
  root1 = import_linked_list(argv[1]);
  printf("\nReading Linked List 1:\n");
  print(root1);
  root2 = import_linked_list(argv[2]);
  printf("Reading Linked List 2:\n");
  print(root2);

  printf("\nRunnin Edit Dist in C\n");
  printf("edit_dist_c results: \n");
  result = edit_dist_c(root1, root2);
  printf("%d\n", result);

  root1 = import_linked_list(argv[1]);
  root2 = import_linked_list(argv[2]);

  printf("Runnin Edit Dist in x86\n");
  printf("edit_dist_asm results: \n");
  result = edit_dist_asm(root1, root2);
  printf("%d\n", result);  
  return 0;
}
