#include <stdint.h>
#include "ece391support.h"
#include "ece391syscall.h"



uint32_t ten_power(int32_t power){
    if(power == 0)
        return 1;
    uint32_t ret = 1;
    while(power>0){
        ret = ret*10;
        power -=1;
    }
    return ret;
}

/* Arguments   name + size */
int main(){
    int32_t fd, cnt;
    uint8_t buf[1024];
    uint8_t name[21];

    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
        return 3;
    }
    cnt = ece391_strlen(buf);
    int32_t i;
    uint32_t size;
    size = 0;
    for(i=0; i<cnt; i++){
        if(buf[i] == ' ') // space find, next argument is size
            break;
        name[i] = buf[i];
    }
    i = i + 1;
    for( ;i < cnt; i++){
        size = ten_power(cnt-i-1)*((uint32_t)(buf[i]) - 48) + size;
    }
    ece391_slab_create(&name, size);
    return 0;

}