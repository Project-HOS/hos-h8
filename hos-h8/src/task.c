/* ------------------------------------------------------------------------- */
/*  HOS ����������                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#include "itron.h"


/* �����Х��ѿ� */
T_TCB *curtcb;			/* ���ߤΥ�������TCB */
BOOL  dlydsp;			/* �ٱ䤵��Ƥ���ǥ����ѥå������뤫 */


/* ���������ν���� */
void __ini_tsk(void)
{
	int i, j;
	
	curtcb = NADR;
	dlydsp = FALSE;
	
	/* ��ǥ������塼�ν���� */
	for ( i = 0; i < rdqcnt; i++ )
		rdyque[i].tskcnt = 0;
	
	/* �����å��ס���ν���� */
	for ( i = 0; i < spcbcnt; i++ ) {
		spcbtbl[i].que.tskcnt = 0;
		for ( j = 0; j < spcbstbl[i].stkcnt - 1; j++ )
			spcbstbl[i].stkhead[spcbstbl[i].stksz * j] = (VH)(j + 1);
		spcbstbl[i].stkhead[spcbstbl[i].stksz * j] = 0xffff;
		spcbtbl[i].freestk = 0;
	}
	
	/* TCB�ơ��֥�ν���� */
	for ( i = 0; i < tcbcnt; i++ ) {
		if ( tcbstbl[i].itskpri != 0 )
			tcbtbl[i].tskstat = TTS_DMT;
		else 
			tcbtbl[i].tskstat = 0;	/* NON-EXISTENT */
		tcbtbl[i].que = NADR;
	}
}

/* �����å������Υ��ե��å������� ñ�̤�VH(WORD)*/
#ifndef __GNUC__ /* ��Ω�� */
 #ifndef __NORMAL_MODE__
  #define POINTER_SIZE_VH		2	/* ���ɥ쥹������ */  
  #define CONTEXT_STACK_SIZE_VH	10	/* callee saved�ʥ쥸����ʬ */
  #define OFFSET_PARAM_VH		1	/* �쥸����ʬ���Ѥ����Υѥ�᡼���ؤΥ��ե��å� */
 #else
  #define POINTER_SIZE_VH		1	/* ���ɥ쥹������ */  
	/* er6�β���WORD��16bit���ɥ쥹�������٤κٹ� */
  #define CONTEXT_STACK_SIZE_VH	9	/* callee saved�ʥ쥸����ʬ */
  #define OFFSET_PARAM_VH		2	/* �쥸����ʬ���Ѥ����Υѥ�᡼���ؤΥ��ե��å� */
 #endif
#else /* GNU�� */
 #ifndef __NORMAL_MODE__
  #define POINTER_SIZE_VH		2	/* ���ɥ쥹������ */  
  #define CONTEXT_STACK_SIZE_VH	8	/* callee saved�ʥ쥸����ʬ */
  #define OFFSET_PARAM_VH		1	/* �쥸����ʬ���Ѥ����Υѥ�᡼���ؤΥ��ե��å� */
 #else
  #define POINTER_SIZE_VH		1	/* ���ɥ쥹������ */  
	/* er6�β���WORD��16bit���ɥ쥹�������٤κٹ� */
  #define CONTEXT_STACK_SIZE_VH	7	/* callee saved�ʥ쥸����ʬ */
  #define OFFSET_PARAM_VH		2	/* �쥸����ʬ���Ѥ����Υѥ�᡼���ؤΥ��ե��å� */
 #endif
#endif

/* ���������� */
ER sta_tsk(ID tskid, INT stacd)
{
	T_TCB *tcb;
	const T_TCBS *tcbs;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb  = &tcbtbl[tskid - 1];
	tcbs = &tcbstbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* ���������֥����å� */
	if ( tcb->tskstat != TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* TCB������ */
	tcb->sp      = tcbs->isp;
	tcb->tskpri  = tcbs->itskpri;
	tcb->tskstat = TTS_RDY;
	tcb->tskwait = 0;
	tcb->wupcnt  = 0;
	tcb->suscnt  = 0;
	
	/* �����å��������å��ס�������ꤵ��Ƥ����� */
	if ( (UW)tcbs->isp & 0x00000001 ) {
		T_SPCB  *spcb;
		const T_SPCBS *spcbs;
		UW      spcbid;
		VH      *stack;
		
		spcbid = (UW)tcbs->isp >> 1;
		spcb   = &spcbtbl[spcbid];
		spcbs  = &spcbstbl[spcbid];
		
		/* �����å��������Ƥ��ʤ������Ԥ� */
		if ( spcb->freestk == 0xffff ) {
			__adt_que(&spcb->que, tcb);
			tcb->tskstat = TTS_WAI;
			tcb->tskwait = TTW_SPL;
			tcb->data    = (VP)(UW)stacd;
			
			__res_imsk();
			return E_OK;
		}
		
		
		/* �����å���ס��뤫����� */
		stack = &spcbs->stkhead[spcbs->stksz * spcb->freestk];
		spcb->freestk = *stack;
		tcb->sp = stack + spcbs->stksz;
	}
	
	/* �����å��ν������ */
	tcb->sp -= POINTER_SIZE_VH;
#if POINTER_SIZE_VH == 2
	*(FP *)tcb->sp = (FP)__tskst_entry;	/* �꥿���󥢥ɥ쥹 */
#else
	*tcb->sp = (VH)(FP)__tskst_entry;	/* �꥿���󥢥ɥ쥹 */
#endif
	tcb->sp -= CONTEXT_STACK_SIZE_VH;
#if POINTER_SIZE_VH == 2
	*(FP *)tcb->sp = tcbs->task;		/* ���������ϥ��ɥ쥹 */
#else
	*tcb->sp = (VH)tcbs->task;		/* ���������ϥ��ɥ쥹 */
#endif
	tcb->sp -= OFFSET_PARAM_VH;
	*(tcb->sp) = (VH)stacd;			/* ����������� */
	
	/* READY���塼���ɲ� */
	__adt_que(&rdyque[tcb->tskpri - 1], tcb);
	
	/* �ǥ����ѥå� */
	__tsk_dsp();
	
	__res_imsk();
	
	return E_OK;
}


/* �����å��������å��ס���Τ�Τʤ��ֵ� */
void __rel_stp(T_TCB *tcb)
{
	const T_TCBS  *tcbs;
	T_SPCB  *spcb;
	const T_SPCBS *spcbs;
	UH spcbid;
	UH stack;
	
	tcbs = &tcbstbl[tcb - tcbtbl];
	
	/* �����å��ס�������Ѥ��Ƥ��뤫�ɤ����� */
	if ( !((UW)tcbs->isp & 0x00000001) )
		return;
	
	/* ���Ѥ��Ƥ��륹���å��򻻽� */
	spcbid = (UW)tcbs->isp >> 1;
	spcb   = &spcbtbl[spcbid];
	spcbs  = &spcbstbl[spcbid];
	stack  = ((VH *)tcb->sp - spcbs->stkhead - 1) / spcbs->stksz;
	
	/* �Ԥ�������������Х����å������ */
	if ( spcb->que.tskcnt > 0 ) {
		T_TCB  *tcbnext;
		const T_TCBS *tcbsnext;
		
		tcbnext  = spcb->que.head;
		tcbsnext = &tcbstbl[tcbnext - tcbtbl];
		__del_que(tcbnext);
		tcbnext->sp = &spcbs->stkhead[(stack + 1) * spcbs->stksz];
		
		/* �����å��ν������ */
		tcbnext->sp -= POINTER_SIZE_VH;
#if POINTER_SIZE_VH == 2
		*(FP *)tcbnext->sp = (FP)__tskst_entry;	/* �꥿���󥢥ɥ쥹 */
#else
		*tcbnext->sp = (VH)(FP)__tskst_entry;	/* �꥿���󥢥ɥ쥹 */
#endif
		tcbnext->sp -= CONTEXT_STACK_SIZE_VH;
#if POINTER_SIZE_VH == 2
		*(FP *)tcbnext->sp = tcbsnext->task;	/* ���������ϥ��ɥ쥹 */
#else
		*tcbnext->sp = (VH)tcbsnext->task;	/* ���������ϥ��ɥ쥹 */
#endif

		tcbnext->sp -= OFFSET_PARAM_VH;
		*(tcbnext->sp) = (VH)(INT)tcbnext->data; /* ����������� */
		
		/* READY���塼���ɲ� */
		__adt_que(&rdyque[tcb->tskpri - 1], tcbnext);
		
		return;
	}
	
	/* �����å��ס�����ֵ� */
	spcbs->stkhead[stack * spcbs->stksz] = spcb->freestk;
	spcb->freestk = stack;
}


/* ��������λ */
void ext_tsk(void)
{
	
#if __ERR_CHECK_LEVEL >= 4
	/* ���顼�����å� */
	if ( sysstat != TSS_TSK ) {
		/* ��̿Ū���顼�����꥿�������ʤ� */
		sysstat = TSS_TSK;
	}
#endif
	
	__set_imsk();
	
	if ( curtcb != NADR ) {
		/* ���塼���鳰��DOMANT���֤˰ܹ� */
		if ( curtcb->que != NADR )
			__del_que(curtcb);
		curtcb->tskstat = TTS_DMT;
		
		/* �����å��������å��ס���Τ�Τʤ��ֵ� */
		__rel_stp(curtcb);
		
		curtcb = NADR;
	}
	
	/* �ǥ����ѥå� */
	__tsk_dsp();
}


/* ¾������������λ */
ER  ter_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* ���������֥����å� */
	if ( (!(sysstat & TSS_INDP) && tcb == curtcb)
						|| tcb->tskstat == TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* ��������λ */
	if ( tcb->que != NADR )
		__del_que(tcb);
	tcb->tskstat = TTS_DMT;
	
	/* �����å��������å��ס���Τ�Τʤ��ֵ� */
	__rel_stp(tcb);
	
	if ( curtcb == tcb )
		curtcb = NADR;	/* �󥿥��������鸽�߼¹��楿������λ������ */
	
	/* �ǥ����ѥå� */
	__tsk_dsp();
	
	__res_imsk();
	
	return E_OK;
}


/* �ǥ����ѥå��ػ� */
ER  dis_dsp(void)
{
#if __ERR_CHECK_LEVEL >= 3
	/* ���顼�����å� */
	if ( sysstat & (TSS_INDP | TSS_DINT) )
		return E_CTX;
#endif
	__set_imsk();
	
	sysstat = TSS_DDSP;
	
	__res_imsk();
	
	return E_OK;
}


/* �ǥ����ѥå����� */
ER  ena_dsp(void)
{
#if __ERR_CHECK_LEVEL >= 3
	/* ���顼�����å� */
	if ( sysstat & (TSS_INDP | TSS_DINT) )
		return E_CTX;
#endif
	
	__set_imsk();
	
	/* �ٱ�ǥ����ѥå� */
	sysstat = TSS_TSK;
	if ( dlydsp )
		__tsk_dsp();
	
	__res_imsk();
	
	return E_OK;
}


/* ������ͥ�����ѹ� */
ER  chg_pri(ID tskid, PRI tskpri)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* �ѥ�᡼���������å� */
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
	/* ���֥����å� */
	if ( tcb->tskstat == TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* ͥ���٤��ѹ� */
	if ( tskpri == TPRI_INI )
		tskpri = tcbstbl[tskid - 1].itskpri;
	tcb->tskpri = tskpri;
	if ( tcb->tskstat == TTS_RDY || tcb->tskstat == TTS_RUN ) {
		/* ��ǥ������塼�ηҤ��ؤ� */
		__del_que(tcb);
		__adt_que(&rdyque[tskpri - 1], tcb);
	}
	
	/* �ǥ����ѥå� */
	__tsk_dsp();
	
	__res_imsk();
	
	return E_OK;
}


/* ��ǥ������塼�β�ž */
ER  rot_rdq(PRI tskpri)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* �ѥ�᡼���������å� */
	if ( tskpri > rdqcnt )
		return E_PAR;
#endif
	
	__set_imsk();
	
	if ( tskpri == TPRI_RUN ) {
		if ( curtcb == NADR ) {
			__res_imsk();
			return E_OK;	/* ��������̵�����ϲ��⤷�ʤ� */
		}
		tskpri = curtcb->tskpri;
	}
	
	/* ���塼�˥������Ϥ��뤫 */
	if ( rdyque[tskpri - 1].tskcnt == 0 ) {
		__res_imsk();
		return E_OK;
	}
	
	/* ��ǥ������塼��ž */
	__rot_que(&rdyque[tskpri - 1]);
	
	/* ɬ�פǤ���Хǥ����ѥå���Ԥʤ� */
	tcb = rdyque[tskpri - 1].head;
	if ( curtcb->tskpri == tskpri && tcb != curtcb ) {
		if ( sysstat == TSS_TSK ) {
			/* �ǥ����ѥå� */
			curtcb->tskstat = TTS_RDY;
			tcb->tskstat    = TTS_RUN;
			__swc_dsp(tcb);
		}
		else {
			/* �ٱ�ǥ����ѥå��ν��� */
			dlydsp = TRUE;
		}
	}
	
	__res_imsk();
	
	return E_OK;
}


/* ¾���������Ԥ����ֲ�� */
ER  rel_wai(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* ���������֥����å� */
	if ( !(tcb->tskstat & TTS_WAI) || tcb->tskwait == TTW_SPL ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* �Ԥ���� */
	__wup_dsp(tcb, E_RLWAI);
	
	__res_imsk();
	
	return E_OK;
}


/* ����������ID���� */
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


/* ���������ֻ��� */
ER   ref_tsk(T_RTSK *pk_rtsk, ID tskid)
{
	T_TCB  *tcb;
	const T_TCBS *tcbs;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
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




/* ¾�����������Ԥ����֤ذܹ� */
ER   sus_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
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
	
	/* ���� SUSPEND�ʤ�ͥ��� */
	if ( tcb->tskstat & TTS_SUS ) {
#if __ERR_CHECK_LEVEL >= 1
		/* �ͥ��ȤΥ����С��ե������å� */
		if ( tcb->suscnt >= MAX_SUSCNT ) {
			__res_imsk();
			return E_QOVR;
		}
#endif
		tcb->suscnt++;
	}
	else {
		/* SUSPEND������ */
		if ( tcb->tskstat == TTS_WAI )
			tcb->tskstat = TTS_WAS;
		else
			tcb->tskstat = TTS_SUS;
		
		/* ��������Ω���Ǽ¹���Υ���������ꤷ����� */
		if ( tcb == curtcb )
			dlydsp = TRUE;	/* �ٱ�ǥ����ѥå����� */
	}
	
	__res_imsk();
	
	return E_OK;
}


/* �����Ԥ����֤Υ�������Ƴ� */
ER   rsm_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* ���顼�����å� */
	if ( tcb == curtcb || !(tcb->tskstat & TTS_SUS) ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	if ( tcb->suscnt != 0 ) {
		/* �ͥ��ȥǥ������ */
		tcb->suscnt--;
	}
	else {
		/* SUSPEND ��� */
		tcb->tskstat &= ~TTS_SUS;
		if ( tcb->tskstat != TTS_WAI ) {
			tcb->tskstat = TTS_RDY;
			
			/* �ǥ����ѥå� */
			__tsk_dsp();
		}
	}
	
	__res_imsk();
	
	return E_OK;
}


/* �����Ԥ����֤Υ����������Ƴ� */
ER frsm_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* ���顼�����å� */
	if ( tcb == curtcb || !(tcb->tskstat & TTS_SUS) ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* ������� */
	tcb->suscnt = 0;
	return rsm_tsk(tskid);
}


/* ���������򵯾��Ԥ����֤ذܹ� */
ER   slp_tsk(void)
{
	ER ercd;
	
#if __ERR_CHECK_LEVEL >= 3
	/* ���顼�����å� */
	if( sysstat != TSS_TSK )
		return E_CTX;
#endif
	
	__set_imsk();
	
	/* �����׵�Υ��塼���󥰤����뤫 */
	if ( curtcb->wupcnt > 0 ) {
		curtcb->wupcnt--;
		__res_imsk();
		return E_OK;
	}
	
	/* �Ԥ����֤˰ܹ� */
	__del_que(curtcb);
	curtcb->tskstat = TTS_WAI;
	curtcb->tskwait = TTW_SLP;
	
	/* �ǥ����ѥå� */
	ercd = __tsk_dsp();
	
	__res_imsk();
	
	return ercd;
}


/* ¾�������ε��� */
ER   wup_tsk(ID tskid)
{
	T_TCB *tcb;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID �����å� */
	if ( tskid <= 0 )
		return E_ID;
	if ( tskid > tcbcnt || tcbtbl[tskid - 1].tskstat == 0 )
		return E_NOEXS;
#endif
	
	tcb = &tcbtbl[tskid - 1];
	
	__set_imsk();
	
#if __ERR_CHECK_LEVEL >= 2
	/* ���֥����å� */
	if ( tcb == curtcb || tcb->tskstat == TTS_DMT ) {
		__res_imsk();
		return E_OBJ;
	}
#endif
	
	/* �Ԥ����֤Ǥʤ���Х��塼���� */
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
		/* �Ԥ���� */
		tcb->tskstat &= ~TTS_WAI;
		tcb->tskwait  = 0;
		*(tcb->sp)    = E_OK;
		if ( tcb->tskstat != TTS_SUS ) {
			/* ��ǥ������塼����³ */
			tcb->tskstat = TTS_RDY;
			__adt_que(&rdyque[tcb->tskpri - 1], tcb);
			
			/* �ǥ����ѥå� */
			__tsk_dsp();
		}
	}
	
	__res_imsk();
	
	return E_OK;
}


/* �����������׵��̵���� */
ER   can_wup(ID tskid)
{
	T_TCB *tcb;
	INT   wupcnt;
	
#if __ERR_CHECK_LEVEL >= 4
	/* ID�����å� */
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



/* �������Υǥ����ѥå�(����߶ػߤϸƤӽФ�¦��ɬ��) */
ER  __tsk_dsp(void)
{
	T_TCB *tcb;
	int   i, j;
	
	/* �ǥ����ѥå���ǽ�� */
	if ( sysstat != TSS_TSK ) {
		dlydsp = TRUE;
		return E_CTX;
	}
	
	/* �ǥ����ѥå��¹� */
	dlydsp = FALSE;
	
	/* ����ͥ���٤μ��Υ�������õ�� */
	for ( i = 0; i < rdqcnt; i++ ) {
		if ( rdyque[i].tskcnt > 0 ) {
			tcb = rdyque[i].head;
			/* �����ڥ�ɤ�̵����Ƭ��������õ�� */
			for ( j = 0; j < rdyque[i].tskcnt; j++ ) {
				if ( !(tcb->tskstat & TTS_SUS) )
					break;
				tcb = tcb->next;
			}
			if ( j >= rdyque[i].tskcnt )
				continue;
			
			/* �ǽ�˸��Ĥ����������� RUN�ˤ��� */
			tcb->tskstat = TTS_RUN;
			if ( curtcb == NADR || curtcb->tskstat == TTS_DMT ) {
				curtcb = tcb;
				__sta_dsp(tcb);		/* ���������� */
			}
			else {
				if ( tcb == curtcb )
					return E_OK;
				if ( curtcb->tskstat == TTS_RUN )
					curtcb->tskstat = TTS_RDY;
				return __swc_dsp(tcb);		/* �������ڤ��ؤ� */
			}
		}
	}
	
	/* ���˼¹Ԥ��륿������̵�� */
	if ( curtcb != NADR && curtcb->tskstat != TTS_DMT )
		return __end_dsp();
	
	/* �¹ԥ������ʤ� */
	__tsk_hlt();
}


/* �Ԥ����ֲ�� */
void __wup_dsp(T_TCB *tcb, ER ercd)
{
	/* �Ԥ����ֲ�� */
	tcb->tskwait  = 0;
	*(tcb->sp)    = (UH)ercd;
	if ( tcb->que != NADR )
		__del_que(tcb);
	
	/* ��ǥ������塼����³ */
	__adt_que(&rdyque[tcb->tskpri - 1], tcb);
	
	/* �������� */
	if ( tcb->tskstat == TTS_WAS ) {
		tcb->tskstat = TTS_SUS;
	}
	else {
		tcb->tskstat = TTS_RDY;
		__tsk_dsp();
	}
}

