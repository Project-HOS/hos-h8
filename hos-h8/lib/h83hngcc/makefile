SRCDIR = ../../src

OBJS =	system.o task.o task_que.o interrup.o timer.o semaphor.o \
	eventflg.o mailbox.o fixmem.o task_dsp.o int_asm.o

LIB	= libhosv3h8hn.a

CC	= h8300-hms-gcc
ASM	= h8300-hms-gcc
OBJCOPY = h8300-hms-objcopy
AR	= h8300-hms-ar
RANLIB	= h8300-hms-ranlib

CFLAGS	= -Os -D__NORMAL_MODE__ -mh -mn -mrelax -I../../include -Wall -pipe
AFLAGS	= -mh -D__NORMAL_MODE__ -c
OCFLAGS	= -O srec
ARFLAGS	= rc
RLFLAGS	=

.SUFFIXES: .S .c .mot .coff

.c.o:
	${CC}  $< ${CFLAGS} -c

.S.o:
	${ASM} $< ${AFLAGS} -o $@

.mot.coff:
	${OBJCOPY} ${OCFLAGS} $< $@

all: ${LIB}

${LIB}: ${OBJS}
	${AR} ${ARFLAGS} ${LIB} ${OBJS}
	${RANLIB} ${LIB}

system.o:	${SRCDIR}/system.c
task.o:		${SRCDIR}/task.c
task_que.o:	${SRCDIR}/task_que.c
interrup.o:	${SRCDIR}/interrup.c
timer.o:	${SRCDIR}/timer.c
semaphor.o:	${SRCDIR}/semaphor.c
eventflg.o:	${SRCDIR}/eventflg.c
mailbox.o:	${SRCDIR}/mailbox.c
fixmem.o:	${SRCDIR}/fixmem.c
task_dsp.o:	${SRCDIR}/task_dsp.S
int_asm.o:	${SRCDIR}/int_asm.S

clean:
	rm -f ${OBJS} ${LIB} *~
