/* ------------------------------------------------------------------------- */
/*  HOS ���٥�ȥե饰����                                                   */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#include <itron.h>


/* ���٥�ȥե饰�Ԥ����� */
typedef struct t_wflg_stat {
	UB  waiptn;	/* �Ԥ��ӥåȥѥ����� */
	UB  wfmode;	/* �Ԥ��⡼�� */
} T_WFLG_STAT;


/* �ե饰�Υ����å� */
BOOL __chk_flg(T_FCB *fcb, T_WFLG_STAT *wflg_stat);


/* ���٥�ȥե饰����� */
void __ini_flg(void)
{
	int i;
	
	for ( i = 0; i < fcbcnt; i++ ) {
		fcbtbl[i].flgptn     = fcbstbl[i].iflgptn;
		fcbtbl[i].que.tskcnt = 0;
	}
}


/* ���٥�ȥե饰���å� */
ER   set_flg(ID flgid, UB setptn)
{
	T_FCB *fcb;
	T_TCB *tcb;
	T_TCB *tcbNext;
	UB    old_syssat;
	BOOL  reqdsp;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( flgid <= 0 )
		return E_ID;
	if ( flgid > fcbcnt )
		return E_NOEXS;
#endif
	
	fcb = &fcbtbl[flgid - 1];
	
	__set_imsk();
	
	/* �ե饰���å� */
	fcb->flgptn |= setptn;
	
	/* �Ԥ��Υ������Υ����å� */
	if ( fcb->que.tskcnt > 0 ) {
		/* ���Ū�˥ǥ����ѥå���ػ� */
		old_syssat = sysstat;
		sysstat   |= TSS_DDSP;
		reqdsp     = FALSE;
		
		/* �ե饰�ΰ��פ��륿���������Ƶ��� */
		tcb = fcb->que.head;
		do {
			tcbNext = tcb->next;
			if ( __chk_flg(fcb, (T_WFLG_STAT *)tcb->data) ) {
				__wup_dsp(tcb, E_OK);
				reqdsp = TRUE;
			}
			tcb = tcbNext;
		} while ( tcb != fcb->que.head );
		
		/* �������� */
		sysstat = old_syssat;
		
		/* ��������������������ʤ�ǥ����ѥå� */
		if ( reqdsp )
			__tsk_dsp();
	}
	
	__res_imsk();
	
	return E_OK;
}


/* ���٥�ȥե饰�Υ��ꥢ */
ER   clr_flg(ID flgid, UB clrptn)
{
	T_FCB *fcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( flgid <= 0 )
		return E_ID;
	if ( flgid > fcbcnt )
		return E_NOEXS;
#endif
	
	fcb = &fcbtbl[flgid - 1];
	
	__set_imsk();
	
	/* �ե饰���ꥢ */
	fcb->flgptn &= clrptn;
	
	__res_imsk();
	
	return E_OK;
}


/* ���٥�ȥե饰�Ԥ� */
ER   wai_flg(UB *p_flgptn, ID flgid, UB waiptn, UB wfmode)
{
	T_WFLG_STAT wflg_stat;
	T_FCB  *fcb;
	INT    ercd;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( flgid <= 0 )
		return E_ID;
	if ( flgid > fcbcnt )
		return E_NOEXS;
	if ( wfmode > 3 || waiptn == 0 )
		return E_PAR;
#endif
	
	fcb = &fcbtbl[flgid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 3
	/* ���֥����å� */
	if ( sysstat != 0 ) {
		__res_imsk();
		return E_CTX;
	}
#endif
	
	if ( p_flgptn != NADR )
		*p_flgptn = fcb->flgptn;
	
	/* �ե饰�����å� */
	wflg_stat.waiptn = waiptn;
	wflg_stat.wfmode = wfmode;
	if ( __chk_flg(fcb, &wflg_stat) ) {
		__res_imsk();
		return E_OK;
	}
	
	/* �Ԥ����֤˰ܹ� */
	__del_que(curtcb);
	curtcb->tskstat = TTS_WAI;
	curtcb->tskwait = TTW_FLG;
	curtcb->data    = (VP)&wflg_stat;
	__adt_que(&fcb->que, curtcb);
	
	/* �ǥ����ѥå� */
	ercd = __tsk_dsp();
	
	__res_imsk();
	
	return ercd;
}


/* ���٥�ȥե饰�Ԥ�(�ݡ����) */
ER   pol_flg(UB *p_flgptn, ID flgid, UB waiptn, UB wfmode)
{
	T_WFLG_STAT wflg_stat;
	T_FCB  *fcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( flgid <= 0 )
		return E_ID;
	if ( flgid > fcbcnt )
		return E_NOEXS;
	if ( wfmode > 3 || waiptn == 0 )
		return E_PAR;
#endif
	
	fcb = &fcbtbl[flgid - 1];
	
	__set_imsk();
	
	/* �ե饰�����å� */
	wflg_stat.waiptn = waiptn;
	wflg_stat.wfmode = wfmode;
	if ( __chk_flg(fcb, &wflg_stat) ) {
		if ( p_flgptn != NADR )
			*p_flgptn = fcb->flgptn;
		__res_imsk();
		return E_OK;
	}
	
	__res_imsk();
	
	return E_TMOUT;
}


/* ���٥�ȥե饰���ֻ��� */
ER   ref_flg(T_RFLG *pk_rflg, ID flgid)
{
	T_FCB  *fcb;
	const T_FCBS *fcbs;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( flgid <= 0 )
		return E_ID;
	if ( flgid > fcbcnt )
		return E_NOEXS;
#endif
	
	fcb  = &fcbtbl[flgid - 1];
	fcbs = &fcbstbl[flgid - 1];
	
	__set_imsk();
	
	pk_rflg->exinf = fcbs->exinf;
	if ( fcb->que.tskcnt == 0 )
		pk_rflg->wtsk = 0;
	else
		pk_rflg->wtsk = (BOOL_ID)(fcb->que.head - tcbtbl + 1);
	pk_rflg->flgptn = fcb->flgptn;
	
	__res_imsk();
	
	return E_OK;
}


/* �ե饰�Υ����å� */
BOOL __chk_flg(T_FCB *fcb, T_WFLG_STAT *wflg_stat)
{
	/* �ե饰�����å� */
	if ( wflg_stat->wfmode & TWF_ORW ) {
		if ( !(fcb->flgptn & wflg_stat->waiptn) )
			return FALSE;
	}
	else {
		if ( (fcb->flgptn & wflg_stat->waiptn) != wflg_stat->waiptn )
			return FALSE;
	}
	
	/* TWF_CLR�����ꤵ��Ƥ���Хե饰�򥯥ꥢ */
	if ( wflg_stat->wfmode & TWF_CLR )
		fcb->flgptn = 0;
	
	return TRUE;
}

