; --------------------------------------------------------------------------- 
;  HOS-H8 スタートアップルーチン                                              
;                                            Copyright (C) 1998-2000 by Ryuz  
; --------------------------------------------------------------------------- 

		.h8300h

; -------------------------------------
;  スタートアップルーチン
; -------------------------------------
		.section	.text
		.global		_hos_start
_hos_start:	
	; ----- スタック初期化
		mov.l	#_stack,er7
		
	; ----- D セクションコピー （DATAは64kbyte以下と仮定）
		mov.l	#___dtors_end,er5
		mov.l	#___data,er6
		mov.w	#data_size,r4
CPY_DSEC:	eepmov.w
		mov.w	r4,r4
		bne	CPY_DSEC
		
	; ----- B セクションクリア（BSSも64kbytes以下と仮定）
		mov.l	#_end,er5
		mov.w	#bss_size,r6
		; beq	PASS_BSS_CLR
CLR_BSEC:	mov.b	r4l,@-er5
		dec.w	#1,r6
		bne	CLR_BSEC
PASS_BSS_CLR:
	; ----- 初期化ルーチン呼び出し
		jsr	@___initialize
		
		mov.w	#0x04,r0
		mov.w	r0,@_sysstat	; スタートアップはタスク独立部
		mov.w	#0x01,r0
		mov.w	r0,@_int_cnt	; スタートアップは割り込み中とみなす
		ldc.b	#0x00,ccr	; 割り込み許可
		
		jsr	@_start_up	; スタートアップルーチン呼び出し
		
		ldc.b	#0xff,ccr	; 割り込み禁止
		xor.w	r0,r0
		mov.w	r0,@_sysstat	; 以後タスク部
		mov.w	r0,@_int_cnt	; 割り込みカウンタクリア
		
		jmp	@___tsk_dsp;	; タスクディスパッチ


		.end
