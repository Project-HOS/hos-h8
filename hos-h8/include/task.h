/* ------------------------------------------------------------------------- */
/*  HOS ���������� �إå��ե�����                                            */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#ifndef __HOS__TAKS_H_
#define __HOS__TAKS_H_


/* ------------------------------------ */
/*             ������                 */
/* ------------------------------------ */

/* ������°�� */
#define TA_ASM   0x0000		/* ������֥�ˤ��ץ���� */
#define TA_HLNG  0x0001		/* ������ˤ��ץ���� */
#define TA_COP0  0x8000
#define TA_COP1  0x4000
#define TA_COP2  0x2000
#define TA_COP3  0x1000
#define TA_COP4  0x0800
#define TA_COP5  0x0400
#define TA_COP6  0x0200
#define TA_COP7  0x0100

/* �����Ȥʤ�ѥ�᡼���� */
#define TSK_SELF      0		/* ��ʬ���ȤΥ����� */
#define TPRI_INI      0		/* ���ͥ���٤���� */
#define TPRI_RUN      0		/* ��ʬ���Ȥ�ͥ���� */

/* ������������� */
#define TTS_RUN  0x01		/* �¹Ծ��� */
#define TTS_RDY  0x02		/* �¹Բ�ǽ���� */
#define TTS_WAI  0x04		/* �Ԥ����� */
#define TTS_SUS  0x08		/* �����Ԥ����� */
#define TTS_WAS  0x0c		/* ����Ԥ����� */
#define TTS_DMT  0x10		/* �ٻ߾��� */

/* �Ԥ����� */
#define TTW_SLP  0x0001		/* slp_tsk, tslp_tsk �ˤ���Ԥ� */
#define TTW_DLY  0x0002		/* dly_tsk �ˤ���Ԥ� */
#define TTW_NOD  0x0008		/* ��³��ǽ�����Ԥ� */
#define TTW_FLG  0x0010		/* wai_flg, twai_flg �ˤ���Ԥ� */
#define TTW_SEM  0x0020		/* wai_sem, twai_sem �ˤ���Ԥ� */
#define TTW_MBX  0x0040		/* rcv_msg, trcv_msg �ˤ���Ԥ� */
#define TTW_SMBF 0x0080		/* snd_mbf, tsnd_mbf �ˤ���Ԥ� */
#define TTW_MBF  0x0100		/* rcv_mbf, trcv_mbf �ˤ���Ԥ� */
#define TTW_CAL  0x0200		/* ���ǥָƽФ��Ԥ� */
#define TTW_ACP  0x0400		/* ���ǥּ��դ��Ԥ� */
#define TTW_RDV  0x0800		/* ���ǥֽ�λ�Ԥ� */
#define TTW_MPL  0x1000		/* get_blk, tget_blk �ˤ���Ԥ� */
#define TTW_MPF  0x2000		/* get_blf, tget_blf �ˤ���Ԥ� */
#define TTW_SPL  0x8000		/* �����å��ס����Ԥ����֡�HOS�ȼ��� */


/* TCB��Ϣ����� */
#define MAX_SUSCNT  65535U		/* SUSPEND ����ͥ��ȿ� */
#define MAX_WUPCNT  65535U		/* �����׵� ���祭�塼���󥰿� */



/* ------------------------------------ */
/*              �����                  */
/* ------------------------------------ */

/* �����������ѥ��塼 */
typedef struct t_que {
	struct t_tcb *head;		/* ���塼����Ƭ */
	INT    tskcnt;			/* ���塼���¤�Ǥ��륿�����ο� */
} T_QUE;

/* TCB(����������ȥ���֥�å�) ưŪ��ʬ RAM���֤� */
/* �ʴ��ǲ��Τ��ᥢ����֥�������������Τ���Ƭ���֤��� */
typedef struct t_tcb {
	VH   *sp;			/* ���ߤΥ����å��ݥ��� */
	UINT tskstat;		/* �������ξ��� */
	PRI  tskpri;		/* ���ߤ�ͥ���� */
	UINT tskwait;		/* �Ԥ����� */
	UH   wupcnt;		/* �����׵ᥭ�塼���󥰿� */
	UH   suscnt;		/* SUSPEND�׵�ͥ��ȿ� */
	VP   data;			/* ���������¸��ɬ�פʥǡ���������лȤ� */
	struct t_que *que;	/* °���Ƥ��륭�塼  */
	struct t_tcb *next;	/* ���塼�Ǥμ���TCB */
	struct t_tcb *prev;	/* ���塼�Ǥ�����TCB */
} T_TCB;

/* TCB��Ū��ʬ ROM���֤� */
typedef struct t_tcbs {
	VP   exinf;			/* ��ĥ���� */
	FP   task;			/* ��������ư���ɥ쥹 */
	PRI  itskpri;		/* ��������ư��ͥ���� */
	VH   *isp;			/* ��������ư���Υ����å��ݥ��󥿤���*/
} T_TCBS;

/* ���������ֻ����� */
typedef struct t_rtsk {
	VP   exinf;			/* ��ĥ���� */
	PRI  tskpri;		/* ���ߤ�ͥ���� */
	UB   tskstat;		/* ���������� */
} T_RTSK;


/* �����å��ס��륳��ȥ���֥�å� RAM��ʬ */
typedef struct t_spcb {
	T_QUE que;		/* �����å��Ԥ����塼 */
	UH    freestk;	/* �����Ƥ��륹���å��ΰ��� */
} T_SPCB;

/* �����å��ס��륳��ȥ���֥�å� ROM��ʬ */
typedef struct t_spcbs {
	VH    *stkhead;	/* ����ס�����Ƭ���ɥ쥹 */
	UH    stkcnt;	/* �֥�å��� */
	UH    stksz;	/* �֥�å������� */
} T_SPCBS;



/* ------------------------------------ */
/*          �����Х��ѿ�              */
/* ------------------------------------ */

extern       T_TCB  tcbtbl[];	/* TCB�ơ��֥� RAM�� */
extern const T_TCBS tcbstbl[];	/* TCB�ơ��֥� ROM������Ū������) */
extern const H      tcbcnt;		/* TCB�ơ��֥�ο� */

extern       T_QUE  rdyque[];	/* ��ǥ������塼 */
extern const H      rdqcnt;		/* ��ǥ������塼�ο� */

extern T_TCB  *curtcb;			/* ���ߤΥ�������TCB */
extern BOOL   dlydsp;			/* �ٱ䤵��Ƥ���ǥ����ѥå������뤫 */

extern       T_SPCB  spcbtbl[];		/* SPCB�ơ��֥� RAM�� */
extern const T_SPCBS spcbstbl[];	/* SPCB�ơ��֥� ROM������Ū������) */
extern const H       spcbcnt;		/* SPCB�ơ��֥�ο� */


/* ------------------------------------ */
/*             �ؿ����                 */
/* ------------------------------------ */

/* ���塼���� */
void __adt_que(T_QUE *que, T_TCB *tcb);		/* �������򥭥塼�������ɲ� */
void __adh_que(T_QUE *que, T_TCB *tcb);		/* �������򥭥塼��Ƭ���ɲ� */
void __ins_que(T_TCB *tcbs, T_TCB *tcbd);	/* ���塼�˥����������� */
void __del_que(T_TCB *tcb);					/* �������򥭥塼���鳰�� */
void __rot_que(T_QUE *que);					/* ���塼�β�ž */

/* ���������� */
void __ini_tsk(void);						/* ���������ν���� */
ER   sta_tsk(ID tskid, INT stacd);			/* ��������ư */
void ext_tsk(void);							/* ����������λ */
ER   ter_tsk(ID tskid);						/* ¾������������λ */
ER   dis_dsp(void);							/* �ǥ����ѥå��ػ� */
ER   ena_dsp(void);							/* �ǥ����ѥå����� */
ER   chg_pri(ID tskid, PRI tskpri);			/* ������ͥ�����ѹ� */
ER   rot_rdq(PRI tskpri);					/* �������Υ�ǥ������塼��ž */
ER   rel_wai(ID tskid);						/* ¾���������Ԥ����ֲ�� */
ER   get_tid(ID *p_tskid);					/* ����������ID���� */
ER   ref_tsk(T_RTSK *pk_rtsk, ID tskid);	/* ���������ֻ��� */

/* ��������Ω������� */
#define ista_tsk  sta_tsk
#define iter_tsk  ter_tsk
#define ichg_pri  chg_pri
#define irot_rdq  rot_rdq
#define irel_wai  rel_wai
#define iget_tid  get_tid
#define iref_tsk  ref_tsk


/* ��������°Ʊ�� */
ER   sus_tsk(ID tskid);
ER   rsm_tsk(ID tskid);
ER   frsm_tsk(ID tskid);
ER   slp_tsk(void);
ER   wup_tsk(ID tskid);
ER   can_wup(ID tskid);

/* ��������Ω������� */
#define isus_tsk  sus_tsk
#define irsm_tsk  rsm_tsk
#define ifrsm_tsk frsm_tsk
#define iwup_tsk  wup_tsk
#define ican_wup  can_wup


/* �ǥ����ѥå����� */
ER   __tsk_dsp(void);					/* �������ǥ����ѥå� */
void __wup_dsp(T_TCB *tcb, ER ercd);	/* �Ԥ����ֲ�� */
H    __swc_dsp(T_TCB *tcb);				/* ���������ڤ��ؤ� */
void __sta_dsp(T_TCB *tcb);				/* �������γ��ϡʥ꥿���󤷤ʤ�) */
H    __end_dsp(void);					/* �������ν�λ */
void __tskst_entry(void);				/* �������¹Գ��Ͼ�� */
void __tsk_hlt(void);					/* �¹ԥ������ʤ� */


/* ���������� */
void __di(void);	/* �����߶ػ� */
void __ei(void);	/* �����ߵ��� */
void __set_imsk(void);  	/* �����߶ػ� */
void __res_imsk(void);		/* loc_cpu���Ƥ��ʤ���г��������� */

#endif /* __HOS__TAKS_H_ */
