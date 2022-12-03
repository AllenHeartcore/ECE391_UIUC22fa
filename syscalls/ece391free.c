#include <stdint.h>
#include "ece391support.h"
#include "ece391syscall.h"

uint32_t sixteen_power(int32_t power){
    if(power == 0)
        return 1;
    uint32_t ret = 1;
    while(power>0){
        ret = ret*16;
        power -=1;
    }
    return ret;
}


int main(){
    int32_t fd, cnt;
    uint8_t buf[1024];
    if (0 != ece391_getargs (buf, 1024)) {
        ece391_fdputs (1, (uint8_t*)"could not read arguments\n");
        return 3;
    }
    cnt = ece391_strlen(buf);
    int32_t i;
    uint32_t ptr;
    ptr = 0;
    for(i=1; i<=cnt; i++){
        if(buf[i-1] > 57){
            ptr= sixteen_power(cnt-i)*((uint32_t)(buf[i-1]) - 55) + ptr;
        }else{
            ptr= sixteen_power(cnt-i)*((uint32_t)(buf[i-1]) - 48) + ptr;
        }
    }
    ece391_free((void*)ptr);
    return 0;
}