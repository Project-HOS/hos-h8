/* ------------------------------------------------------------------------- */
/*  HOS ���ִ�����ʬ                                                         */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#include <itron.h>


/* 24bit�黻�롼���� */
void __add_systim(void);	/* �����޲û� */
void __add_tim(SYSTIME *pk_tim, DLYTIME *dlytim);	/* ���ֲû� */
int  __cmp_tim(SYSTIME *pk_tim1, SYSTIME *pk_tim2);	/* ������� */


/* �����Х��ѿ� */
SYSTIME  systim;	/* �����ƥ९��å� */
T_QUE    dlyque;	/* �������ٱ�����Ԥ����� */
UH       timint;	/* �����ޥ롼���󤬸ƤФ��ֳ� */


/* �����޽���� */
void __ini_tim(void)
{
	/* �������ѥ����Х��ѿ������ */
	timint         = 1;		/* �ǥե���Ȥ�1ms */
	dlyque.tskcnt  = 0;
	systim.utime   = 0;
	systim.ltime   = 0;
}


/* �����ޥϥ�ɥ� */
void __timer_handler(void)
{
	T_TCB *tcb;
	
	/* �����󥿤βû� */
	__add_systim();
	
	/* �����Ԥ������������� */
	while ( dlyque.tskcnt > 0 ) {
		tcb = dlyque.head;
		if ( !__cmp_tim((SYSTIME *)tcb->data, &systim) )
			break;
		__set_imsk();
		__wup_dsp(tcb, E_OK);
	}
}


/* �����ƥ९��å����� */
ER   set_tim(SYSTIME *pk_tim)
{
	__set_imsk();
	
	systim = *pk_tim;
	
	__res_imsk();
	
	return E_OK;
}


/* �����ƥ९��å����� */
ER   get_tim(SYSTIME *pk_tim)
{
	__set_imsk();

	*pk_tim = systim;

	__res_imsk();

	return E_OK;
}


/* �������ٱ� */
ER   dly_tsk(DLYTIME dlytim)
{
	SYSTIME st;
	T_TCB   *tcb;
	ER      ercd;
	
#if __ERR_CHECK_LEVEL >= 3
	/* ��������Ω����ǥ����ѥå��ػ߾��֤ǤϸƤ٤ʤ� */
	if ( sysstat != TSS_TSK )
		return E_CTX;
#endif
#if __ERR_CHECK_LEVEL >= 4
	if ( dlytim < 0 )
		return E_PAR;
#endif
	
	__set_imsk();
	
	/* �Ԥ����֤˰ܹ� */
	__del_que(curtcb);
	curtcb->tskstat = TTS_WAI;
	curtcb->tskwait = TTW_DLY;
	
	/* �Ԥ���λ���ַ׻� */
	st = systim;
	__add_tim(&st, &dlytim);
	curtcb->data = (VP)&st;
	
	/* �����Ԥ����塼�˥��å� */
	if ( dlyque.tskcnt == 0 ) {
		curtcb->que  = &dlyque;
		curtcb->next = curtcb;
		curtcb->prev = curtcb;
		dlyque.head  = curtcb;
		dlyque.tskcnt++;
	}
	else {
		/* �������ֽ���¤٤� */
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
	
	
	/* �ǥ����ѥå� */
	ercd = __tsk_dsp();
	
	
	__res_imsk();
	
	return ercd;
}


/* �����ƥॿ���޲û� (systim += timint) */
void __add_systim(void)
{
	UW last;
	
	last = systim.ltime;
	systim.ltime += timint;
	if ( systim.ltime < last )
		systim.utime++;		/* ����꡼�η���夲 */
}


/* �ٱ���֤βû�  (*pk_tim += *dlytim) */
void __add_tim(SYSTIME *pk_tim, DLYTIME *dlytim)
{
	UW last;
	
	last = pk_tim->ltime;
	pk_tim->ltime += *dlytim;
	if (pk_tim->ltime < last)
		pk_tim->utime++;	/* ����꡼�η���夲 */
}


/* ������ӡ�pk_tim1 �� pk_time2 �ʲ��ʤ鿿�� */
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
