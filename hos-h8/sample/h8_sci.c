#include "h8_3048.h"
#include "h8_sci.h"


/* ------------------------------ */
/*             SCI                */
/* ------------------------------ */

/* ���ѥݡ��Ȥ���� */
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


#define SCIBUFSIZ 128			/* �Хåե������� */

static unsigned char sci_rx_buff[SCIBUFSIZ];	/* ������󥰥Хåե� */
static unsigned char sci_rx_stptr;				/* �����Хåե���Ƭ */
static unsigned char sci_rx_enptr;				/* �����Хåե����� */
static unsigned char sci_tx_buff[SCIBUFSIZ];	/* ������󥰥Хåե� */
static unsigned char sci_tx_stptr;				/* �����Хåե���Ƭ */
static unsigned char sci_tx_enptr;				/* �����Хåե����� */


/* SCI����� */
void SCI_Init(unsigned char rate)
{
	int i;
	
	/* ��󥰥Хåե������ */
	sci_rx_stptr = sci_rx_enptr = 0;
	sci_tx_stptr = sci_tx_enptr = 0;
	
	/* SCI����� */
	SCR = 0x00;
	SMR = 0x00;
	BRR = rate;
	for ( i = 0; i < 280; i++ )
		;
	SCR = 0x70;
}


/* ���顼���� �����ߥϥ�ɥ� */
void SCI_ERR_Intr(void)
{
	SSR &= 0xcf;
}


/* �ǡ������� �����ߥϥ�ɥ� */
void SCI_RX_Intr(void)
{
	unsigned char c;
	unsigned char nxptr;
	
	/* 1ʸ������ */
	SSR &= 0xbf;
	c = RDR;
	
	/* FIFO��1ʸ������� */
	nxptr = sci_rx_enptr + 1;
	if(nxptr >= SCIBUFSIZ) nxptr = 0;
	if(sci_rx_stptr != nxptr) {
		sci_rx_buff[sci_rx_enptr] = c;
		sci_rx_enptr = nxptr;
	} else {
		/* �Хåե������� */
		/* ����ʤ餳���˥��顼���� */
		/* �Ȥꤢ������ꤳ�ܤ����Ȥˤ��� */
	}
}


/* ���������� */
void SCI_TX_Intr(void)
{
	unsigned char nxptr;
	
	if ( sci_tx_stptr == sci_tx_enptr ) {
		/* �����Хåե������ʤ�����ߤ�ػߤ��ƥ꥿���� */
		SCR &= 0x7f;
		return;
	}
	
	/* �����Хåե�����1ʸ�����Ф������� */
	TDR  = sci_tx_buff[sci_tx_stptr];
	SSR &= 0x7f;
	
	/* �ݥ��󥿹��� */
	nxptr = sci_tx_stptr + 1;
	if (nxptr >= SCIBUFSIZ)  nxptr = 0;
	sci_tx_stptr = nxptr;
	
	if ( sci_tx_stptr == sci_tx_enptr ) {
		/* �Хåե������ʤ鼡�γ����ߤ�ߤ�� */
		SCR &= 0x7f;
		return;
	}
}


/* ��ʸ������ */
void SCI_Putc(char data)
{
	unsigned char nxptr;
	
	__di();
	
	if ( (sci_tx_stptr == sci_tx_enptr) && (SSR & 0x80) ) {
		/* �Хåե�������������ǽ�ʤ餹������ */
		TDR = data;
		SSR &= 0x7f;
		SCR &= 0x7f;
		__ei();
		return;
	}
	
	nxptr = sci_tx_enptr + 1;
	if( nxptr >= SCIBUFSIZ ) nxptr = 0;
	while ( (volatile)sci_tx_stptr == nxptr ) {
		/* �Хåե������դʤ�����Τ��Ԥ� */
		__ei();
		__di();
	}
	
	/* �����Хåե���1ʸ������� */
	sci_tx_buff[sci_tx_enptr] = data;
	sci_tx_enptr = nxptr;
	
	/* �����ߤ���� */
	SCR |= 0x80;
	
	__ei();
}


/* ʸ������� */
void SCI_Puts(const char *str)
{
	while ( *str )
		SCI_Putc(*str++);
}


/* ��ʸ������ */
int  SCI_Getc(void)
{
	unsigned char c;
	unsigned char nxptr;

	if (sci_rx_stptr == sci_rx_enptr)
		return -1;	/* �����Хåե����� */

	/* FIFO����1ʸ�����Ф� */
	c = sci_rx_buff[sci_rx_stptr];

	/* �ݥ��󥿹��� */
	nxptr = sci_rx_stptr + 1;
	if (nxptr >= SCIBUFSIZ)
		nxptr = 0;
	
	sci_rx_stptr = nxptr;
	
	return c;
}


/* ʸ�������� */
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

