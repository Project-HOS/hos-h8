CSRCS  = system.c task.c task_que.c interrup.c timer.c semaphor.c \
         eventflg.c mailbox.c fixmem.c
ASRCS  = startup.src task_dsp.src int_asm.src
OBJS   = hos_c.obj hos_asm.obj

CC     = cc38h
ASM    = a38h
CFLAGS = -INCLUDE=e:\akih8c -INCLUDE=..\include
AFLAGS =

.SUFFIXES:
.SUFFIXES: .src .c

.c.obj:
	$(CC)  $< $(CFLAGS)

.src.obj:
	$(ASM) $< $(AFLAGS)

all: $(OBJS)

hos_c.obj: $(CSRCS)

hos_asm.obj: $(ASRCS)

