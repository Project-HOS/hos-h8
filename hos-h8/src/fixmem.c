/* ------------------------------------------------------------------------- */
/*  HOS ����Ĺ����ס������                                               */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#include <itron.h>


/* -------------------------------------------------------------- */
/*  �������ס���϶����֥�å�����Ƭ��ʬ��Ȥäƶ����֥�å�  */
/*  �Υꥹ�Ȥ��뤳�Ȥˤ�äƴ������롣�ꥹ�Ȥ�Ϣ�뤵��Ƥ��ʤ�  */
/*  ��ʬ��������Ǥ��롣�Ǿ��֥�å��������� sizeof(void *)       */
/* -------------------------------------------------------------- */


/* ����Ĺ����ס���ν���� */
void __ini_mpf(void)
{
	UB i;
	UH j;
	UH cnt;
	UH sz;
	UB *blk;
	
	for ( i = 0; i < fmcbcnt; i++ ) {
		fmcbtbl[i].que.tskcnt = 0;
		fmcbtbl[i].freeblk = fmcbstbl[i].mpfhead;
		blk = (UB *)fmcbstbl[i].mpfhead;
		cnt = fmcbstbl[i].mpfcnt - 1;
		sz  = fmcbstbl[i].mpfsz;
		for ( j = 0; j < cnt; j++ ) {
			*(VP *)blk = (VP)(blk + sz);
			blk += sz;
		}
		*(VP *)blk = NADR;
	}
}


/* ����Ĺ����֥�å����� */
ER get_blf(VP *p_blf, ID mpfid)
{
	T_FMCB *fmcb;
	const T_FMCBS *fmcbs;
	ER ercd;
	
#if __ERR_CHECK_LEVEL >= 4
	if ( mpfid <= 0 )
		return E_ID;
	if ( mpfid > fmcbcnt || fmcbstbl[mpfid - 1].mpfcnt == 0 )
		return E_NOEXS;
#endif
	
#if __ERR_CHECK_LEVEL >= 3
	if ( sysstat != TSS_TSK )
		return E_CTX;
#endif
	
	fmcb  = &fmcbtbl[mpfid - 1];
	fmcbs = &fmcbstbl[mpfid - 1];
	
	__set_imsk();
	
	/* ���꤬����ʤ������Ƥ� */
	if ( fmcb->freeblk != NADR ) {
		*p_blf = fmcb->freeblk;
		fmcb->freeblk = *(VP *)fmcb->freeblk;
		
		__res_imsk();
		
		return E_OK;
	}
	
	/* �Ԥ����֤ˤ��� */
	curtcb->tskstat = TTS_WAI;
	curtcb->tskwait = TTW_SEM;
	__del_que(curtcb);
	/* ����Ĺ����ס�����Ԥ�������ɲ� */
	__adt_que(&fmcb->que, curtcb);
	
	/* �ǥ����ѥå� */
	ercd = __tsk_dsp();
	
	/* ����֥�å����å� */
	*p_blf = curtcb->data;	/* ercd == E_OK �ʤ� data �˥��ɥ쥹������ */
	
	__res_imsk();
	
	return ercd;
}


/* ����Ĺ����֥�å������ʥݡ���󥰡� */
ER pget_blf(VP *p_blf, ID mpfid)
{
	T_FMCB *fmcb;
	const T_FMCBS *fmcbs;
	
#if __ERR_CHECK_LEVEL >= 4
	if ( mpfid <= 0 )
		return E_ID;
	if ( mpfid > fmcbcnt || fmcbstbl[mpfid - 1].mpfcnt == 0 )
		return E_NOEXS;
#endif
	
	fmcb  = &fmcbtbl[mpfid - 1];
	fmcbs = &fmcbstbl[mpfid - 1];
	
	__set_imsk();
	
	/* �������̵꤬����м��� */
	if ( fmcb->freeblk == NADR ) {
		__res_imsk();
		return E_TMOUT;
	}
	
	/* ���꤬����ʤ������Ƥ� */
	*p_blf = fmcb->freeblk;
	fmcb->freeblk = *(VP *)fmcb->freeblk;
	
	__res_imsk();
	
	return E_OK;
}


/* ����Ĺ����֥�å��ֵ� */
ER rel_blf(ID mpfid, VP blf)
{
	T_FMCB *fmcb;
	const T_FMCBS *fmcbs;
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	if ( mpfid <= 0 )
		return E_ID;
	if ( mpfid > fmcbcnt || fmcbstbl[mpfid - 1].mpfcnt == 0 )
		return E_NOEXS;
#endif
	
	fmcb  = &fmcbtbl[mpfid - 1];
	fmcbs = &fmcbstbl[mpfid - 1];
	
	__set_imsk();
	
	if ( fmcb->que.tskcnt == 0 ) {
		/* �Ԥ���������̵����Х��곫�� */
		*(VP *)blf    = fmcb->freeblk;
		fmcb->freeblk = blf;
	}
	else {
		/* �Ԥ��������˥�����Ϥ����Ԥ���� */
		tcb = fmcb->que.head;
		tcb->data = blf;
		__wup_dsp(tcb, E_OK);
	}
	
	__res_imsk();
	
	return E_OK;
}


/* ����Ĺ����ס�����ֻ��� */
ER ref_mpf(T_RMPF *pk_rsem, ID mpfid)
{
	T_FMCB *fmcb;
	const T_FMCBS *fmcbs;
	UH cnt;
	VP adr;
	
#if __ERR_CHECK_LEVEL >= 4
	if ( mpfid <= 0 )
		return E_ID;
	if ( mpfid > fmcbcnt || fmcbstbl[mpfid - 1].mpfcnt == 0 )
		return E_NOEXS;
#endif
	
	fmcb  = &fmcbtbl[mpfid - 1];
	fmcbs = &fmcbstbl[mpfid - 1];
	
	__set_imsk();
	
	/* �����֥�å�������� */
	cnt = 0;
	adr = fmcb->freeblk;
	while ( adr != NADR ) {
		adr = *(VP *)adr;
		cnt++;
	}
	
	/* �Ԥ��������ɣĤμ��� */
	if ( fmcb->que.tskcnt == 0 )
		pk_rsem->wtsk = FALSE;
	else
		pk_rsem->wtsk = fmcb->que.head - tcbtbl + 1;
	
	__res_imsk();
	
	pk_rsem->exinf  = fmcbs->exinf;	/* ��ĥ���� */
	pk_rsem->frbcnt = cnt;			/* ��������� */
	
	return E_OK;
}

