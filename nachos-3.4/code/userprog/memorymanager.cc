

#include "memorymanager.h"
#include "machine.h"


MemoryManager::MemoryManager() {

    bitmap = new BitMap(NumPhysPages);

}


MemoryManager::~MemoryManager() {

    delete bitmap;

}


int MemoryManager::AllocatePage() {

    return bitmap->Find();

}

int MemoryManager::DeallocatePage(int which) {
    //KH addition: We could have a case in which a PID or
    //page number is -1 because a function did not work.
    //For bitmap->test() and bitmap->clear(), this results
    //in an ASSERT failing. Therefore, we should first check
    //if the page number is -1.
    if(which < 0){
        return -1;
    }

    if(bitmap->Test(which) == false) return -1;
    else {
        bitmap->Clear(which);
        return 0;
    }

}


unsigned int MemoryManager::GetFreePageCount() {

    return bitmap->NumClear();

}


