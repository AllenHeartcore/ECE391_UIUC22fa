// You may add up to 5 elements to this struct.
// The type of synchronization primitive you may use is SpinLock.
typedef struct zs_enter_exit_lock{

}zs_lock;


int zombie_enter(zs_lock* zs) {

}

int zombie_exit(zs_lock* zs) {

}

int scientist_enter(zs_lock* zs) {

}

int scientist_exit(zs_lock* zs) {

}

/* 
 * sanitize_lab
 *   DESCRIPTION: Sanitizes the lab, removing all zombie contaminants. 
 *   INPUTS: zs -- pointer to the zombie-scientist lock structure 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Must only be called when the lab is empty.
 */
void sanitize_lab(zs_lock* zs);

