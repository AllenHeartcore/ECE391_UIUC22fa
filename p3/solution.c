#include <linux/spinlock.h>
#define lock(fd) spin_lock_irqsave(fd->lock, flags)
#define unlock(fd) spin_unlock_irqrestore(fd->lock, flags)

typedef struct biscuit_juice_lock{
    spinlock_t* lock;
    int num_biscuit;
    int num_juice;
} fd_lock_t;
int flags;                          // global var: INT status store & restore

void produce_biscuit(fd_lock_t* fd) {
    lock(fd);                       // refer to the "#define"s above ^
    fd->num_biscuit++;
    unlock(fd);
}

void consume_biscuit(fd_lock_t* fd) {
    while (true) {                  // trapped in a "blocked" state
        lock(fd);
        if (fd->num_biscuit > 0) {  // proceed if non-empty
            fd->num_biscuit--;
            unlock(fd);
            return;                 // escape
        }
        unlock(fd);
    }
}

int produce_juice(fd_lock_t* fd) {
    lock(fd);
    if (fd->num_juice == 4) {       // "if" is not atomic, so if it...
        unlock(fd);                 // appears outside the lock, ...
        return -1;                  // this is a race condition
    }
    fd->num_juice++;
    unlock(fd);
    return 0;
}

void consume_juice(fd_lock_t* fd) {
    while (true) {
        lock(fd);
        if (fd->num_juice > 0) {    // "fd" is volatile!
            fd->num_juice--;
            unlock(fd);
            return;
        }
        unlock(fd);
    }
}

int main() {
    fd_lock_t* fd = (fd_lock_t*) malloc(sizeof(fd_lock_t));
    fd->lock = (spinlock_t*) malloc(sizeof(spinlock_t));
    spin_lock_init(fd->lock);       // a better idea is to...
    fd->num_biscuit = 0;            // wrap up these inits...
    fd->num_juice = 0;              // in a class
    // perform operations
    return 0;
}
