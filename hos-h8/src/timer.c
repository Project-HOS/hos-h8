/* ------------------------------------------------------------------------- */
/*  HOS 時間管理部分                                                         */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#include <itron.h>


/* 24bit演算ルーチン */
void __add_systim(void);	/* タイマ加算 */
void __add_tim(SYSTIME *pk_tim, DLYTIME *dlytim);	/* 時間加算 */
int  __cmp_tim(SYSTIME *pk_tim1, SYSTIME *pk_tim2);	/* 時間比較 */


/* グローバル変数 */
SYSTIME  systim;	/* システムクロック */
T_QUE    dlyque;	/* タスク遅延中の待ち行列 */
UH       timint;	/* タイマルーチンが呼ばれる間隔 */


/* タイマ初期化 */
void __ini_tim(void)
{
	/* タイマ用グローバル変数初期化 */
	timint         = 1;		/* デフォルトで1ms */
	dlyque.tskcnt  = 0;
	systim.utime   = 0;
	systim.ltime   = 0;
}


/* タイマハンドラ */
void __timer_handler(void)
{
	T_TCB *tcb;
	
	/* カウンタの加算 */
	__add_systim();
	
	/* 時間待ちタスクの復帰 */
	while ( dlyque.tskcnt > 0 ) {
		tcb = dlyque.head;
		if ( !__cmp_tim((SYSTIME *)tcb->data, &systim) )
			break;
		__set_imsk();
		__wup_dsp(tcb, E_OK);
	}
}


/* システムクロック設定 */
ER   set_tim(SYSTIME *pk_tim)
{
	__set_imsk();
	
	systim = *pk_tim;
	
	__res_imsk();
	
	return E_OK;
}


/* システムクロック参照 */
ER   get_tim(SYSTIME *pk_tim)
{
	__set_imsk();

	*pk_tim = systim;

	__res_imsk();

	return E_OK;
}


/* タスク遅延 */
ER   dly_tsk(DLYTIME dlytim)
{
	SYSTIME st;
	T_TCB   *tcb;
	ER      ercd;
	
#if __ERR_CHECK_LEVEL >= 3
	/* タスク独立部やディスパッチ禁止状態では呼べない */
	if ( sysstat != TSS_TSK )
		return E_CTX;
#endif
#if __ERR_CHECK_LEVEL >= 4
	if ( dlytim < 0 )
		return E_PAR;
#endif
	
	__set_imsk();
	
	/* 待ち状態に移行 */
	__del_que(curtcb);
	curtcb->tskstat = TTS_WAI;
	curtcb->tskwait = TTW_DLY;
	
	/* 待ち終了時間計算 */
	st = systim;
	__add_tim(&st, &dlytim);
	curtcb->data = (VP)&st;
	
	/* 時間待ちキューにセット */
	if ( dlyque.tskcnt == 0 ) {
		curtcb->que  = &dlyque;
		curtcb->next = curtcb;
		curtcb->prev = curtcb;
		dlyque.head  = curtcb;
		dlyque.tskcnt++;
	}
	else {
		/* 起床時間順に並べる */
		tcb = dlyque.head;
		for ( ; ; ) {
			tcb = tcb->prev;
			if ( __cmp_tim((SYSTIME *)(tcb->data), &st) ) {
				__ins_que(tcb, curtcb);
				break;
			}
			if ( tcb == dlyque.head ) {
				__adh_que(&dlyque, curtcb);
				break;
			}
		}
	}
	
	
	/* ディスパッチ */
	ercd = __tsk_dsp();
	
	
	__res_imsk();
	
	return ercd;
}


/* システムタイマ加算 (systim += timint) */
void __add_systim(void)
{
	UW last;
	
	last = systim.ltime;
	systim.ltime += timint;
	if ( systim.ltime < last )
		systim.utime++;		/* キャリーの繰り上げ */
}


/* 遅延時間の加算  (*pk_tim += *dlytim) */
void __add_tim(SYSTIME *pk_tim, DLYTIME *dlytim)
{
	UW last;
	
	last = pk_tim->ltime;
	pk_tim->ltime += *dlytim;
	if (pk_tim->ltime < last)
		pk_tim->utime++;	/* キャリーの繰り上げ */
}


/* 時間比較（pk_tim1 が pk_time2 以下なら真） */
int __cmp_tim(SYSTIME *pk_tim1, SYSTIME *pk_tim2)
{
	if ( pk_tim1->utime < pk_tim2->utime )
    	return TRUE;
	if ( pk_tim1->utime > pk_tim2->utime )
		return FALSE;
	
	if ( pk_tim1->ltime <= pk_tim2->ltime )
		return TRUE;
	
	return FALSE;
}
