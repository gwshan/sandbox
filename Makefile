#
# We need to create the binary by dynamic linking some day.
# For now, it's fine reply on gcc to resolve everyting for
# us.
#

CC	:= gcc
CFLAGS	:= -Wall -O2 -g -Wstrict-prototypes -Wuninitialized	\
	   -std=gnu99 -fno-stack-protector -I inc -pthread
SOURCES := lib/bitops.c		\
	   lib/rbtree.c		\
	   lib/sparsebit.c	\
	   sched/elf.c		\
	   mm/mm.c		\
	   kvm/mm.c		\
	   kvm/vcpu.c		\
	   kvm/kvm.c		\
	   main.c

default:
	$(CC) $(CFLAGS) $(SOURCES) -o sandbox
