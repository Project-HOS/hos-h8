/* ------------------------------------------------------------------------- */
/*  HOS ���ޥե����� �إå��ե�����                                          */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#ifndef __HOS__SEMAPHOR_H_
#define __HOS__SEMAPHOR_H_


/* ------------------------------------ */
/*              �����                  */
/* ------------------------------------ */

/* SCBưŪ��ʬ RAM���֤�*/
typedef struct t_scb {
	T_QUE que;		/* ���ޥե��Ԥ����� */
	UH    semcnt;	/* ���ޥե��Υ����� */
} T_SCB;

/* TCB��Ū��ʬ ROM���֤� */
typedef struct t_scbs {
	VP    exinf;	/* ��ĥ���� */
	UH    isemcnt;	/* ���ޥե��ν���� */
	UH    maxsem;	/* ���ޥե��κ����� */
} T_SCBS;

/* ���ޥե����ֻ����� */
typedef struct t_rsem {
	VP      exinf;	/* ��ĥ���� */
	BOOL_ID wtsk;	/* �Ԥ���������̵ͭ */
	INT     semcnt;	/* ���ߤΥ��ޥե���������� */
} T_RSEM;



/* ------------------------------------ */
/*          �����Х��ѿ�              */
/* ------------------------------------ */

extern       T_SCB  scbtbl[];		/* SCB����RAM�� */
extern const T_SCBS scbstbl[];		/* SCB����ROM�� */
extern const H      scbcnt;			/* SCB����ο� */


/* ------------------------------------ */
/*             �ؿ����                 */
/* ------------------------------------ */

void __ini_sem(void);		/* ���ޥե��ν���� */
ER   sig_sem(ID semid);		/* ���ޥե��ֵ� */
ER   wai_sem(ID semid);		/* ���ޥե����Ԥ� */
ER   preq_sem(ID semid);	/* ���ޥե����Ԥ�(�ݡ����) */
ER   ref_sem(T_RSEM *pk_rsem, ID semid);	/* ���ޥե����ֻ��� */

/* ��������Ω������� */
#define isig_sem  sig_sem
#define ipreq_sem preq_sem
#define iref_sem  ref_sem


#endif /* __ROS__SEMAPHOR_H_ */
