/* ------------------------------------------------------------------------- */
/*  HOS-H8 ����ץ�ץ����                                                */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#include "itron.h"		/* HOS-H8�ѥإå��ե����� */
#include "h8_3048.h"
#include "h8_sci.h"
#include "sample.h"
#include "defid.h"		/* ����ե�����졼��������������ID����ե����� */


/* �������ȥ��åס��󥿥������� */
void start_up(void)
{
	int i;
	
	
	/* ITU0 (�����ƥॿ���ޡ˽���� */
	TCR0   = 0x23;
	TSR0  &= 0xfe;
	TIER0  = 1;
	GRA0   = 1999;	/* 1 msec �Υ��󥿡��Х��16MHz���� */
	TSTR  |= 0x01;
	
	/* SCI����� */
	SCI_Init(SCI_19200);
	SCI_Puts("HOS-H8h Sample Program.\r\r");
	
	/* ���������� */
	sta_tsk(TID_SMP1, 1);
	sta_tsk(TID_SMP2, 2);
}


/* �������� */
void Task1(INT stcd)
{
	int c;
	
	SCI_Puts("\rTask1 Start.\r");
	
	for ( ; ; ) {
		/* SCI����μ������Ԥä� 1ʸ�����餷������ */
		wai_flg(NADR, FID_SCIRECV, 1, TWF_ANDW | TWF_CLR);
		
		while ( (c = SCI_Getc()) != -1 ) {
			wai_sem(SID_SCI);
			SCI_Putc(c + 1);
			sig_sem(SID_SCI);
		}
	}
}


/* �������� */
void Task2(INT stcd)
{
	SYSTIME st;
	
	SCI_Puts("\rTask2 Start.\r");
	
	do {
		dly_tsk(1000);	/* ���ä��Ԥ� */
		
		/* ���߻����ɽ�� */
		get_tim(&st);
		wai_sem(SID_SCI);
		SCI_Putc('\r');
		SCI_Putc('0' + st.ltime % 10000 / 1000);
		SCI_Putc('0' + st.ltime % 1000 / 100);
		SCI_Putc('0' + st.ltime % 100 / 10);
		SCI_Putc('0' + st.ltime % 10);
		SCI_Putc('\r');
		sig_sem(SID_SCI);
	} while ( st.ltime <= 10000L );
	
	SCI_Puts("\rTask2 End.\r");
	
	ext_tsk();
}


/* �������ѳ����ߥϥ�ɥ� */
void TimerTrap(void)
{
	TSR0 &= 0xfe;
	
	/* �ϣӤΥ����ޥϥ�ɥ�򥳡��� */
	__timer_handler();
}


/* �����������ѳ����ߥϥ�ɥ� */
void Int_SCI_Recv(void)
{
	/* ʸ������� */
	SCI_RX_Intr();
	
	
	/* ����ʸ�������뤳�Ȥ򥤥٥�ȥե饰������ */
	set_flg(FID_SCIRECV, 1);
}
