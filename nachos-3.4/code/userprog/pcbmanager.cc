#include "pcbmanager.h"
#include "synch.h"
//KH Addition: to use the global lock declared in system.h, must
//include here I believe
#include "system.h"

PCBManager::PCBManager(int maxProcesses)
{

    bitmap = new BitMap(maxProcesses);
    pcbs = new PCB *[maxProcesses];
    //initializing the lock 
    //KH Addition (Edit): if this setup doesn't work,
    //we may consider implementing in system.h and system.cc
    //with the memory manager lock. 
    pcbManagerLock = new Lock("PCBManagerLock");
    for (int i = 0; i < maxProcesses; i++)
    {
        pcbs[i] = NULL;
    }
}

PCBManager::~PCBManager()
{

    delete bitmap;
    //KH Addition: Discussed with Jose :)
    delete pcbManagerLock;
    delete pcbs;
}

PCB *PCBManager::AllocatePCB()
{

    //KH Addition: if this setup doesn't work,
    // we may consider an implementation similar
    // to the memory manager lock.
    //Aquire pcbManagerLock
   
    pcbManagerLock->Acquire();

    int pid = bitmap->Find();

    // Release pcbManagerLock
    pcbManagerLock->Release();

    ASSERT(pid != -1);

    pcbs[pid] = new PCB(pid);

    return pcbs[pid];
}

int PCBManager::DeallocatePCB(PCB *pcb)
{

    // Check is pcb is valid -- check pcbs for pcb->pid

    // Aquire pcbManagerLock
    pcbManagerLock->Acquire();
    //KH Addition: I believe this would be where we should 
    //have the memory manager deallocate the pages. 
    //Wait, maybe this will work if I put it in the address space deallocator
    //pageTable = pcb->thread->space->GetPageTable();


    bitmap->Clear(pcb->pid);

    // Release pcbManagerLock
    pcbManagerLock->Release();

    int pid = pcb->pid;

    delete pcbs[pid];

    pcbs[pid] = NULL;

    return 0;
}

PCB *PCBManager::GetPCB(int pid)
{
    return pcbs[pid];
}