; --------------------------------------------------------------------------- 
;  HOS-H8 �������ȥ��åץ롼����                                              
;                                            Copyright (C) 1998-2000 by Ryuz  
; --------------------------------------------------------------------------- 

		.h8300h

; -------------------------------------
;  �������ȥ��åץ롼����
; -------------------------------------
		.section	.text
		.global		_hos_start
_hos_start:	
	; ----- �����å������
		mov.l	#_stack,er7
		
	; ----- D ��������󥳥ԡ� ��DATA��64kbyte�ʲ��Ȳ����
		mov.l	#___dtors_end,er5
		mov.l	#___data,er6
		mov.w	#data_size,r4
CPY_DSEC:	eepmov.w
		mov.w	r4,r4
		bne	CPY_DSEC
		
	; ----- B ��������󥯥ꥢ��BSS��64kbytes�ʲ��Ȳ����
		mov.l	#_end,er5
		mov.w	#bss_size,r6
		; beq	PASS_BSS_CLR
CLR_BSEC:	mov.b	r4l,@-er5
		dec.w	#1,r6
		bne	CLR_BSEC
PASS_BSS_CLR:
	; ----- ������롼����ƤӽФ�
		jsr	@___initialize
		
		mov.w	#0x04,r0
		mov.w	r0,@_sysstat	; �������ȥ��åפϥ�������Ω��
		mov.w	#0x01,r0
		mov.w	r0,@_int_cnt	; �������ȥ��åפϳ�������Ȥߤʤ�
		ldc.b	#0x00,ccr	; �����ߵ���
		
		jsr	@_start_up	; �������ȥ��åץ롼����ƤӽФ�
		
		ldc.b	#0xff,ccr	; �����߶ػ�
		xor.w	r0,r0
		mov.w	r0,@_sysstat	; �ʸ奿������
		mov.w	r0,@_int_cnt	; �����ߥ����󥿥��ꥢ
		
		jmp	@___tsk_dsp;	; �������ǥ����ѥå�


		.end
