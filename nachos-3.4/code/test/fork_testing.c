//KH Addition: Creating this file for testing. 
#include "syscall.h"

int sum = 0;

void myFunction(){
    sum = sum + 1;
    Exit(sum);
}

int
main()
{
    Fork(myFunction);
    Fork(myFunction);
    Exit(sum);





}