OBJS   = cfg_asm.obj cfg_c.obj sample.obj h8_sci.obj ..\src\hos_c.obj ..\src\hos_asm.obj
CC     = cc38h
ASM    = a38h
LNK    = l38h
CNV    = c38h
CFLAGS = -INCLUDE=\akih8c -INCLUDE=..\include
AFLAGS = 

.SUFFIXES:
.SUFFIXES: .src .c

.c.obj:
	$(CC)  $< $(CFLAGS)

.src.obj:
	$(ASM) $< $(AFLAGS)


sample.mot: sample.abs
	$(CNV) sample.abs sample.mot

sample.abs: $(OBJS) LNK.SUB
	$(LNK) -SUBCOMMAND=lnk.sub

cfg_c.c cfg_asm.src defid.h: hos.cfg sample.h h8_sci.h
	HCFGH8h

h8_sci.obj: h8_sci.h

sample.obj: sample.h


clean:
	del *.obj
