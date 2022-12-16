/*
 * tab:2
 *
 * edit_dist.c - inplementation of computing edit distance between two (linked) lists
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

#include "edit_dist.h"

uint32_t edit_dist_c(node_t* root1, node_t* root2){
    uint32_t insert, delete, replace;  

    if (root1 == NULL && root2 == NULL)  
        return 0;
    if (root1 == NULL)
        return 1 + edit_dist_c(root1, root2->next);  
    if (root2 == NULL)
        return 1 + edit_dist_c(root1->next, root2);

    if (root1->val == root2->val)
    {
        return edit_dist_c(root1->next, root2->next);
    }

    insert = edit_dist_c(root1->next, root2);
    delete = edit_dist_c(root1, root2->next);
    replace = edit_dist_c(root1->next, root2->next);

    if (insert <= delete && insert <= replace)
        return 1 + insert;
    if (delete <= replace)
        return 1 + delete;
    return 1 + replace;    
}
