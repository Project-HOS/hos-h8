#include "h8_3048.h"
#include "h8_sci.h"


/* ------------------------------ */
/*             SCI                */
/* ------------------------------ */

/* 使用ポートの定義 */
#ifdef __USE_SCI0
#define SMR     SMR0
#define BRR     BRR0
#define SCR     SCR0
#define TDR     TDR0
#define SSR     SSR0
#define RDR     RDR0
#else
#define SMR     SMR1
#define BRR     BRR1
#define SCR     SCR1
#define TDR     TDR1
#define SSR     SSR1
#define RDR     RDR1
#endif


#define SCIBUFSIZ 128			/* バッファサイズ */

static unsigned char sci_rx_buff[SCIBUFSIZ];	/* 受信リングバッファ */
static unsigned char sci_rx_stptr;				/* 受信バッファ先頭 */
static unsigned char sci_rx_enptr;				/* 受信バッファ末尾 */
static unsigned char sci_tx_buff[SCIBUFSIZ];	/* 送信リングバッファ */
static unsigned char sci_tx_stptr;				/* 送信バッファ先頭 */
static unsigned char sci_tx_enptr;				/* 送信バッファ末尾 */


/* SCI初期化 */
void SCI_Init(unsigned char rate)
{
	int i;
	
	/* リングバッファ初期化 */
	sci_rx_stptr = sci_rx_enptr = 0;
	sci_tx_stptr = sci_tx_enptr = 0;
	
	/* SCI初期化 */
	SCR = 0x00;
	SMR = 0x00;
	BRR = rate;
	for ( i = 0; i < 280; i++ )
		;
	SCR = 0x70;
}


/* エラー受信 割り込みハンドラ */
void SCI_ERR_Intr(void)
{
	SSR &= 0xcf;
}


/* データ受信 割り込みハンドラ */
void SCI_RX_Intr(void)
{
	unsigned char c;
	unsigned char nxptr;
	
	/* 1文字受信 */
	SSR &= 0xbf;
	c = RDR;
	
	/* FIFOに1文字入れる */
	nxptr = sci_rx_enptr + 1;
	if(nxptr >= SCIBUFSIZ) nxptr = 0;
	if(sci_rx_stptr != nxptr) {
		sci_rx_buff[sci_rx_enptr] = c;
		sci_rx_enptr = nxptr;
	} else {
		/* バッファが満杯 */
		/* 本来ならここにエラー処理 */
		/* とりあえず取りこぼすことにする */
	}
}


/* 送信割り込み */
void SCI_TX_Intr(void)
{
	unsigned char nxptr;
	
	if ( sci_tx_stptr == sci_tx_enptr ) {
		/* 送信バッファが空なら割り込みを禁止してリターン */
		SCR &= 0x7f;
		return;
	}
	
	/* 送信バッファから1文字取り出して送信 */
	TDR  = sci_tx_buff[sci_tx_stptr];
	SSR &= 0x7f;
	
	/* ポインタ更新 */
	nxptr = sci_tx_stptr + 1;
	if (nxptr >= SCIBUFSIZ)  nxptr = 0;
	sci_tx_stptr = nxptr;
	
	if ( sci_tx_stptr == sci_tx_enptr ) {
		/* バッファが空なら次の割り込みを止める */
		SCR &= 0x7f;
		return;
	}
}


/* １文字出力 */
void SCI_Putc(char data)
{
	unsigned char nxptr;
	
	__di();
	
	if ( (sci_tx_stptr == sci_tx_enptr) && (SSR & 0x80) ) {
		/* バッファが空で送信可能ならすぐ出力 */
		TDR = data;
		SSR &= 0x7f;
		SCR &= 0x7f;
		__ei();
		return;
	}
	
	nxptr = sci_tx_enptr + 1;
	if( nxptr >= SCIBUFSIZ ) nxptr = 0;
	while ( (volatile)sci_tx_stptr == nxptr ) {
		/* バッファが一杯なら空くのを待つ */
		__ei();
		__di();
	}
	
	/* 送信バッファに1文字入れる */
	sci_tx_buff[sci_tx_enptr] = data;
	sci_tx_enptr = nxptr;
	
	/* 割り込みを許可 */
	SCR |= 0x80;
	
	__ei();
}


/* 文字列出力 */
void SCI_Puts(const char *str)
{
	while ( *str )
		SCI_Putc(*str++);
}


/* １文字入力 */
int  SCI_Getc(void)
{
	unsigned char c;
	unsigned char nxptr;

	if (sci_rx_stptr == sci_rx_enptr)
		return -1;	/* 受信バッファが空 */

	/* FIFOから1文字取り出す */
	c = sci_rx_buff[sci_rx_stptr];

	/* ポインタ更新 */
	nxptr = sci_rx_stptr + 1;
	if (nxptr >= SCIBUFSIZ)
		nxptr = 0;
	
	sci_rx_stptr = nxptr;
	
	return c;
}


/* 文字列入力 */
void SCI_Gets(char *buf)
{
	int c;
	
	for ( ; ; ) {
		c = SCI_Getc();
		if ( c != -1 ) {
			*buf++ = c;
			if ( c == '\r' ) {
				*buf = '\0';
				return;
			}
		}
	}
}

