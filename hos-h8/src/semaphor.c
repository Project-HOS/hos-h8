/* ------------------------------------------------------------------------- */
/*  HOS ���ޥե�����                                                         */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#include <itron.h>


/* ���ޥե��ν���� */
void __ini_sem(void)
{
	int i;
	
	for ( i = 0; i < scbcnt; i++ ) {
		scbtbl[i].semcnt = scbstbl[i].isemcnt;
		scbtbl[i].que.tskcnt = 0;
	}
}


/* ���ޥե��ֵ� */
ER   sig_sem(ID semid)
{
	T_SCB  *scb;
	const T_SCBS *scbs;
	T_TCB  *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	if ( semid <= 0 )
		return E_ID;
	if ( semid > scbcnt || scbstbl[semid - 1].maxsem == 0 )
		return E_NOEXS;
#endif
	
	scb  = &scbtbl[semid - 1];
	scbs = &scbstbl[semid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 1
	/* �����С��ե������å� */
	if ( scb->semcnt >= scbs->maxsem ) {
		__res_imsk();
		return E_QOVR;
	}
#endif
	
	if ( scb->que.tskcnt == 0 ) {
		scb->semcnt++;
	}
	else {
		/* ���塼��Ƭ���Ԥ���� */
		tcb = scb->que.head;
		__wup_dsp(tcb, E_OK);
	}
	
	__res_imsk();
	
	return E_OK;
}


/* ���ޥե����Ԥ� */
ER   wai_sem(ID semid)
{
	T_SCB  *scb;
	ER     ercd;
	
#if __ERR_CHECK_LEVEL >= 4
	if ( semid <= 0 )
		return E_ID;
	if ( semid > scbcnt || scbstbl[semid - 1].maxsem == 0 )
		return E_NOEXS;
#endif
	
#if __ERR_CHECK_LEVEL >= 3
	if ( sysstat != TSS_TSK )
		return E_CTX;
#endif
	
	scb  = &scbtbl[semid - 1];
	
	__set_imsk();
	
	if ( scb->semcnt > 0 ) {
		scb->semcnt--;
		__res_imsk();
		return E_OK;
	}
	
	/* �Ԥ����֤ˤ��� */
	curtcb->tskstat = TTS_WAI;
	curtcb->tskwait = TTW_SEM;
	__del_que(curtcb);
	/* ���ޥե����Ԥ�������ɲ� */
	__adt_que(&scb->que, curtcb);
	
	/* �ǥ����ѥå� */
	ercd = __tsk_dsp();
	
	__res_imsk();
	
	return ercd;
}


/* ���ޥե����Ԥ�(�ݡ����) */
ER   preq_sem(ID semid)
{
	T_SCB  *scb;
	
#if __ERR_CHECK_LEVEL >= 4
	if ( semid <= 0 )
		return E_ID;
	if ( semid > scbcnt || scbstbl[semid - 1].maxsem == 0 )
		return E_NOEXS;
#endif
	
	scb  = &scbtbl[semid - 1];
	
	__set_imsk();
	
	if ( scb->semcnt > 0 ) {
		scb->semcnt--;
		__res_imsk();
		return E_OK;
	}
	
	__res_imsk();
	
	return E_TMOUT;
}


/* ���ޥե����ֻ��� */
ER   ref_sem(T_RSEM *pk_rsem, ID semid)
{
	T_SCB  *scb;
	const T_SCBS *scbs;
	
#if __ERR_CHECK_LEVEL >= 4
	if ( semid <= 0 )
		return E_ID;
	if ( semid > scbcnt || scbstbl[semid - 1].maxsem == 0 )
		return E_NOEXS;
#endif
	
	scb  = &scbtbl[semid - 1];
	scbs = &scbstbl[semid - 1];
	
	__set_imsk();

	pk_rsem->exinf = scbs->exinf;
	if ( scb->que.tskcnt == 0 )
		pk_rsem->wtsk = 0;
	else
		pk_rsem->wtsk = (BOOL_ID)(scb->que.head - tcbtbl + 1);
	pk_rsem->semcnt = (INT)scb->semcnt;
	
	__res_imsk();
	
	return E_OK;
}
