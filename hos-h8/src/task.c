/* ------------------------------------------------------------------------- */
/*  HOS タスク管理                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#include "itron.h"


/* グローバル変数 */
T_TCB *curtcb;			/* 現在のタスクのTCB */
BOOL  dlydsp;			/* 遅延されているディスパッチがあるか */


/* タスク部の初期化 */
void __ini_tsk(void)
{
	int i, j;
	
	curtcb = NADR;
	dlydsp = FALSE;
	
	/* レディーキューの初期化 */
	for ( i = 0; i < rdqcnt; i++ )
		rdyque[i].tskcnt = 0;
	
	/* スタックプールの初期化 */
	for ( i = 0; i < spcbcnt; i++ ) {
		spcbtbl[i].que.tskcnt = 0;
		for ( j = 0; j < spcbstbl[i].stkcnt - 1; j++ )
			spcbstbl[i].stkhead[spcbstbl[i].stksz * j] = (VH)(j + 1);
		spcbstbl[i].stkhead[spcbstbl[i].stksz * j] = 0xffff;
		spcbtbl[i].freestk = 0;
	}
	
	/* TCBテーブルの初期化 */
	for ( i = 0; i < tcbcnt; i++ ) {
		if ( tcbstbl[i].itskpri != 0 )
			tcbtbl[i].tskstat = TTS_DMT;
		else 
			tcbtbl[i].tskstat = 0;	/* NON-EXISTENT */
		tcbtbl[i].que = NADR;
	}
}

/* スタック操作時のオフセット定数定義 単位はVH(WORD)*/
#ifndef __GNUC__ /* 日立版 */
 #ifndef __NORMAL_MODE__
  #define POINTER_SIZE_VH		2	/* アドレスサイズ */  
  #define CONTEXT_STACK_SIZE_VH	10	/* callee savedなレジスタ分 */
  #define OFFSET_PARAM_VH		1	/* レジスタ分を積んだ後のパラメータへのオフセット */
 #else
  #define POINTER_SIZE_VH		1	/* アドレスサイズ */  
	/* er6の下位WORDに16bitアドレスを入れる為の細工 */
  #define CONTEXT_STACK_SIZE_VH	9	/* callee savedなレジスタ分 */
  #define OFFSET_PARAM_VH		2	/* レジスタ分を積んだ後のパラメータへのオフセット */
 #endif
#else /* GNU版 */
 #ifndef __NORMAL_MODE__
  #define POINTER_SIZE_VH		2	/* アドレスサイズ */  
  #define CONTEXT_STACK_SIZE_VH	8	/* callee savedなレジスタ分 */
  #define OFFSET_PARAM_VH		1	/* レジスタ分を積んだ後のパラメータへのオフセット */
 #else
  #define POINTER_SIZE_VH		1	/* アドレスサイズ */  
	/* er6の下位WORDに16bitアドレスを入れる為の細工 */
  #define CONTEXT_STACK_SIZE_VH	7	/* callee savedなレジスタ分 */
  #define OFFSET_PARAM_VH		2	/* レジスタ分を積んだ後のパラメータへのオフセット */
 #endif
#endif

/* タスク開始 */
ER sta_tsk(ID tskid, INT stacd)
{
	T_TCB *tcb;
	const T_TCBS *tcbs;
	
#if __ERR_CHECK_LEVEL >= 4
	/* IDチェック */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb  = &tcbtbl[tskid - 1];
	tcbs = &tcbstbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* タスク状態チェック */
	if ( tcb->tskstat != TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* TCBを初期化 */
	tcb->sp      = tcbs->isp;
	tcb->tskpri  = tcbs->itskpri;
	tcb->tskstat = TTS_RDY;
	tcb->tskwait = 0;
	tcb->wupcnt  = 0;
	tcb->suscnt  = 0;
	
	/* スタックがスタックプールに設定されている場合 */
	if ( (UW)tcbs->isp & 0x00000001 ) {
		T_SPCB  *spcb;
		const T_SPCBS *spcbs;
		UW      spcbid;
		VH      *stack;
		
		spcbid = (UW)tcbs->isp >> 1;
		spcb   = &spcbtbl[spcbid];
		spcbs  = &spcbstbl[spcbid];
		
		/* スタックが空いていない場合は待ち */
		if ( spcb->freestk == 0xffff ) {
			__adt_que(&spcb->que, tcb);
			tcb->tskstat = TTS_WAI;
			tcb->tskwait = TTW_SPL;
			tcb->data    = (VP)(UW)stacd;
			
			__res_imsk();
			return E_OK;
		}
		
		
		/* スタックをプールから取得 */
		stack = &spcbs->stkhead[spcbs->stksz * spcb->freestk];
		spcb->freestk = *stack;
		tcb->sp = stack + spcbs->stksz;
	}
	
	/* スタックの初期設定 */
	tcb->sp -= POINTER_SIZE_VH;
#if POINTER_SIZE_VH == 2
	*(FP *)tcb->sp = (FP)__tskst_entry;	/* リターンアドレス */
#else
	*tcb->sp = (VH)(FP)__tskst_entry;	/* リターンアドレス */
#endif
	tcb->sp -= CONTEXT_STACK_SIZE_VH;
#if POINTER_SIZE_VH == 2
	*(FP *)tcb->sp = tcbs->task;		/* タスク開始アドレス */
#else
	*tcb->sp = (VH)tcbs->task;		/* タスク開始アドレス */
#endif
	tcb->sp -= OFFSET_PARAM_VH;
	*(tcb->sp) = (VH)stacd;			/* 初期化コード */
	
	/* READYキューに追加 */
	__adt_que(&rdyque[tcb->tskpri - 1], tcb);
	
	/* ディスパッチ */
	__tsk_dsp();
	
	__res_imsk();
	
	return E_OK;
}


/* スタックがスタックプールのものなら返却 */
void __rel_stp(T_TCB *tcb)
{
	const T_TCBS  *tcbs;
	T_SPCB  *spcb;
	const T_SPCBS *spcbs;
	UH spcbid;
	UH stack;
	
	tcbs = &tcbstbl[tcb - tcbtbl];
	
	/* スタックプールを利用しているかどうか？ */
	if ( !((UW)tcbs->isp & 0x00000001) )
		return;
	
	/* 利用しているスタックを算出 */
	spcbid = (UW)tcbs->isp >> 1;
	spcb   = &spcbtbl[spcbid];
	spcbs  = &spcbstbl[spcbid];
	stack  = ((VH *)tcb->sp - spcbs->stkhead - 1) / spcbs->stksz;
	
	/* 待ちタスクがあればスタックを譲渡 */
	if ( spcb->que.tskcnt > 0 ) {
		T_TCB  *tcbnext;
		const T_TCBS *tcbsnext;
		
		tcbnext  = spcb->que.head;
		tcbsnext = &tcbstbl[tcbnext - tcbtbl];
		__del_que(tcbnext);
		tcbnext->sp = &spcbs->stkhead[(stack + 1) * spcbs->stksz];
		
		/* スタックの初期設定 */
		tcbnext->sp -= POINTER_SIZE_VH;
#if POINTER_SIZE_VH == 2
		*(FP *)tcbnext->sp = (FP)__tskst_entry;	/* リターンアドレス */
#else
		*tcbnext->sp = (VH)(FP)__tskst_entry;	/* リターンアドレス */
#endif
		tcbnext->sp -= CONTEXT_STACK_SIZE_VH;
#if POINTER_SIZE_VH == 2
		*(FP *)tcbnext->sp = tcbsnext->task;	/* タスク開始アドレス */
#else
		*tcbnext->sp = (VH)tcbsnext->task;	/* タスク開始アドレス */
#endif

		tcbnext->sp -= OFFSET_PARAM_VH;
		*(tcbnext->sp) = (VH)(INT)tcbnext->data; /* 初期化コード */
		
		/* READYキューに追加 */
		__adt_que(&rdyque[tcb->tskpri - 1], tcbnext);
		
		return;
	}
	
	/* スタックプールに返却 */
	spcbs->stkhead[stack * spcbs->stksz] = spcb->freestk;
	spcb->freestk = stack;
}


/* タスク終了 */
void ext_tsk(void)
{
	
#if __ERR_CHECK_LEVEL >= 4
	/* エラーチェック */
	if ( sysstat != TSS_TSK ) {
		/* 致命的エラーだがリターン出来ない */
		sysstat = TSS_TSK;
	}
#endif
	
	__set_imsk();
	
	if ( curtcb != NADR ) {
		/* キューから外しDOMANT状態に移行 */
		if ( curtcb->que != NADR )
			__del_que(curtcb);
		curtcb->tskstat = TTS_DMT;
		
		/* スタックがスタックプールのものなら返却 */
		__rel_stp(curtcb);
		
		curtcb = NADR;
	}
	
	/* ディスパッチ */
	__tsk_dsp();
}


/* 他タスク強制終了 */
ER  ter_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* IDチェック */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* タスク状態チェック */
	if ( (!(sysstat & TSS_INDP) && tcb == curtcb)
						|| tcb->tskstat == TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* タスク終了 */
	if ( tcb->que != NADR )
		__del_que(tcb);
	tcb->tskstat = TTS_DMT;
	
	/* スタックがスタックプールのものなら返却 */
	__rel_stp(tcb);
	
	if ( curtcb == tcb )
		curtcb = NADR;	/* 非タスク部から現在実行中タスクを終了した時 */
	
	/* ディスパッチ */
	__tsk_dsp();
	
	__res_imsk();
	
	return E_OK;
}


/* ディスパッチ禁止 */
ER  dis_dsp(void)
{
#if __ERR_CHECK_LEVEL >= 3
	/* エラーチェック */
	if ( sysstat & (TSS_INDP | TSS_DINT) )
		return E_CTX;
#endif
	__set_imsk();
	
	sysstat = TSS_DDSP;
	
	__res_imsk();
	
	return E_OK;
}


/* ディスパッチ許可 */
ER  ena_dsp(void)
{
#if __ERR_CHECK_LEVEL >= 3
	/* エラーチェック */
	if ( sysstat & (TSS_INDP | TSS_DINT) )
		return E_CTX;
#endif
	
	__set_imsk();
	
	/* 遅延ディスパッチ */
	sysstat = TSS_TSK;
	if ( dlydsp )
		__tsk_dsp();
	
	__res_imsk();
	
	return E_OK;
}


/* タスク優先度変更 */
ER  chg_pri(ID tskid, PRI tskpri)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* パラメーターチェック */
	if ( tskid > tcbcnt )
		return E_NOEXS;
	if ( tskid == TSK_SELF && (sysstat & TSS_INDP) )
		return E_ID;
	if ( tskpri > rdqcnt )
		return E_PAR;
#endif
	
	/* ID -> TCB */
	if ( tskid == TSK_SELF )
		tskid = (curtcb - tcbtbl) + 1;
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* 状態チェック */
	if ( tcb->tskstat == TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* 優先度の変更 */
	if ( tskpri == TPRI_INI )
		tskpri = tcbstbl[tskid - 1].itskpri;
	tcb->tskpri = tskpri;
	if ( tcb->tskstat == TTS_RDY || tcb->tskstat == TTS_RUN ) {
		/* レディーキューの繋ぎ替え */
		__del_que(tcb);
		__adt_que(&rdyque[tskpri - 1], tcb);
	}
	
	/* ディスパッチ */
	__tsk_dsp();
	
	__res_imsk();
	
	return E_OK;
}


/* レディーキューの回転 */
ER  rot_rdq(PRI tskpri)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* パラメーターチェック */
	if ( tskpri > rdqcnt )
		return E_PAR;
#endif
	
	__set_imsk();
	
	if ( tskpri == TPRI_RUN ) {
		if ( curtcb == NADR ) {
			__res_imsk();
			return E_OK;	/* タスクが無い場合は何もしない */
		}
		tskpri = curtcb->tskpri;
	}
	
	/* キューにタスクはあるか */
	if ( rdyque[tskpri - 1].tskcnt == 0 ) {
		__res_imsk();
		return E_OK;
	}
	
	/* レディーキュー回転 */
	__rot_que(&rdyque[tskpri - 1]);
	
	/* 必要であればディスパッチを行なう */
	tcb = rdyque[tskpri - 1].head;
	if ( curtcb->tskpri == tskpri && tcb != curtcb ) {
		if ( sysstat == TSS_TSK ) {
			/* ディスパッチ */
			curtcb->tskstat = TTS_RDY;
			tcb->tskstat    = TTS_RUN;
			__swc_dsp(tcb);
		}
		else {
			/* 遅延ディスパッチの準備 */
			dlydsp = TRUE;
		}
	}
	
	__res_imsk();
	
	return E_OK;
}


/* 他タスクの待ち状態解除 */
ER  rel_wai(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* IDチェック */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* タスク状態チェック */
	if ( !(tcb->tskstat & TTS_WAI) || tcb->tskwait == TTW_SPL ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* 待ち解除 */
	__wup_dsp(tcb, E_RLWAI);
	
	__res_imsk();
	
	return E_OK;
}


/* 自タスクのID参照 */
ER   get_tid(ID *p_tskid)
{
#if __ERR_CHECK_LEVEL >= 4
	if ( (sysstat & TSS_INDP) || curtcb == NADR ) {
		*p_tskid = FALSE;
		return E_OK;
	}
#endif
	
	*p_tskid = (curtcb - tcbtbl) + 1;
	
	return E_OK;
}


/* タスク状態参照 */
ER   ref_tsk(T_RTSK *pk_rtsk, ID tskid)
{
	T_TCB  *tcb;
	const T_TCBS *tcbs;
	
#if __ERR_CHECK_LEVEL >= 4
	/* IDチェック */
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
	if ( tskid == TSK_SELF && ((sysstat & TSS_INDP) || curtcb == NADR) )
		return E_ID;
#endif
	
	if ( tskid == TSK_SELF )
		tskid = (curtcb - tcbtbl) + 1;
	
	tcb  = &tcbtbl[tskid - 1];
	tcbs = &tcbstbl[tskid - 1];
	
	__set_imsk();
	
	pk_rtsk->exinf   = tcbs->exinf;
	pk_rtsk->tskpri  = tcb->tskpri;
	pk_rtsk->tskstat = tcb->tskstat;
	
	__res_imsk();
	
	return E_OK;
}




/* 他タスクを強制待ち状態へ移行 */
ER   sus_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* IDチェック */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	if ( (!(sysstat & TSS_INDP) && tcb == curtcb)
			|| tcb->tskstat == TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* 既に SUSPENDならネスト */
	if ( tcb->tskstat & TTS_SUS ) {
#if __ERR_CHECK_LEVEL >= 1
		/* ネストのオーバーフローチェック */
		if ( tcb->suscnt >= MAX_SUSCNT ) {
			__res_imsk();
			return E_QOVR;
		}
#endif
		tcb->suscnt++;
	}
	else {
		/* SUSPENDさせる */
		if ( tcb->tskstat == TTS_WAI )
			tcb->tskstat = TTS_WAS;
		else
			tcb->tskstat = TTS_SUS;
		
		/* タスク独立部で実行中のタスクを指定した場合 */
		if ( tcb == curtcb )
			dlydsp = TRUE;	/* 遅延ディスパッチ設定 */
	}
	
	__res_imsk();
	
	return E_OK;
}


/* 強制待ち状態のタスクを再開 */
ER   rsm_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* IDチェック */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* エラーチェック */
	if ( tcb == curtcb || !(tcb->tskstat & TTS_SUS) ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	if ( tcb->suscnt != 0 ) {
		/* ネストデクリメント */
		tcb->suscnt--;
	}
	else {
		/* SUSPEND 解除 */
		tcb->tskstat &= ~TTS_SUS;
		if ( tcb->tskstat != TTS_WAI ) {
			tcb->tskstat = TTS_RDY;
			
			/* ディスパッチ */
			__tsk_dsp();
		}
	}
	
	__res_imsk();
	
	return E_OK;
}


/* 強制待ち状態のタスクを強制再開 */
ER frsm_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* IDチェック */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* エラーチェック */
	if ( tcb == curtcb || !(tcb->tskstat & TTS_SUS) ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* 強制解除 */
	tcb->suscnt = 0;
	return rsm_tsk(tskid);
}


/* 自タスクを起床待ち状態へ移行 */
ER   slp_tsk(void)
{
	ER ercd;
	
#if __ERR_CHECK_LEVEL >= 3
	/* エラーチェック */
	if( sysstat != TSS_TSK )
		return E_CTX;
#endif
	
	__set_imsk();
	
	/* 起床要求のキューイングがあるか */
	if ( curtcb->wupcnt > 0 ) {
		curtcb->wupcnt--;
		__res_imsk();
		return E_OK;
	}
	
	/* 待ち状態に移行 */
	__del_que(curtcb);
	curtcb->tskstat = TTS_WAI;
	curtcb->tskwait = TTW_SLP;
	
	/* ディスパッチ */
	ercd = __tsk_dsp();
	
	__res_imsk();
	
	return ercd;
}


/* 他タスクの起床 */
ER   wup_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID チェック */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* 状態チェック */
	if ( tcb == curtcb || tcb->tskstat == TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* 待ち状態でなければキューイング */
	if ( !(tcb->tskstat & TTS_WAI) || tcb->tskwait != TTW_SLP ) {
#if __ERR_CHECK_LEVEL >= 1
		if ( tcb->wupcnt == MAX_WUPCNT ) {
			__res_imsk();
			return E_QOVR;
		}
#endif
		tcb->wupcnt++;
	}
	else {
		/* 待ち解除 */
		tcb->tskstat &= ~TTS_WAI;
		tcb->tskwait  = 0;
		*(tcb->sp)    = E_OK;
		if ( tcb->tskstat != TTS_SUS ) {
			/* レディーキューに接続 */
			tcb->tskstat = TTS_RDY;
			__adt_que(&rdyque[tcb->tskpri - 1], tcb);
			
			/* ディスパッチ */
			__tsk_dsp();
		}
	}
	
	__res_imsk();
	
	return E_OK;
}


/* タスク起床要求を無効化 */
ER   can_wup(ID tskid)
{
	T_TCB *tcb;
	INT   wupcnt;
	
#if __ERR_CHECK_LEVEL >= 4
	/* IDチェック */
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
	if ( tskid == TSK_SELF && (sysstat & TSS_INDP) )
		return E_ID;
#endif
	
	if ( tskid == TSK_SELF )
		tcb = curtcb;
	else
		tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
	wupcnt = tcb->wupcnt;
	tcb->wupcnt = 0;
	
	__res_imsk();
	
	return wupcnt;
}



/* タスクのディスパッチ(割込み禁止は呼び出し側で必要) */
ER  __tsk_dsp(void)
{
	T_TCB *tcb;
	int   i, j;
	
	/* ディスパッチ可能か */
	if ( sysstat != TSS_TSK ) {
		dlydsp = TRUE;
		return E_CTX;
	}
	
	/* ディスパッチ実行 */
	dlydsp = FALSE;
	
	/* 指定優先度の次のタスクを探す */
	for ( i = 0; i < rdqcnt; i++ ) {
		if ( rdyque[i].tskcnt > 0 ) {
			tcb = rdyque[i].head;
			/* サスペンドで無い先頭タスクを探す */
			for ( j = 0; j < rdyque[i].tskcnt; j++ ) {
				if ( !(tcb->tskstat & TTS_SUS) )
					break;
				tcb = tcb->next;
			}
			if ( j >= rdyque[i].tskcnt )
				continue;
			
			/* 最初に見つけたタスクを RUNにする */
			tcb->tskstat = TTS_RUN;
			if ( curtcb == NADR || curtcb->tskstat == TTS_DMT ) {
				curtcb = tcb;
				__sta_dsp(tcb);		/* タスク開始 */
			}
			else {
				if ( tcb == curtcb )
					return E_OK;
				if ( curtcb->tskstat == TTS_RUN )
					curtcb->tskstat = TTS_RDY;
				return __swc_dsp(tcb);		/* タスク切り替え */
			}
		}
	}
	
	/* 次に実行するタスクが無い */
	if ( curtcb != NADR && curtcb->tskstat != TTS_DMT )
		return __end_dsp();
	
	/* 実行タスクなし */
	__tsk_hlt();
}


/* 待ち状態解除 */
void __wup_dsp(T_TCB *tcb, ER ercd)
{
	/* 待ち状態解除 */
	tcb->tskwait  = 0;
	*(tcb->sp)    = (UH)ercd;
	if ( tcb->que != NADR )
		__del_que(tcb);
	
	/* レディーキューへ接続 */
	__adt_que(&rdyque[tcb->tskpri - 1], tcb);
	
	/* 状態復帰 */
	if ( tcb->tskstat == TTS_WAS ) {
		tcb->tskstat = TTS_SUS;
	}
	else {
		tcb->tskstat = TTS_RDY;
		__tsk_dsp();
	}
}

