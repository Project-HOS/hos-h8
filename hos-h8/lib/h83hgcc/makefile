# HOS-V3/H8hのカーネルソースディレクトリを指定
SRCDIR = ../../src

# HOS-V3/H8hのヘッダファイルを収めたディレクトリを指定
HOSINC = ../../include

OBJS =	system.o task.o task_que.o interrup.o timer.o semaphor.o \
	eventflg.o mailbox.o fixmem.o task_dsp.o int_asm.o

LIB	= libh8h.a

CC	= h8300-hms-gcc
ASM	= h8300-hms-gcc
OBJCOPY = h8300-hms-objcopy
AR	= h8300-hms-ar
RANLIB	= h8300-hms-ranlib

CFLAGS	= -Os -mh -mrelax -I$(HOSINC) -Wall -pipe
AFLAGS	= -mh -c
OCFLAGS	= -O srec
ARFLAGS	= rc
RLFLAGS	=

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

system.o:	$(SRCDIR)/system.c
	$(CC)  $< $(CFLAGS) -c

task.o:		$(SRCDIR)/task.c
	$(CC)  $< $(CFLAGS) -c

task_que.o:	$(SRCDIR)/task_que.c
	$(CC)  $< $(CFLAGS) -c

interrup.o:	$(SRCDIR)/interrup.c
	$(CC)  $< $(CFLAGS) -c

timer.o:	$(SRCDIR)/timer.c
	$(CC)  $< $(CFLAGS) -c

semaphor.o:	$(SRCDIR)/semaphor.c
	$(CC)  $< $(CFLAGS) -c

eventflg.o:	$(SRCDIR)/eventflg.c
	$(CC)  $< $(CFLAGS) -c

mailbox.o:	$(SRCDIR)/mailbox.c
	$(CC)  $< $(CFLAGS) -c

fixmem.o:	$(SRCDIR)/fixmem.c
	$(CC)  $< $(CFLAGS) -c

task_dsp.o:	$(SRCDIR)/task_dsp.S
	$(ASM) $< $(AFLAGS) -o $@

int_asm.o:	$(SRCDIR)/int_asm.S
	$(CC)  $< $(CFLAGS) -c

clean:
	rm -f $(OBJS) $(LIB) *~
