; --------------------------------------------------------------------------- 
;  HOS タスクディスパッチ処理                                                 
;                                     Copyright (C) 1998-2002 by Project HOS  
; --------------------------------------------------------------------------- 

		.h8300h
; -----------------------------------------------
;  割込みマスク (汎用)
; -----------------------------------------------
		.text
		.align 2
		.global	___di		
___di:		
		orc	#0x80,ccr
		rts

; -----------------------------------------------
;  割込みマスク解除 (汎用)
; -----------------------------------------------
		.text
		.align 2
		.global	___ei
___ei:		
		andc	#0x7F,ccr
		rts



; -----------------------------------------------
;  割込みマスク(システムコール用)
; -----------------------------------------------
		.text
		.align 2
		.global		___set_imsk
___set_imsk:	
		orc	#0x80,ccr
		rts

; -----------------------------------------------
;  割込みマスクからの復帰 (システムコール用)
; -----------------------------------------------
		.text
		.align 2
		.global		___res_imsk
___res_imsk:	
		mov.w	@_sysstat,r0
		and.w	#0x02,r0
		bne	NO_SET
		andc	#0x7f,ccr
NO_SET:		
		rts


; -----------------------------------------------
;           タスクスタート部分
;     最初のディスパッチ後ここに来る
; -----------------------------------------------
		.text
		.global		___tskst_entry
___tskst_entry:
		ldc	#0x00,ccr	; 割り込み許可
		push.w	r0		; スタートパラメーター
		jsr	@er6
		adds	#2,er7
		jmp	@_ext_tsk	; タスク終了


; -----------------------------------------------
;  タスク切り替え
;  H   __swc_dsp(T_TCB *tcb);
; -----------------------------------------------
		.text
		.align 2
		.global		___swc_dsp
___swc_dsp:	
		push.l	er3
		push.l	er4
		push.l	er5
		push.l	er6
		subs	#2, er7		; リターンパラメータ用領域作成
		mov.l	@_curtcb,er1	; GCCではpointer sizeを16bitに出来ない 
		mov.l	er7,@(0, er1)	; スタックポインタ保存
		mov.l	@(0, er0), er7	; スタックポインタ復帰
		mov.l	er0,@_curtcb	; 新TCB登録
		pop.w	r0		; リターンパラメーター取得
		pop.l	er6
		pop.l	er5
		pop.l	er4
		pop.l	er3

		rts


; -----------------------------------------------
; タスクの復帰
; void __sta_dsp(T_TCB *tcb);
; -----------------------------------------------
		.text
		.align 2
		.global		___sta_dsp
___sta_dsp:	
		mov.l	@(0, er0),er7	; スタックポインタ復帰
		pop.w	r0
		pop.l	er6
		pop.l	er5
		pop.l	er4
		pop.l	er3

		rts


; -----------------------------------------------
; タスクの停止
; H    __end_dsp(void);
; -----------------------------------------------
		.text
		.align 2
		.global		___end_dsp
___end_dsp:	
		push.l	er3
		push.l	er4
		push.l	er5
		push.l	er6
		subs	#2, er7		; リターンパラメータ用領域作成
		mov.l	@_curtcb,er1
		mov.l	er7,@(0, er1)	; スタックポインタ保存
		jmp	@___tsk_hlt	; タスク停止


; -----------------------------------------------
;        HALT (レディーキューにタスクが無い)
; -----------------------------------------------
		.text
		.align 2
		.global		___tsk_hlt
___tsk_hlt:	
		mov.l	#0xffffffff,er0		; NADR
		mov.l	er0,@_curtcb
#ifdef __NORMAL_MODE__
		mov.w	#_halt_sp,r7
#else
		mov.l	#_halt_sp,er7
#endif
		ldc	#0x00,ccr		; 割り込み許可
HLT_LOOP:	bra	HLT_LOOP		; 無限ループ


		.bss
		.global		_halt_stack
		.align	2
_halt_stack:	.space	2*64			; HALT時のスタック
_halt_sp:

		.end
