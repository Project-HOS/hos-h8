/* ------------------------------------------------------------------------- */
/*  HOS-H8 サンプルプログラム                                                */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#include "itron.h"		/* HOS-H8用ヘッダファイル */
#include "h8_3048.h"
#include "h8_sci.h"
#include "sample.h"
#include "defid.h"		/* コンフィギュレーターが生成するID定義ファイル */


/* スタートアップ（非タスク部） */
void start_up(void)
{
	int i;
	
	
	/* ITU0 (システムタイマ）初期化 */
	TCR0   = 0x23;
	TSR0  &= 0xfe;
	TIER0  = 1;
	GRA0   = 1999;	/* 1 msec のインターバル（16MHz時） */
	TSTR  |= 0x01;
	
	/* SCI初期化 */
	SCI_Init(SCI_19200);
	SCI_Puts("HOS-H8h Sample Program.\r\r");
	
	/* タスク開始 */
	sta_tsk(TID_SMP1, 1);
	sta_tsk(TID_SMP2, 2);
}


/* タスク１ */
void Task1(INT stcd)
{
	int c;
	
	SCI_Puts("\rTask1 Start.\r");
	
	for ( ; ; ) {
		/* SCIからの受信を待って 1文字ずらして送信 */
		wai_flg(NADR, FID_SCIRECV, 1, TWF_ANDW | TWF_CLR);
		
		while ( (c = SCI_Getc()) != -1 ) {
			wai_sem(SID_SCI);
			SCI_Putc(c + 1);
			sig_sem(SID_SCI);
		}
	}
}


/* タスク２ */
void Task2(INT stcd)
{
	SYSTIME st;
	
	SCI_Puts("\rTask2 Start.\r");
	
	do {
		dly_tsk(1000);	/* １秒の待ち */
		
		/* 現在時刻を表示 */
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


/* タイマ用割り込みハンドラ */
void TimerTrap(void)
{
	TSR0 &= 0xfe;
	
	/* ＯＳのタイマハンドラをコール */
	__timer_handler();
}


/* 受信割り込み用割り込みハンドラ */
void Int_SCI_Recv(void)
{
	/* 文字を受信 */
	SCI_RX_Intr();
	
	
	/* 受信文字があることをイベントフラグで通知 */
	set_flg(FID_SCIRECV, 1);
}
