/* ------------------------------------------------------------------------- */
/*  HOS �ᥤ��ܥå������� �إå��ե�����                                    */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#ifndef __HOS__MAILBOX_H_
#define __HOS__MAILBOX_H_


/* ------------------------------------ */
/*              �����                  */
/* ------------------------------------ */

typedef VP T_MSG;	/* �إå������Ѥ��ʤ� */

/* MCB(MailBox����ȥ���֥�å�) ưŪ��ʬ RAM���֤� */
typedef struct t_mcb {
	T_QUE que;		/* ��å������Ԥ����塼 */
	VP    *head;	/* �Хåե������Ƭ��å��������� */
	VP    *tail;	/* �Хåե����������å��������� */
} T_MCB;

/* MCB��Ū��ʬ ROM���֤� */
typedef struct t_mcbs {
	VP   exinf;		/* ��ĥ���� */
	VP   *bufhead;	/* ��å������Хåե���Ƭ */
	VP   *buftail;	/* ��å������Хåե����� */
} T_MCBS;

/* �ᥤ��ܥå������ֻ��� */
typedef struct t_rmbx {
	VP      exinf;		/* ��ĥ���� */
	BOOL_ID wtsk;		/* �Ԥ���������̵ͭ */
	VP      pk_msg;		/* ���˼��������ѥ��åȥ��ɥ쥹 */
} T_RMBX;



/* ------------------------------------ */
/*          �����Х��ѿ�              */
/* ------------------------------------ */

/* ��������Ω������� */
extern       T_MCB  mcbtbl[];		/* MCB�ơ��֥� RAM�� */
extern const T_MCBS mcbstbl[];	/* MCB�ơ��֥� ROM������Ū������) */
extern const H      mcbcnt;		/* MCB�ơ��֥�ο� */



/* ------------------------------------ */
/*             �ؿ����                 */
/* ------------------------------------ */

void __ini_mbx(void);		/* �ᥤ��ܥå�������� */
ER   snd_msg(ID mbxid, VP pk_msg);	/* �ᥤ��ܥå��������� */
ER   rcv_msg(VP *pk_msg, ID mbxid);	/* �ᥤ��ܥå���������� */
ER   prcv_msg(VP *pk_msg, ID mbxid);
								/* �ᥤ��ܥå����������(�ݡ����) */
ER   ref_mbx(T_RMBX *pk_rmbx, ID mbxid);	/* �ᥤ��ܥå������ֻ��� */

#define isnd_msg  snd_msg
#define iprcv_msg prcv_msg
#define iref_mbx  ref_mbx


#endif /* __HOS__MAILBOX_H_ */
