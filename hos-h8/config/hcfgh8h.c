/* ------------------------------------------------------------------------- */
/*  HOS-H8h �� ����ե�����졼���� Ver 1.00                                 */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ------------------------------------ */
/*             ������                 */
/* ------------------------------------ */

#define TRUE  1
#define FALSE 0

#define MAX_PATH   		256		/* ����ѥ�̾ */
#define MAX_PARAMETER     6     /* ����ѥ�᡼������ */
#define MAX_ID          255     /* ����ɣĿ� */
#define MAX_INT          64		/* ����������ֹ� */
#define MAX_INCLUDE_C    64		/* �����ɤ߹��ߥե������(�ø�����) */



/* ------------------------------------ */
/*             �ؿ����                 */
/* ------------------------------------ */

/* ���ޥ�ɲ��� */
int  AnalizeCommand(char *(*pppPara[MAX_PARAMETER]), int nPara, char *pBuf);
void AnalizeDefInt(char *pBuf);
void AnalizeIncludeC(char *pBuf);
void AnalizeMaxTskPri(char *pBuf);
void AnalizeSysStack(char *pBuf);

/* ����γ��� */
void FreeParameter(char *ppPara[MAX_PARAMETER]);
void FreeTable(char ***pppTable);

/* ʸ������� */
void CutSpace(char *pBuf);
int  ClipParameter(char *ppPar[], int nNum, char *pBuf);
int  StrToInt(int *pNum, char *pBuf);
int  StrToLong(long *pNum, char *pBuf);

/* �ե�������� */
void Write_Def_ID(void);		/* ����ե�������� */
void Write_C_Cfg(void);			/* �ø��������� */
void Write_Asm_Cfg(void);		/* ������֥���������� */



/* ------------------------------------ */
/*             �ѿ����                 */
/* ------------------------------------ */

char szCfgFile[MAX_PATH]  = "hos.cfg";		/* ����ե����졼�����ե����� */
char szAsmFile[MAX_PATH]  = "cfg_asm.src";	/* ������֥������ϥե�����̾ */
char szCFile[MAX_PATH]    = "cfg_c.c";		/* �ø�����ϥե�����̾ */
char szDefCFile[MAX_PATH] = "defid.h";		/* �������إå��ե�����̾ */

int  nLine;			/* ������ι��ֹ� */
int  bErr = FALSE;	/* ���顼 */

char *pMaxTskPri = NULL;		/* ����ͥ����(�ǥե���Ȥǣ�) */

/* �ƥѥ�᡼���� ���Ǽ */
char **pppTaskTable[MAX_ID];	/* �����������ǡ����ơ��֥� */
char **pppSplTable[MAX_ID];		/* �����å��ס��������ǡ����ơ��֥� */
char **pppSemTable[MAX_ID];		/* ���ޥե������ǡ����ơ��֥� */
char **pppFlgTable[MAX_ID];		/* ���٥�ȥե饰�����ǡ����ơ��֥� */
char **pppMbxTable[MAX_ID];		/* ��å������ܥå��������ǡ����ơ��֥� */
char **pppMpfTable[MAX_ID];		/* ����Ĺ����ס��������ǡ����ơ��֥� */
char *ppIntTable[MAX_INT];		/* �����ߥ٥����Υơ��֥� */
int  nTaskCount;	/* �������ο� */
int  nSplCount;		/* �����å��ס���ο� */
int  nSemCount;		/* ���ޥե��ο� */
int  nFlgCount;		/* ���٥�ȥե饰�ο� */
int  nMbxCount;		/* �ᥤ��ܥå����� */
int  nMpfCount;		/* ����Ĺ����ס���ο� */

char *ppIncludeCTable[MAX_INCLUDE_C];	/* �ø���Υ��󥯥롼�ɥե����� */
int  nIncCCount;		/* �ø��쥤�󥯥롼�ɥե������ */

unsigned long SystemStack = 0xfff10;	/* �����ƥॹ���å��Υ��ɥ쥹 */

int  bUseTimer = TRUE;		/* �����ޤ�Ȥ����ɤ��� */



/* ------------------------------------ */
/*             �ᥤ��                   */
/* ------------------------------------ */

/* �ᥤ��ؿ� */
int main(int argc, char *argv[])
{
	FILE *fp;
	char szBuf[256];
	int  i;
	
	/* ��ɽ�� */
	fprintf(stderr, "HOS-H8h Configrater Ver1.01 by Ryuz\n");
	
	/* ���ޥ�ɥ饤����� */
	if ( argc == 2 )
		strcpy(szCfgFile, argv[1]);
	
	
	/* �ե����륪���ץ� */
	if ( (fp = fopen(szCfgFile, "r")) == NULL ) {
		fprintf(stderr, "%s �������ޤ���\n", szCfgFile);
		exit(1);
	}
	
	/* �ե�����β��� */
	nLine = 1;
	while ( fgets(szBuf, sizeof(szBuf), fp) != NULL ) {
		/* ;ʬ�ʶ������ */
		CutSpace(szBuf);
		
		/* ���ޥ��̾�˱����Ʋ��� */
		if ( strncmp(szBuf, "CRE_TSKSP", 9) == 0 ) {
			int nID;
			nID = AnalizeCommand(pppTaskTable, 5, &szBuf[9]);
			if ( nID != -1 )
				pppTaskTable[nID][5] = malloc(1);	/* ������� (^^;; */
		}
		else if ( strncmp(szBuf, "CRE_TSK", 7) == 0 )
			AnalizeCommand(pppTaskTable, 5, &szBuf[7]);
		else if ( strncmp(szBuf, "CRE_SPL", 7) == 0 )
			AnalizeCommand(pppSplTable, 3, &szBuf[7]);
		else if ( strncmp(szBuf, "CRE_SEM", 7) == 0 )
			AnalizeCommand(pppSemTable, 4, &szBuf[7]);
		else if ( strncmp(szBuf, "CRE_FLG", 7) == 0 )
			AnalizeCommand(pppFlgTable, 3, &szBuf[7]);
		else if ( strncmp(szBuf, "CRE_MBX", 7) == 0 )
			AnalizeCommand(pppMbxTable, 3, &szBuf[7]);
		else if ( strncmp(szBuf, "CRE_MPF", 7) == 0 )
			AnalizeCommand(pppMpfTable, 4, &szBuf[7]);
		else if ( strncmp(szBuf, "DEF_INT", 7) == 0 )
			AnalizeDefInt(&szBuf[7]);
		else if ( strncmp(szBuf, "INCLUDE_C", 9) == 0 )
			AnalizeIncludeC(&szBuf[9]);
		else if ( strncmp(szBuf, "MAX_TSKPRI", 10) == 0 )
			AnalizeMaxTskPri(&szBuf[10]);
		else if ( strncmp(szBuf, "SYSTEM_STACK", 12) == 0 )
			AnalizeSysStack(&szBuf[12]);
		else if ( strncmp(szBuf, "USE_TIMER", 9) == 0 )
			bUseTimer = TRUE;
		else if ( strncmp(szBuf, "NOUSE_TIMER", 11) == 0 )
			bUseTimer = FALSE;
		else if ( szBuf[0] != ';' && szBuf[0] != '\0' && szBuf[0] != '\n' ) {
			fprintf(stdout, "line(%d): ʸˡ���顼�Ǥ�\n", nLine);
			bErr = TRUE;
		}
		nLine++;
	}
	
	fclose(fp);
	
	/* �ե�������� */
	Write_Def_ID();
	Write_C_Cfg();
	Write_Asm_Cfg();
	
	/* ���곫�� */
	FreeTable(pppTaskTable);
	FreeTable(pppSemTable);
	FreeTable(pppFlgTable);
	FreeTable(pppMbxTable);
	FreeTable(pppMpfTable);
	if ( pMaxTskPri != NULL )  free(pMaxTskPri);
	for ( i = 0; i < MAX_INT; i++ )
		if ( ppIntTable[i] != NULL )  free(ppIntTable[i]);
	for ( i = 0; i < MAX_INCLUDE_C; i++ )
		if ( ppIncludeCTable[i] != NULL )  free(ppIncludeCTable[i]);
	
	return bErr;
}



/* ------------------------------------ */
/*         �ե��������                 */
/* ------------------------------------ */

/* ����ե�������� */
void Write_Def_ID(void)
{
	FILE *fp;
	int  i;
	
	/* �ե����륪���ץ� */
	if ( (fp = fopen(szDefCFile, "w")) == NULL ) {
		fprintf(stderr, "%s �κ����˼��Ԥ��ޤ���\n", szDefCFile);
		exit(1);
	}
	
	fprintf(fp, "/* HOS-H8h configuration-file to define ID */\n\n");
	fprintf(fp, "#ifndef __HOSCFG__DEFID_H_\n");
	fprintf(fp, "#define __HOSCFG__DEFID_H_\n\n\n");
	
	/* ������ID ���� */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppTaskTable[i] != NULL && pppTaskTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppTaskTable[i][0], i + 1);
	}
	fprintf(fp, "\n");
	
	/* �����å��ס���ID ���� */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppSplTable[i] != NULL && pppSplTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppSplTable[i][0], i + 1);
	}
	fprintf(fp, "\n");
	
	/* ���ޥե�ID ���� */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppSemTable[i] != NULL && pppSemTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppSemTable[i][0], i + 1);
	}
	fprintf(fp, "\n");
	
	/* ���٥�ȥե饰ID ���� */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppFlgTable[i] != NULL && pppFlgTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppFlgTable[i][0], i + 1);
	}
	fprintf(fp, "\n");
	
	/* �᡼��ܥå���ID ���� */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppMbxTable[i] != NULL && pppMbxTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppMbxTable[i][0], i + 1);
	}
	fprintf(fp, "\n");

	/* ����Ĺ����ס���ID ���� */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppMpfTable[i] != NULL && pppMpfTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppMpfTable[i][0], i + 1);
	}
	fprintf(fp, "\n");

	fprintf(fp, "\n#endif /* __HOSCFG__DEFID_H_ */\n");
}


/* ����ե�����졼�����ե�����ø��������� */
void Write_C_Cfg(void)
{
	FILE *fp;
	char *pPri;
	int  nMaxID;
	int  i;
	
	/* �ե����륪���ץ� */
	if ( (fp = fopen(szCFile, "w")) == NULL ) {
		fprintf(stderr, "%s �κ����˼��Ԥ��ޤ���\n", szDefCFile);
		exit(1);
	}
	
	/* �����Ƚ��� */
	fprintf(fp, "/* HOS-H8�� ����ե�����졼�����ե�����ø����� */\n\n");
	
	/* ���󥯥롼�ɥե�������� */
	fprintf(fp, "#include <itron.h>\n");
	fprintf(fp, "#include \"%s\"\n", szDefCFile);
	for ( i = 0; i < nIncCCount; i++ )
		fprintf(fp, "#include %s\n", ppIncludeCTable[i]);
	
	
	/* ------- �����ƥॹ���å����ɥ쥹 -------- */
	fprintf(fp, "\n\n/* �����ƥॹ���å�������ɥ쥹 */\n");
	fprintf(fp, "void *const __system_stack = (void *)(0x%05lx);\n",
					SystemStack);
	
	
	
	/* ------- ��ǥ������塼���� -------- */
	fprintf(fp, "\n");
	if ( pMaxTskPri == NULL )
		pPri = "8";
	else
		pPri = pMaxTskPri;
	fprintf(fp, "\n\n/* ��ǥ������塼 */\n");
	fprintf(fp, "T_QUE   rdyque[%s];\n", pPri);
	fprintf(fp, "const H rdqcnt = %s;\n", pPri);
	
	
	/* ------- �����å��ס���ǡ������� -------- */
	fprintf(fp, "\n");
	
	/* ���祹���å��ס���ID���� */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppSplTable[i] != NULL ) {
			nSplCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		/* �����å��ΰ���� */
		fprintf(fp, "\n\n/* �����å��ס����ѥ����å��ΰ� */\n");
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppSplTable[i] != NULL ) {
				fprintf(fp, "VH StackPool%d[(%s) / 2];\n",
									i + 1, pppSplTable[i][2]);
			}
		}
		
		/* �����å��ס������ */
		fprintf(fp, "\n\n/* �����å��ס�����Ͽ */\n");
		fprintf(fp, "T_SPCB  spcbtbl[%d];\n", nMaxID);
		fprintf(fp, "const T_SPCBS spcbstbl[%d] = {\n", nMaxID);
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppSplTable[i] == NULL )
				fprintf(fp, "\t\t{}");
			else
				fprintf(fp, "\t\t{StackPool%d, %s, (%s) / 2}",
							i + 1,
							pppSplTable[i][1],
							pppSplTable[i][2]
						);
			if ( i == nMaxID - 1 )
				fprintf(fp, "\n");
			else
				fprintf(fp, ",\n");
		}
		fprintf(fp, "\t};\n");
	}
	fprintf(fp, "const H  spcbcnt = %d;\t/* �����å��ס���� */\n", nMaxID);
	
	
	
	/* ------- �������ǡ������� -------- */
	fprintf(fp, "\n");
	
	/* ���祿����ID���� */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppTaskTable[i] != NULL ) {
			nTaskCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		/* �������ѥ����å��ΰ���� */
		fprintf(fp, "\n\n/* �����å��ΰ� */\n");
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppTaskTable[i] != NULL && pppTaskTable[i][5] == NULL ) {
				fprintf(fp, "VH Stack%d[(%s) / 2];\n",
								i + 1, pppTaskTable[i][4]);
			}
		}
		
		/* ��������� */
		fprintf(fp, "\n/* ��������Ͽ */\n");
		fprintf(fp, "T_TCB  tcbtbl[%d];\n", nMaxID);
		fprintf(fp, "const T_TCBS tcbstbl[%d] = {\n", nMaxID);
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppTaskTable[i] == NULL )
				fprintf(fp, "\t\t{}");
			else {
				if ( pppTaskTable[i][5] == NULL ) {
					/* �����ǥ����å������ */
					fprintf(fp, "\t\t"
								"{%s, %s, %s, &Stack%d[(%s) / 2]}",
								pppTaskTable[i][1],
								pppTaskTable[i][2],
								pppTaskTable[i][3],
								i + 1,
								pppTaskTable[i][4]
							);
				}
				else {
					/* �����å��ס����Ȥ� */
					fprintf(fp, "\t\t"
								"{%s, %s, %s, (VH*)(((%s) - 1) * 2 + 1)}",
								pppTaskTable[i][1],
								pppTaskTable[i][2],
								pppTaskTable[i][3],
								pppTaskTable[i][4]
							);
				}
			}
			if ( i == nMaxID - 1 )
				fprintf(fp, "\n");
			else
				fprintf(fp, ",\n");
		}
		fprintf(fp, "\t};\n");
	}
	fprintf(fp, "const H  tcbcnt = %d;\t/* �������� */\n", nMaxID);
	
	
	/* ------- ���ޥե��ǡ������� -------- */
	fprintf(fp, "\n");
	
	/* ���祻�ޥե�ID���� */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppSemTable[i] != NULL ) {
			nSemCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		/* ���ޥե���� */
		fprintf(fp, "\n\n/* ���ޥե���Ͽ */\n");
		fprintf(fp, "T_SCB  scbtbl[%d];\n", nMaxID);
		fprintf(fp, "const T_SCBS scbstbl[%d] = {\n", nMaxID);
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppSemTable[i] == NULL )
				fprintf(fp, "\t\t{}");
			else
				fprintf(fp, "\t\t{%s, %s, %s}",
							pppSemTable[i][1],
							pppSemTable[i][2],
							pppSemTable[i][3]
						);
			if ( i == nMaxID - 1 )
				fprintf(fp, "\n");
			else
				fprintf(fp, ",\n");
		}
		fprintf(fp, "\t};\n");
	}
	fprintf(fp, "const H  scbcnt = %d;\t/* ���ޥե��� */\n", nMaxID);


	/* ------- �ե饰�ǡ������� -------- */
	fprintf(fp, "\n");
	
	/* ���祤�٥�ȥե饰ID���� */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppFlgTable[i] != NULL ) {
			nFlgCount++;
			nMaxID = i + 1;
		}
	}

	if ( nMaxID > 0 ) {
		/* ���٥�ȥե饰��� */
		fprintf(fp, "\n\n/* ���٥�ȥե饰��Ͽ */\n");
		fprintf(fp, "T_FCB  fcbtbl[%d];\n", nMaxID);
		fprintf(fp, "const T_FCBS fcbstbl[%d] = {\n", nMaxID);
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppFlgTable[i] == NULL )
				fprintf(fp, "\t\t{}");
			else
				fprintf(fp, "\t\t{%s, %s}",
							pppFlgTable[i][1],
							pppFlgTable[i][2]);
			if ( i == nMaxID - 1 )
				fprintf(fp, "\n");
			else
				fprintf(fp, ",\n");
		}
		fprintf(fp, "\t};\n");
	}
	fprintf(fp, "const H  fcbcnt = %d;\t/* ���٥�ȥե饰�� */\n", nMaxID);

	/* ------- �ᥤ��ܥå����ǡ������� -------- */
	fprintf(fp, "\n");

	/* ����᡼��ܥå���ID���� */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppMbxTable[i] != NULL ) {
			nMbxCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		fprintf(fp, "\n\n/* �᡼��ܥå����ѥХåե� */\n");
		for ( i = 0; i < nMaxID; i++ )
			fprintf(fp, "VP msgbuf%d[%s];\n", i, pppMbxTable[i][2]);
		
		/* �᡼��ܥå������ */
		fprintf(fp, "\n/* �᡼��ܥå�����Ͽ */\n");
		fprintf(fp, "T_MCB  mcbtbl[%d];\n", nMaxID);
		fprintf(fp, "const T_MCBS mcbstbl[%d] = {\n", nMaxID);
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppMbxTable[i] == NULL )
				fprintf(fp, "\t\t{}");
			else
				fprintf(fp, "\t\t{%s, msgbuf%d, msgbuf%d + %s}",
							pppMbxTable[i][1], i, i, pppMbxTable[i][2]);
			if ( i == nMaxID - 1 )
				fprintf(fp, "\n");
			else
				fprintf(fp, ",\n");
		}
		fprintf(fp, "\t};\n");
	}
	fprintf(fp, "const H  mcbcnt = %d;\t/* �᡼��ܥå����� */\n", nMaxID);
	
	
	/* ------- ����Ĺ����ס���ǡ������� -------- */
	fprintf(fp, "\n");
	
	/* �������Ĺ����ס���ID���� */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppMpfTable[i] != NULL ) {
			nMpfCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		fprintf(fp, "\n\n/* ����Ĺ����ס����ѥ��� */\n");
		for ( i = 0; i < nMaxID; i++ )
			fprintf(fp, "UB mpfblk%d[(%s) * (%s)];\n", i,
						pppMpfTable[i][2], pppMpfTable[i][3]);
		
		/* ����Ĺ����ס������ */
		fprintf(fp, "\n/* ����Ĺ����ס�����Ͽ */\n");
		fprintf(fp, "T_FMCB  fmcbtbl[%d];\n", nMaxID);
		fprintf(fp, "const T_FMCBS fmcbstbl[%d] = {\n", nMaxID);
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppMpfTable[i] == NULL )
				fprintf(fp, "\t\t{}");
			else
				fprintf(fp, "\t\t{%s, mpfblk%d, %s, %s}",
							pppMpfTable[i][1], i,
							pppMpfTable[i][2], pppMpfTable[i][3]);
			if ( i == nMaxID - 1 )
				fprintf(fp, "\n");
			else
				fprintf(fp, ",\n");
		}
		fprintf(fp, "\t};\n");
	}
	fprintf(fp, "const H  fmcbcnt = %d;\t/* ����Ĺ����ס���� */\n", nMaxID);	
	
	/* ������롼������� */
	fprintf(fp, "\n\n/* ����� */\n"
				"void __initialize(void)\n{\n"
			);
	if ( bUseTimer )
		fprintf(fp, "\t__ini_tim();\t/* �����޽���� */\n");
	if ( nTaskCount > 0 )
		fprintf(fp, "\t__ini_tsk();\t/* ����������� */\n");
	if ( nSemCount > 0 )
		fprintf(fp, "\t__ini_sem();\t/* ���ޥե������ */\n");
	if ( nFlgCount > 0 )
		fprintf(fp, "\t__ini_flg();\t/* ���٥�ȥե饰����� */\n");
	if ( nMbxCount > 0 )
		fprintf(fp, "\t__ini_mbx();\t/* �ᥤ��ܥå�������� */\n");
	if ( nMpfCount > 0 )
		fprintf(fp, "\t__ini_mpf();\t/* ����Ĺ����ס������� */\n");
	fprintf(fp, "}\n");

	fclose(fp);
}


/* ������֥���������� */
void Write_Asm_Cfg(void)
{
	FILE *fp;
	int  i;
	
	/* �ե����륪���ץ� */
	if ( (fp = fopen(szAsmFile, "w")) == NULL ) {
		fprintf(stderr, "%s �κ����˼��Ԥ��ޤ���\n", szAsmFile);
		exit(1);
	}
	
	/* IMPORT������� */
	fprintf(fp, "; ����ե�����졼���� ������֥���\n\n"
				"\t\t.CPU     300HA\n\n"
				"\t\t.IMPORT\t_hos_start\n"
				"\t\t.IMPORT\t_int_default\n"
				"\t\t.IMPORT\t_int_trap\n");
	for ( i = 1; i < MAX_INT; i++ ) {
		if ( ppIntTable[i] != NULL )
			fprintf(fp, "\t\t.IMPORT\t_%s\n", ppIntTable[i]);
	}
	
	/* �٥����ơ��֥���� */
	fprintf(fp, "\n\t\t.SECTION  VECT,DATA,ALIGN=2\n\n"
				"; -----------------------------------------------\n"
				";          �����ߥ٥����ơ��֥�\n"
				"; -----------------------------------------------\n"
				"\t\t.DATA.L\t_hos_start\n");
	for ( i = 1; i < MAX_INT; i++ ) {
		if ( ppIntTable[i] != NULL ) {
			fprintf(fp, "\t\t.DATA.L\tINT%02X\n", i);
		}
		else {
			fprintf(fp, "\t\t.DATA.L\t_int_default\n");
		}
	}
	
	/* �����ߥϥ�ɥ���� */
	fprintf(fp,
				"\n\n\t\t.SECTION  P,CODE,ALIGN=2\n\n"
				"; -----------------------------------------------\n"
				";          �����ߥϥ�ɥ�\n"
				"; -----------------------------------------------\n\n"
			);
	for ( i = 1; i < MAX_INT; i++ ) {
		if ( ppIntTable[i] != NULL ) {
			fprintf(fp, "INT%02X:\t\tPUSH.L\tER1\n"
						"\t\tMOV.L\t#_%s,ER1\n"
						"\t\tjmp\t@_int_trap\n\n", i, ppIntTable[i]);
		}
	}
	
	
	/* �ơ��֥뤬̵����� Warning �к��˥��ɥ쥹�Τ߳�꿶�� */
	if ( nTaskCount == 0 || nSplCount == 0 || nSemCount == 0
			|| nFlgCount == 0 || nMbxCount == 0 || nMpfCount == 0 ) {
		fprintf(fp, "\n\n");
		if ( nSplCount == 0 )
			fprintf(fp,"\t\t.EXPORT\t_spcbtbl\n"
						"\t\t.EXPORT\t_spcbstbl\n");
		if ( nTaskCount == 0 )
			fprintf(fp,"\t\t.EXPORT\t_tcbtbl\n"
						"\t\t.EXPORT\t_tcbstbl\n");
		if ( nSemCount == 0 )
			fprintf(fp,"\t\t.EXPORT\t_scbtbl\n"
						"\t\t.EXPORT\t_scbstbl\n");
		if ( nFlgCount == 0 )
			fprintf(fp,"\t\t.EXPORT\t_fcbtbl\n"
						"\t\t.EXPORT\t_fcbstbl\n");
		if ( nMbxCount == 0 )
			fprintf(fp,"\t\t.EXPORT\t_mcbtbl\n"
						"\t\t.EXPORT\t_mcbstbl\n");
		if ( nMpfCount == 0 )
			fprintf(fp,"\t\t.EXPORT\t_fmcbtbl\n"
						"\t\t.EXPORT\t_fmcbstbl\n");
		
		fprintf(fp,"\n\n\t\t.SECTION  C,DATA,ALIGN=2\n\n");
		
		if ( nSplCount == 0 )
			fprintf(fp, "_spcbtbl:\n"
						"_spcbstbl:\n");
		if ( nTaskCount == 0 )
			fprintf(fp,"_tcbtbl:\n"
						"_tcbstbl:\n");
		if ( nSemCount == 0 )
			fprintf(fp,"_scbtbl:\n"
						"_scbstbl:\n");
		if ( nFlgCount == 0 )
			fprintf(fp,"_fcbtbl:\n"
						"_fcbstbl:\n");
		if ( nMbxCount == 0 )
			fprintf(fp,"_mcbtbl:\n"
						"_mcbstbl:\n");
		if ( nMpfCount == 0 )
			fprintf(fp,"_fmcbtbl:\n"
						"_fmcbstbl:\n");
	}
	
	fprintf(fp, "\n\n\t\t.END\n");
	
	fclose(fp);
}



/* ------------------------------------ */
/*           ���ޥ�ɲ���               */
/* ------------------------------------ */

/* ���ޥ�ɲ��� (CRE_TSK, CRE_SEM, CRE_FLG, CRE_MBX) */
/* �ƥѥ�᡼������ pppTable[id][pram] ��ʸ����Τޤ޳�Ǽ */
int AnalizeCommand(char ***pppTable, int nPara, char *pBuf)
{
	char **ppPar;
	int  nID;
	
	/* ������� */
	ppPar = (char **)calloc(MAX_PARAMETER, sizeof(char *));
	if ( ppPar == NULL ) {
		fprintf(stderr, "���꤬­��ޤ���\n");
		exit(1);
	}
	
	/* �ѥ�᡼����ʬ�� */
	if ( !ClipParameter(ppPar, nPara, pBuf) ) {
		FreeParameter(ppPar);
		return -1;
	}
	
	/* ID�����å� */
	if ( StrToInt(&nID, ppPar[0]) ) {
		nID--;
		if ( nID < 0 || nID >= MAX_ID ) {
			/* ID�ֹ椬���� */
			fprintf(stdout, "line(%d): �����ʣɣ��ֹ�Ǥ�\n", nLine);
			FreeParameter(ppPar);
			bErr = TRUE;
			return -1;
		}
		if ( pppTable[nID] != NULL ) {
			/* ID�ֹ椬��ʣ */
			fprintf(stdout, "line(%d): �ɣ��ֹ椬��ʣ���Ƥ��ޤ�\n", nLine);
			FreeParameter(ppPar);
			bErr = TRUE;
			return -1;
		}
		ppPar[0][0] = '\0';
	}
	else {
		for ( nID = 0; nID < MAX_ID; nID++ ) {
			if ( pppTable[nID] == NULL )
				break;
		}
		if ( nID >= MAX_ID ) {
			/* ���֥������Ȥ�¿������ */
			fprintf(stdout, "line(%d): ���֥������Ȥ�¿�����ޤ�\n", nLine);
			FreeParameter(ppPar);
			bErr = TRUE;
			return -1;
		}
	}
	
	/* ��Ͽ */
	pppTable[nID] = ppPar;
	
	return nID;
}


/* DEF_INT �β��� */
void AnalizeDefInt(char *pBuf)
{
	char *ppPara[2] = {NULL, NULL};
	int  nIntNum;

	/* ���ޥ���ڤ�Ф� */
	if ( !ClipParameter(ppPara, 2, pBuf) ) {
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		if ( ppPara[1] != NULL )  free(ppPara[1]);
		return;
	}

	/* �������ֹ�����å� */
	if ( !StrToInt(&nIntNum, ppPara[0])
			|| nIntNum < 1 || nIntNum >= MAX_INT ) {
		/* �������ֹ椬���� */
		fprintf(stdout, "line(%d): illegal intrrupt-number.\n", nLine);
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		if ( ppPara[1] != NULL )  free(ppPara[1]);
		bErr = TRUE;
		return;
	}
	if ( ppIntTable[nIntNum] != NULL ) {
		/* �������ֹ椬��ʣ */
		fprintf(stdout, "line(%d): �������ֹ椬��ʣ���Ƥ��ޤ�\n", nLine);
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		if ( ppPara[1] != NULL )  free(ppPara[1]);
		bErr = TRUE;
		return;
	}

	/* �����ߥ��å� */
	ppIntTable[nIntNum] = ppPara[1];
	free(ppIntTable[0]);
}


/* INCLUDE_C �β��� */
void AnalizeIncludeC(char *pBuf)
{
	char *ppPara[1] = {NULL};
	
	if ( !ClipParameter(ppPara, 1, pBuf) ) {
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		return;
	}
	
	if ( nIncCCount >= MAX_INCLUDE_C ) {
		/* ���󥯥롼�ɥե����뤬¿������ */
		fprintf(stdout, "line(%d): ���󥯥롼�ɥե����뤬¿�����ޤ�\n",
					nLine);
		free(ppPara[0]);
		bErr = TRUE;
		return;
	}
	
	ppIncludeCTable[nIncCCount++] = ppPara[0];
}


/* MAX_TSKPRI �β��� */
void AnalizeMaxTskPri(char *pBuf)
{
	char *ppPara[1] = {NULL};
	
	if ( !ClipParameter(ppPara, 1, pBuf) ) {
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		return;
	}
	
	pMaxTskPri = ppPara[0];
}


/* SYSTEM_STACK �β��� */
void AnalizeSysStack(char *pBuf)
{
	char *ppPara[1] = {NULL};
	long  nAdr;

	if ( !ClipParameter(ppPara, 1, pBuf) ) {
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		return;
	}
	if ( StrToLong(&nAdr, ppPara[0]) ) {
		SystemStack = (unsigned long)nAdr;
	}
	else {
		/* �ѥ�᡼���������� */
		fprintf(stdout, "line(%d): �ѥ�᡼�����������Ǥ�\n", nLine);
	}
	
	free(ppPara[0]);
}


/* ------------------------------------ */
/*           ���곫��                 */
/* ------------------------------------ */

/* �ѥ�᡼����¸��ʬ�γ��� */
void FreeParameter(char **ppPara)
{
	int i;
	
	for ( i = 0; i < MAX_PARAMETER; i++ ) {
		if ( ppPara[i] != NULL ) {
			free(ppPara[i]);
			ppPara[i] = NULL;
		}
	}
	free(ppPara);
}

/* �ѥ�᡼�����ơ��֥�γ��� */
void FreeTable(char ***pppTable)
{
	int i;

	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppTable[i] != NULL ) {
			FreeParameter(pppTable[i]);
			pppTable[i] = NULL;
		}
	}
}



/* ------------------------------------ */
/*            ʸ�������                */
/* ------------------------------------ */

/* ����ζ���ν��� */
void CutSpace(char *pBuf)
{
	char *p;
	
	/* ���ζ��򥫥å� */
	p = &pBuf[strlen(pBuf) - 1];
	while ( p >= pBuf && (*p == ' ' || *p == '\t' || *p == '\n') )
		p--;
	*(p + 1) = '\0';
	
	/* ��Ⱦ�ζ��򥫥å� */
	p = pBuf;
	while ( *p == ' ' || *p == '\t' )
		p++;
	memmove(pBuf, p, strlen(p) + 1);
}


/* ��̤ǰϤޤ줿�ΰ褫��ѥ�᡼�����ڤ�Ф� */
int ClipParameter(char *ppPar[], int nNum, char *pBuf)
{
	char *pBase;
	int  nParNest = 1;
	int  i;

	/* �Ϥޤ�� '(' �����å� */
	CutSpace(pBuf);
	if ( *pBuf++ != '(' ) {
		/* ʸˡ���顼 */
		fprintf(stdout, "line(%d): ʸˡ���顼\n", nLine);
		bErr = TRUE;
		return FALSE;
	}
	
	/* ����ʬ�� */
	for ( i = 0; i < nNum; i++ ) {
		pBase = pBuf;
		while ( *pBuf ) {
			if ( nParNest == 1 && *pBuf == ',' )
				break;
			if ( *pBuf == '(' )
				nParNest++;
			if ( *pBuf == ')' )
				nParNest--;
			if ( nParNest == 0 )
				break;
			pBuf++;
		}
		if ( (i < nNum - 1 && *pBuf == ',')
				|| (i == nNum - 1 && *pBuf == ')') ) {
			*pBuf++ = '\0';
			CutSpace(pBase);
			ppPar[i] = (char *)calloc(strlen(pBase) + 1, sizeof(char));
			if ( ppPar[i] == NULL ) {
				/* ������­ */
				fprintf(stderr, "���꤬­��ޤ���\n");
				exit(1);
			}
			strcpy(ppPar[i], pBase);
		}
		else {
			/* �ѥ�᡼���������۾� */
			fprintf(stdout, "line(%d): �ѥ�᡼�������۾�Ǥ�\n", nLine);
			bErr = TRUE;
			return FALSE;
		}
	}
	
	return TRUE;
}


/* ʸ�������ͤ��Ѵ�����(16���б�) */
int StrToInt(int *pNum, char *pBuf)
{
	/* ���򥫥å� */
	CutSpace(pBuf);
	
	/* ���ͤ��ɤ��������å� */
	if ( pBuf[0] < '0' || pBuf[0] > '9' )
		return FALSE;
	
	/* �����ʤ��ɤ��������å� */
	if ( pBuf[0] == '0' && (pBuf[1] == 'x' || pBuf[1] == 'X') )
		return (sscanf(&pBuf[2], "%x", pNum) == 1);
	
	return (sscanf(&pBuf[0], "%d", pNum) == 1);
}


/* ʸ�������ͤ��Ѵ�����(16���б�) */
int StrToLong(long *pNum, char *pBuf)
{
	/* ���򥫥å� */
	CutSpace(pBuf);
	
	/* ���ͤ��ɤ��������å� */
	if ( pBuf[0] < '0' || pBuf[0] > '9' )
		return FALSE;
	
	/* �����ʤ��ɤ��������å� */
	if ( pBuf[0] == '0' && (pBuf[1] == 'x' || pBuf[1] == 'X') )
		return (sscanf(&pBuf[2], "%lx", pNum) == 1);
	
	return (sscanf(&pBuf[0], "%ld", pNum) == 1);
}
