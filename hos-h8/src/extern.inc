; ---------------------------------------------------------------------------- 
;  HOS-H8h 外部定義シンボル宣言ファイル                                        
;                                              Copyright(C) 1998-2000 by Ryuz  
; ---------------------------------------------------------------------------- 


	; Ｃ言語ルーチンでの宣言
		.IMPORT	___initialize	; コンフィギュレーター生成の初期化
		.IMPORT	_start_up	; スタートアップルーチン
		.IMPORT	___tsk_dsp	; タスクの正規化
		.IMPORT	_ext_tsk	; タスクの終了

		.IMPORT	_sysstat	; システム状態
		.IMPORT	___system_stack	; タスク独立部利用のスタック
		.IMPORT	_int_cnt	; 割り込みネスト回数
		.IMPORT	_curtcb		; 現在実行中のTCB
		.IMPORT	_dlydsp		; 遅延ディスパッチ用フラグ
		.IMPORT	_rdyque		; レディーキュー
		.IMPORT	_rdqcnt		; レディーキューの数


	; int_asm.src での宣言

		.GLOBAL	_int_default	; デフォルトの割り込みハンドラ
		.GLOBAL	_int_trap	; 通常の割り込みハンドラ


	; satrtup.src での宣言
		.GLOBAL	_hos_start	; ＯＳの開始番地


	; tsk_dsp.src での宣言

		.GLOBAL	___di		; 割込み禁止(汎用)
		.GLOBAL	___ei		; 割込み許可(汎用)
		.GLOBAL	___set_imsk	; 割込み禁止(システムコール用)
		.GLOBAL	___res_imsk	; 割り込み復帰(システムコール用)
		.GLOBAL	___tskst_entry	; タスクの実行開始番地
		.GLOBAL	___swc_dsp	; タスクの切り替え
		.GLOBAL	___sta_dsp	; タスクの開始
		.GLOBAL	___end_dsp	; タスクの終了
		.GLOBAL	___tsk_hlt	; 実行タスク無し(HALT)
		.GLOBAL	_halt_stack	; HALT時に使うスタック
		.GLOBAL	_halt_sp	; HALT時のスタックポインタ

