/*
 * tab:2
 *
 * uitilities.c - Helper functions for Linked List
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
 * Filename:      utilities.c
 * Author:        Xiang Li
 * Version:       2
 * Modification Date: Sat Aug 20 2022
 * History:
 *    YM    1    Sun Aug 29 2021
 *	  XL    2    Sat Aug 20 2022
 */
#include "edit_dist.h"

node_t* create_node(uint32_t key) {
  // allocate memeory for node
  node_t* node = malloc(sizeof(node_t));

  // set param values
  node->val = key;
  node->unused = 0xECEB;
  node->next = NULL;

  return node;  
}

void print(node_t* node) {
  node_t * cur = node;
  while (cur != NULL){
    printf("%d ", cur->val);
    cur = cur->next;
  }
  printf("\n");
}

void delete(node_t* node) {
  node_t * cur = node;
  node_t * temp;
  while (cur != NULL){
    temp = cur;
    cur = cur->next;
    free(temp);
  }
}
