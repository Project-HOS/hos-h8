#ifndef __PROJECT_HOS__H8_SCI_H_
#define __PROJECT_HOS__H8_SCI_H_


/* SCI�̿�®����� (16MHz�ΤȤ�) */
#define SCI_2400   207
#define SCI_4800   103
#define SCI_9600    51
#define SCI_19200   25
#define SCI_31250   15
#define SCI_38400   12
#define SCI_57600    8		/* �� -3.5% ���꤮�ꤤ���뤫�� (^^;; */


/* SCI */
void SCI_Init(unsigned char rate);	/* SCI����� */
void SCI_ERR_Intr(void);			/* ���顼���� �����ߥϥ�ɥ� */
void SCI_RX_Intr(void);				/* �ǡ������� �����ߥϥ�ɥ� */
void SCI_TX_Intr(void);				/* �����Хåե����������� */
void SCI_Putc(char c)			;	/* ��ʸ������ */
int  SCI_Getc(void);				/* ��ʸ������ */
void SCI_Puts(const char* str);		/* ʸ������� */
void SCI_Gets(char* buf);			/* ʸ�������� */


#endif /* __PROJECT_HOS__H8_SCI_H_ */
