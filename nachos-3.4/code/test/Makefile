# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as

GCCDIR = ../../../gnu-decstation-ultrix/decstation-ultrix/2.95.3/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc -B../../../gnu-decstation-ultrix/
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -m32 -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)


#KH Addition: adding fork_testing and fork
all: create exit halt shell matmult sort fork_testing fork join kill exec memory


start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.c > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

create.o: create.c
	$(CC) $(CFLAGS) -c create.c
create: create.o start.o
	$(LD) $(LDFLAGS) start.o create.o -o create.coff
	../bin/coff2noff create.coff create

exit.o: exit.c
	$(CC) $(CFLAGS) -c exit.c
exit: exit.o start.o
	$(LD) $(LDFLAGS) start.o exit.o -o exit.coff
	../bin/coff2noff exit.coff exit

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

#KH Addition: Adding fork_testing by copying/pasting halt
# and changing halt to fork_testing.
fork_testing.o: fork_testing.c
	$(CC) $(CFLAGS) -c fork_testing.c
fork_testing: fork_testing.o start.o
	$(LD) $(LDFLAGS) start.o fork_testing.o -o fork_testing.coff
	../bin/coff2noff fork_testing.coff fork_testing

#KH Addition: Adding fork from Canvas for testing
fork.o: fork.c
	$(CC) $(CFLAGS) -c fork.c
fork: fork.o start.o
	$(LD) $(LDFLAGS) start.o fork.o -o fork.coff
	../bin/coff2noff fork.coff fork

 
join.o: join.c
	$(CC) $(CFLAGS) -c join.c
join: join.o start.o
	$(LD) $(LDFLAGS) start.o join.o -o join.coff
	../bin/coff2noff join.coff join

kill.o: kill.c
	$(CC) $(CFLAGS) -c kill.c
kill: kill.o start.o
	$(LD) $(LDFLAGS) start.o kill.o -o kill.coff
	../bin/coff2noff kill.coff kill

exec.o: exec.c
	$(CC) $(CFLAGS) -c exec.c
exec: exec.o start.o
	$(LD) $(LDFLAGS) start.o exec.o -o exec.coff
	../bin/coff2noff exec.coff exec 

memory.o: memory.c
	$(CC) $(CFLAGS) -c memory.c
memory: memory.o start.o
	$(LD) $(LDFLAGS) start.o memory.o -o memory.coff
	../bin/coff2noff memory.coff memory 