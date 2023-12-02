// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "system.h"
//KH Addition
#include "pcb.h"
#include "openfile.h"
class pcb;
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void doExit(int status)
{
    //KH Addition: Just going to add this so I can see both
    //forked processes exiting hopefully:
    int pid = currentThread->space->pcb->pid;

    printf("System Call: [%d] invoked [Exit]\n", pid);
    printf("Process [%d] exits with [%d]\n", pid, status);

    currentThread->space->pcb->exitStatus = status;

    // Manage PCB memory As a parent process
    PCB *pcb = currentThread->space->pcb;

    // Delete exited children and set parent null for non-exited ones
    pcb->DeleteExitedChildrenSetParentNull();

    // Manage PCB memory As a child process
    if (pcb->parent == NULL){
        //printf("No parent. Deallocating current PCB.\n");
         pcbManager->DeallocatePCB(pcb);
    } else{
        //KH Addition: I see a potential problem here with the prof's
        //pseudocode. In the original pseudocode, it looks
        //like we are only deallocating the pcb if the parent is null.
        //If the parent is not null, we would want to fix the children list
        //and then still deallocate the PCB, no? I will put that in
        //here in an else statement.
        //For testing:
        //printf("Parent not null. Removing process from parent's children list.\n");
        
        PCB* parentPCB = pcb->parent;
        parentPCB->RemoveChild(pcb);

        //Now still deallocate:
        //printf("Deallocating current PCB.\n");
        pcbManager->DeallocatePCB(pcb);

    }

    // Delete address space only after use is completed

    //KH Note: This line should cause the pages to be freed because it causes
    //the addrspace deconstructor to be called. Therefore, even if PCB's are still
    //allocated to certain PID's because their children have not exited or something
    //like that, the pages should be free, allowing the system to create more processes.
    delete currentThread->space;

    // Finish current thread only after all the cleanup is done
    // because currentThread marks itself to be destroyed (by a different thread)
    // and then puts itself to sleep -- thus anything after this statement will not be executed!
    currentThread->Finish();
}

void incrementPC()
{
    int oldPCReg = machine->ReadRegister(PCReg);

    machine->WriteRegister(PrevPCReg, oldPCReg);
    machine->WriteRegister(PCReg, oldPCReg + 4);
    machine->WriteRegister(NextPCReg, oldPCReg + 8);
}

void childFunction(int pid)
{   
    //For testing:
    //printf("\nInside childFunction! PID: [%d]\n", pid);

   
    // 1. Restore the state of registers
    // currentThread->RestoreUserState()
    currentThread->RestoreUserState();

    // 2. Restore the page table for child
    // currentThread->space->RestoreState()
    currentThread->space->RestoreState();

    // PCReg == machine->ReadRegister(PCReg)
    // print message for child creation (pid,  PCReg, currentThread->space->GetNumPages())
    
    // n/a - no child creation print message given in
    // project instructions

    //for testing:
    //printf("About to call machine->Run()!\n");
    machine->Run();
    // machine->Run();
}

int doFork(int functionAddr)
{
    
    // 1. Check if sufficient memory exists to create new process
    // currentThread->space->GetNumPages() <= mm->GetFreePageCount()
    // if check fails, return -1
    int currPID = currentThread->space->pcb->pid;
    int needed = currentThread->space->GetNumPages();
    
    printf("System Call: [%d] invoked [Fork]\n", currPID);
   
    int avail = mm->GetFreePageCount();
    if(needed > avail){
        //For testing:
        //printf("Not enough memory.\n");
        //printf("Need %d pages.\n", needed);
        //printf("Machine only has %d \n", avail);
        printf("Process [%d] Fork: Not Enough memory available for child process.\n", currPID);
        return -1;
    }
    printf("Process [%d] Fork: start at address [0x%x] with [%d] pages memory\n", currPID, functionAddr, needed);
    // 2. SaveUserState for the parent thread
    // currentThread->SaveUserState();
    currentThread->SaveUserState();

    // 3. Create a new address space for child by copying parent address space
    // Parent: currentThread->space
    // childAddrSpace: new AddrSpace(currentThread->space)
    AddrSpace *childAddrSpace;
    childAddrSpace = new AddrSpace(currentThread->space);

    // 4. Create a new thread for the child and set its addrSpace
    // childThread = new Thread("childThread")
    // child->space = childAddSpace;
    Thread *childThread;
    childThread = new Thread("Forked Child Thread");
    childThread->space = childAddrSpace;

    // 5. Create a PCB for the child and connect it all up
    // pcb: pcbManager->AllocatePCB();
    // pcb->thread = childThread
    // set parent for child pcb
    // add child for parent pcb
    PCB* childPCB = pcbManager->AllocatePCB();
    childPCB->thread = childThread;
    childPCB->parent = currentThread->space->pcb;

    currentThread->space->pcb->AddChild(childPCB);

    //Give the child address space the new PCB:
    childAddrSpace->pcb = childPCB;


    // 6. Set up machine registers for child and save it to child thread
    // PCReg: functionAddr
    // PrevPCReg: functionAddr-4
    // NextPCReg: functionAddr+4
    // childThread->SaveUserState();

    
    machine->WriteRegister(PrevPCReg, functionAddr - 4);//see if changing to 0 helps
    machine->WriteRegister(PCReg, functionAddr);
    machine->WriteRegister(NextPCReg, functionAddr + 4);
    childThread->SaveUserState();
   

    // 7. Restore register state of parent user-level process
    // currentThread->RestoreUserState()
    currentThread->RestoreUserState();

    // 8. Call thread->fork on Child
    // childThread->Fork(childFunction, pcb->pid)
    int childPID = childPCB->pid;
    childThread->Fork(childFunction, childPID);
    

    //Since we don't know if the child thread ran or not,
    //I feel like we should restore the state again here.
    //Now I do not believe this is necessary, however my code
    //Is working and I do not want to change anything haha
    currentThread->RestoreUserState();
    currentThread->space->RestoreState();
    
    // 9. return pcb->pid;
    
    //printf("Returning child PID: %d \n", childPID);
    return childPID;
}

int doExec(char *filename)
{
    printf("System Call: [%d] invoked [Exec]\n", currentThread->space->pcb->pid);
	printf("Exec Program: [%d] loading [%s]\n", currentThread->space->pcb->pid, filename);

    // Use progtest.cc:StartProcess() as a guide

    // 1. Open the file and check validity
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        return -1;
    }

    // 2. Delete current address space but store current PCB first if using in Step 5.
    PCB *pcb = currentThread->space->pcb;
    delete currentThread->space;

    // 3. Create new address space
    space = new AddrSpace(executable);
    //4.     // close file
    delete executable;			

    // 5. Check if Addrspace creation was successful
    if(space->valid != true) {
        printf("Could not create AddrSpace\n");
        return -1;
    }

    // Manage PCB memory As a parent process
    PCB *spcb = space->pcb;

    // Delete exited children and set parent null for non-exited ones
    spcb->DeleteExitedChildrenSetParentNull();

    // Manage PCB memory As a child process
    if (spcb->parent == NULL)
        pcbManager->DeallocatePCB(spcb);

    // 6. Set the PCB for the new addrspace - reused from deleted address space
    space->pcb = pcb;

    // 7. Set the addrspace for currentThread
    currentThread->space = space;

    // 8. Initialize registers for new addrspace
    space->InitRegisters();		// set the initial register values

    // 9. Initialize the page table
    space->RestoreState();		// load page table register

    // 10. Run the machine now that all is set up
    machine->Run();			// jump to the user progam
    ASSERT(FALSE); // Execution nevere reaches here

    return 0;
}


int doJoin(int pid)
{
    int currentPID = currentThread->space->pcb->pid;
    // 1. Check if this is a valid pid and return -1 if not

    //KH addition for testing:
    printf("Process [%d] invoked Join on process [%d]\n", currentPID, pid);

    PCB* joinPCB = pcbManager->GetPCB(pid);
    
    printf("System Call: [%d] invoked [Join]\n", currentThread->space->pcb->pid);

    if (pid <= 0)
    {
        return -1;
    }

    if(joinPCB == NULL)
    {
        return -1;
    }

    // 2. Check if pid is a child of current process
    PCB *pcb = currentThread->space->pcb;
    if (pcb != joinPCB->parent)
    {

        return -1;
    }

    // 3. Yield until joinPCB has not exited
    while (!joinPCB->HasExited())
    {
        currentThread->Yield();
    }

    // 4. Store status and delete joinPCB
    int status = joinPCB->exitStatus;
    delete joinPCB;

    // 5. return status;
    return status;
}

int doKill(int pid)
{
    printf("System Call: [%d] invoked [Kill]\n", currentThread->space->pcb->pid);

    // 1. Check if the pid is valid and if not, return -1
    PCB *targetPCB = pcbManager->GetPCB(pid);
    if (targetPCB == nullptr)
    {
        ////printing the line for when its unsuccessful
        printf("Process [%d] cannot kill process [%d]: doesn't exist\n", currentThread->space->pcb->pid, pid);
        return -1;
    }

    // 2. IF pid is self, then just exit the process
    if (targetPCB->thread == currentThread)
    {
        doExit(0);
        return 0;
    }

    // 3. Valid kill, pid exists and not self, do cleanup similar to Exit
    // However, change references from currentThread to the target thread
    // pcb->thread is the target thread


    // 4. Set thread to be destroyed.
    scheduler->RemoveThread(targetPCB->thread);
    currentThread->Yield();

    //printing the line for when process is killed
    printf("Process [%d] killed process [%d]\n", currentThread->space->pcb->pid, pid);

    // 5. return 0 for success!
    return 0;
}

void doYield()
{
    //KH Addition: Adding printout statement for testing. We should
    //be able to keep this actually if you haven't already added it
    //to yours, Annie
    int PID = currentThread->space->pcb->pid;
    printf("System Call: [%d] invoked Yield.\n", PID);
    currentThread->Yield();
}

// This implementation (discussed in one of the videos) is broken!
// Try and figure out why.
char *readString1(int virtAddr)
{

    unsigned int pageNumber = virtAddr / 128;
    unsigned int pageOffset = virtAddr % 128;
    unsigned int frameNumber = machine->pageTable[pageNumber].physicalPage;
    unsigned int physicalAddr = frameNumber * 128 + pageOffset;

    char *string = &(machine->mainMemory[physicalAddr]);

    return string;
}

// This implementation is correct!
// perform MMU translation to access physical memory
char *readString(int virtualAddr)
{
    int i = 0;
    char *str = new char[256];
    unsigned int physicalAddr = currentThread->space->Translate(virtualAddr);

    // Need to get one byte at a time since the string may straddle multiple pages that are not guaranteed to be contiguous in the physicalAddr space
    bcopy(&(machine->mainMemory[physicalAddr]), &str[i], 1);
    while (str[i] != '\0' && i != 256 - 1)
    {
        virtualAddr++;
        i++;
        physicalAddr = currentThread->space->Translate(virtualAddr);
        bcopy(&(machine->mainMemory[physicalAddr]), &str[i], 1);
    }
    if (i == 256 - 1 && str[i] != '\0')
    {
        str[i] = '\0';
    }

    return str;
}

void doClose(char *fileName)
{
    printf("Syscall Call: [%d] invoked Close.\n", currentThread->space->pcb->pid);
    fileSystem->Create(fileName, 0);
}

void doCreate(char *fileName)
{
    printf("Syscall Call: [%d] invoked Create.\n", currentThread->space->pcb->pid);
    fileSystem->Create(fileName, 0);
}

OpenFileId doOpen(char *fileName)
{
    printf("Syscall Call: [%d] invoked Open.\n", currentThread->space->pcb->pid);

    OpenFile* openFile = fileSystem->Open(fileName);
    
    // SysOpenFile* file = currentThread->space->pcb->FindOpenFile(fileName)
    //if(file){
    //     file->userOpens++;
    // }
    // else{
    //     new SysOpenFile(openFileId, 0, fileName);
    // }
    return 0;
}

void doWrite(char *buffer, int size, OpenFileId id){
    printf("Syscall Call: [%d] invoked Write.\n", currentThread->space->pcb->pid);
    printf(buffer);
}

int doRead(char *buffer, int size, OpenFileId id){
    printf("Syscall Call: [%d] invoked Read.\n", currentThread->space->pcb->pid);
    buffer[0] = getchar();
    return 0;
}


void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if ((which == SyscallException) && (type == SC_Halt))
    {
        DEBUG('a', "Shutdown, initiated by user program.\n");
        interrupt->Halt();
    }
    else if ((which == SyscallException) && (type == SC_Exit))
    {
        // Implement Exit system call
        doExit(machine->ReadRegister(4));
    }
    else if ((which == SyscallException) && (type == SC_Fork))
    {
      
        int ret = doFork(machine->ReadRegister(4));
        //for testing:
        //printf("Returned from doFork()! Inside ExceptionHandler. \n");
        //printf("About to return childpid %d\n", ret);
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Exec))
    {
        int virtAddr = machine->ReadRegister(4);
        char *fileName = readString(virtAddr);
        int ret = doExec(fileName);
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Join))
    {
        int ret = doJoin(machine->ReadRegister(4));
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Kill))
    {
        int ret = doKill(machine->ReadRegister(4));
        machine->WriteRegister(2, ret);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Yield))
    {
        doYield();
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Create))
    {
        int virtAddr = machine->ReadRegister(4);
        char *fileName = readString(virtAddr);
        doCreate(fileName);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Open))
    {
        int virtAddr = machine->ReadRegister(4);
        char *fileName = readString(virtAddr);
        doOpen(fileName);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Close))
    {
        int virtAddr = machine->ReadRegister(4);
        char *fileName = readString(virtAddr);
        doClose(fileName);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Read))
    {
        int virtAddr4 = machine->ReadRegister(4);
        int virtAddr5 = machine->ReadRegister(5);
        int virtAddr6 = machine->ReadRegister(6);
        char *buffer = readString(virtAddr4);
        char *bufferSize = readString(virtAddr5);
        int size;
        sscanf(bufferSize, "%d", &size); 
        char *bufferId = readString(virtAddr6);
        int id;
        sscanf(bufferId, "%d", &id); 
        doRead(buffer, size, id);
        incrementPC();
    }
    else if ((which == SyscallException) && (type == SC_Write))
    {
       int virtAddr4 = machine->ReadRegister(4);
        int virtAddr5 = machine->ReadRegister(5);
        int virtAddr6 = machine->ReadRegister(6);
        char *buffer = readString(virtAddr4);
        char *bufferSize = readString(virtAddr5);
        int size;
        sscanf(bufferSize, "%d", &size); 
        char *bufferId = readString(virtAddr6);
        int id;
        sscanf(bufferId, "%d", &id); 
        doWrite(buffer, size, id);
        incrementPC();
    }
    else
    {
        printf("Unexpected user mode exception %d %d\n", which, type);
        ASSERT(FALSE);
    }
}
