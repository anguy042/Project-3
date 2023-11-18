//KH Addition: Creating this file for testing. 
#include "syscall.h"

void myFunction(){
    int i = 0;
    while (i < 10){
        i = i + 1;
    }
}

int
main()
{
    Fork(myFunction);





}