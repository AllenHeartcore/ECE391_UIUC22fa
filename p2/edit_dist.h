/*
 * tab:2
 *
 * edit_score.h - helper function declarations
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
 * Version: 	  2
 * Modification Date: Sun Aug 29 2021
 * Author:        Xiang Li
 * Version: 	  3
 * Modification Date: Sat Aug 20 2022
 * History:
 *    AS    1    Fri Aug 30 2020
 *    YM    2    Sun Aug 29 2021
 *	  XL    3    Sat Aug 20 2022
 */

#ifndef EDITDIST_H
#define EDITDIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct node_t {
	uint32_t unused; // reserved for future use
	uint32_t val;
	struct node_t* next;
} node_t;

// create_node
// 		Creates a new node with the associated key and returns a pointed to the node
extern node_t* create_node(uint32_t key);

// print
// 		Prints the pre order traversal of the tree
extern void print(node_t* node);

// delete
// 		Deletes the tree. Frees all memory used by the tree
extern void delete(node_t* node);

// edit_dist_c
//   compute edit distance between two (linked) lists in C
extern uint32_t edit_dist_c(node_t* root1, node_t* root2);

// edit_dist_asm
//   compute edit distance between two (linked) lists in Assembly
extern uint32_t edit_dist_asm(node_t* root1, node_t* root2);

#endif
