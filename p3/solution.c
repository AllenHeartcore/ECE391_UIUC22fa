#include <linux/spinlock.h>
#define lock(fd) spin_lock_irqsave(fd->lock, flags)
#define unlock(fd) spin_unlock_irqrestore(fd->lock, flags)

typedef struct biscuit_juice_lock{
    spinlock_t* lock;
    int num_biscuit;
    int num_juice;
} fd_lock_t;
int flags;

void produce_biscuit(fd_lock_t* fd) {
    lock(fd);
    fd->num_biscuit++;
    unlock(fd);
}

void consume_biscuit(fd_lock_t* fd) {
    while (true) {
        lock(fd);
        if (fd->num_biscuit > 0) {
            fd->num_biscuit--;
            unlock(fd);
            return;
        }
        unlock(fd);
    }
}

int produce_juice(fd_lock_t* fd) {
    lock(fd);
    if (fd->num_juice == 4) {
        unlock(fd);
        return -1;
    }
    fd->num_juice++;
    unlock(fd);
    return 0;
}

void consume_juice(fd_lock_t* fd) {
    while (true) {
        lock(fd);
        if (fd->num_juice > 0) {
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
    spin_lock_init(fd->lock);       // a better idea is to ...
    fd->num_biscuit = 0;            // wrap up these inits ...
    fd->num_juice = 0;              // in a class
    // perform operations
    return 0;
}
