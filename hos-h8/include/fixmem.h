/* ------------------------------------------------------------------------- */
/*  HOS ����Ĺ����ס������ �إå��ե�����                                */
/*                                           Copyright (C) 1998-2000 by Ryuz */
/* ------------------------------------------------------------------------- */

#ifndef __HOS__FIXMEM_H_
#define __HOS__FIXMEM_H_


/* ------------------------------------ */
/*              �����                  */
/* ------------------------------------ */

/* Fixed-size MemoryPool Controle Block ưŪ��ʬ RAM���֤�*/
typedef struct t_fmcb {
	T_QUE que;		/* ���ޥե��Ԥ����� */
	VP    freeblk;	/* �����֥�å��Υ��ɥ쥹 */
} T_FMCB;

/* Fixed-size MemoryPool Controle Block ��Ū��ʬ ROM���֤� */
typedef struct t_fmcbs {
	VP    exinf;	/* ��ĥ���� */
	VP    mpfhead;	/* ����ס�����Ƭ���ɥ쥹 */
	UH    mpfcnt;	/* �֥�å��� */
	UH    mpfsz;	/* �֥�å������� */
} T_FMCBS;

/* ����Ĺ����ס�����ֻ����� */
typedef struct t_rmpf {
	VP      exinf;	/* ��ĥ���� */
	BOOL_ID wtsk;	/* �Ԥ���������̵ͭ */
	INT     frbcnt;	/* �����ΰ�Υ֥�å��� */
} T_RMPF;



/* ------------------------------------ */
/*          �����Х��ѿ�              */
/* ------------------------------------ */

extern       T_FMCB  fmcbtbl[];		/* FMCB����RAM�� */
extern const T_FMCBS fmcbstbl[];	/* FMCB����ROM�� */
extern const H       fmcbcnt;		/* FMCB����ο� */


/* ------------------------------------ */
/*             �ؿ����                 */
/* ------------------------------------ */

void __ini_mpf(void);		/* ����Ĺ����ס���ν���� */
ER   get_blf(VP *p_blf, ID mpfid);	/* ����Ĺ����֥�å����� */
ER   pget_blf(VP *p_blf, ID mpfid);	/* ����Ĺ����֥�å�����(�ݡ����) */
ER   rel_blf(ID mpfid, VP blf);		/* ����Ĺ����֥�å��ֵ� */
ER   ref_mpf(T_RMPF *pk_rsem, ID mpfid);	/* ����Ĺ����ס�����ֻ��� */

/* ��������Ω������� */
#define ipget_blf pget_blf
#define irel_blf  rel_blf
#define iref_mpf  ref_mpf


#endif /* __HOS__FIXMEM_H_ */
