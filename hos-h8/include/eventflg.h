/* ------------------------------------------------------------------------- */
/*  HOS ���٥�ȥե饰�إå��ե�����                                         */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#ifndef __HOS__EVENTFLG_H_
#define __HOS__EVENTFLG_H_



/* ------------------------------------ */
/*             ������                 */
/* ------------------------------------ */

#define TWF_ANDW  0x00
#define TWF_ORW   0x02
#define TWF_CLR   0x01

/* ------------------------------------ */
/*              �����                  */
/* ------------------------------------ */

/* FCBưŪ��ʬ RAM���֤� */
typedef struct t_fcb {
	T_QUE que;		/* ���٥�ȥե饰�Ԥ����� */
	UH    flgptn;	/* ���٥�ȥե饰�ξ��� */
} T_FCB;

/* FCB��Ū��ʬ ROM���֤� */
typedef struct t_fcbs {
	VP   exinf;		/* ��ĥ���� */
	UH   iflgptn;	/* ���٥�ȥե饰�ν���� */
} T_FCBS;

/* ���٥�ȥե饰���ֻ����� */
typedef struct t_rflg {
	VP      exinf;
	BOOL_ID wtsk;
	UH      flgptn;
} T_RFLG;

/* ------------------------------------ */
/*          �����Х��ѿ�              */
/* ------------------------------------ */

extern       T_FCB  fcbtbl[];	/* FCB����RAM�� */
extern const T_FCBS fcbstbl[];	/* FCB����ROM�� */
extern const H      fcbcnt;		/* FCB����ο� */


/* ------------------------------------ */
/*             �ؿ����                 */
/* ------------------------------------ */

void __ini_flg(void);		/* ���٥�ȥե饰����� */
ER   set_flg(ID flgid, UB setptn);	/* ���٥�ȥե饰���å� */
ER   clr_flg(ID flgid, UB clrptn);	/* ���٥�ȥե饰�Υ��ꥢ */
ER   wai_flg(UB *p_flgptn, ID flgid, UB waiptn, UB wfmode);
									/* ���٥�ȥե饰�Ԥ� */
ER   pol_flg(UB *p_flgptn, ID flgid, UB waiptn, UB wfmode);
									/* ���٥�ȥե饰�Ԥ�(�ݡ����) */
ER   ref_flg(T_RFLG *pk_rflg, ID flgid);	/* ���٥�ȥե饰���ֻ��� */

/* ��������Ω������� */
#define iset_flg  set_flg
#define iclr_flg  clr_flg
#define ipol_flg  pol_flg
#define iref_flg  ref_flg


#endif /* __HOS__EVENTFLG_H_ */
