; -------------------------------------------------------------------------- 
;  HOS 割り込み管理 アセンブリ部                                             
;                                            Copyright(C) 1998-2000 by Ryuz  
; -------------------------------------------------------------------------- 
		.h8300h

; -----------------------------------------------
;       デフォルトの割り込みハンドラ 
; -----------------------------------------------
		.text
		.align 2
		.global		_int_default
_int_default:
		rte


; -----------------------------------------------
;            割り込みのトラップ
;   er1 に呼び出しアドレスが入ってくる
; -----------------------------------------------
		.text
		.global		_int_trap
		.align 2
_int_trap:	
	; -------- レジスタ保存
		push.l	er0
		push.l	er2	; er1はint_trapを呼ぶintXXでpushされている
		push.l	er3
		
	; -------- 多重割り込みの判別
		mov.w	@_int_cnt,r0
		bne	MUL_INT


	; ======== 最初の割り込み ========
		mov.w	#1,r0
		mov.w	r0,@_int_cnt
		
	; -------- システム状態の変更
		mov.w	@_sysstat,r0
		bset	#2,r0l
		mov.w	r0,@_sysstat
		
	; -------- スタックポインタの入れ替え
#ifdef __NORMAL_MODE__	
		mov.w	r7,@_int_sp
		mov.w	#_stack,r7
#else
		mov.l	er7,@_int_sp
		mov.l	#_stack,er7
#endif		
	; -------- ハンドラ呼び出し
		jsr	@er1
		
	; -------- 割り込み復帰
		ldc	#0xff,ccr	; 割り込み禁止
		xor.w	r0,r0
		mov.w	r0,@_int_cnt
		
	; -------- スタックポインタ復帰
#ifdef __NORMAL_MODE__	
		mov.w	@_int_sp,r7
#else
		mov.l	@_int_sp,er7
#endif
		
	; -------- システム状態変更
		mov.w	@_sysstat,r0
		bclr	#2,r0l
		mov.w	r0,@_sysstat
		
	; -------- 遅延ディスパッチ
		mov.w	@_sysstat,r0
		bne	INT_RETURN
		mov.w	@_dlydsp,r0
		beq	INT_RETURN
		jsr	@___tsk_dsp
		
	; -------- 復帰
INT_RETURN:
		pop.l	er3
		pop.l	er2	; er1はint_trapを呼ぶintXXでpushされている
		pop.l	er0
		pop.l	er1
		rte


	; ======== 多重割り込み ========
MUL_INT:	
		inc.w	#1,r0
		mov.w	r0,@_int_cnt
		
	; -------- 割り込みハンドラ呼び出し
		jsr	@er1
		ldc	#0xff,ccr	; 割り込み禁止
		
	; -------- 割り込みからの復帰
		mov.w	@_int_cnt,r0
		dec.w	#1,r0
		mov.w	r0,@_int_cnt
	
		pop.l	er3
		pop.l	er2	; er1はint_trapを呼ぶintXXでpushされている
		pop.l	er0
		pop.l	er1

		rte


; -----------------------------------------------
;       割り込み処理データ
; -----------------------------------------------
		.bss
_int_sp:
#ifdef __NORMAL_MODE__	
		.space	2*1	; 割り込み時の r7 保存用
#else
		.space	4*1	; 割り込み時の er7 保存用
#endif
		.end
