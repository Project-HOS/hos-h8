/* ------------------------------------------------------------------------- */
/*  HOS ���ִ��� �إå��ե�����                                              */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#ifndef __HOS__TIMER_H_
#define __HOS__TIMER_H_


/* ------------------------------------ */
/*              �����                  */
/* ------------------------------------ */

typedef long DLYTIME;	/* �Ԥ����������� */

/* ���ִ�����¤��(�ӥå�����ǥ����󥹣���bit) */
typedef struct t_systime {
	H   utime;	/* ���16bit */
	UW  ltime;	/* ����32bit */
} SYSTIME, CYCTIME, ALMTIME;



/* ------------------------------------ */
/*          �����Х��ѿ�              */
/* ------------------------------------ */

extern SYSTIME systim;		/* �����ƥ९��å� */
extern T_QUE   dlyque;		/* �������ٱ�����Ԥ����� */
extern UH      timint;		/* �����ޥ롼���󤬸ƤФ��ֳ� */


/* ------------------------------------ */
/*             �ؿ����                 */
/* ------------------------------------ */

void __ini_tim(void);			/* ������IC����� */
void __timer_handler(void);		/* �����ޥϥ�ɥ� */

ER   set_tim(SYSTIME *pk_tim);	/* �����ƥ९��å����� */
ER   get_tim(SYSTIME *pk_tim);	/* �����ƥ९��å����� */
ER   dly_tsk(DLYTIME dlytim);	/* �������ٱ� */

#define vset_tmi(x)  (timint = (x)) /* �����޴ֳ֤����� */

/* ��������Ω������� */
#define iset_tim  set_tim
#define iget_tim  get_tim


#endif /* __HOS__TIMER_H_ */
