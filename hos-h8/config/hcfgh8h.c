/* ------------------------------------------------------------------------- */
/*  HOS-H8h 用 コンフィギュレーター Ver 1.00                                 */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ------------------------------------ */
/*             定数定義                 */
/* ------------------------------------ */

#define TRUE  1
#define FALSE 0

#define MAX_PATH   		256		/* 最大パス名 */
#define MAX_PARAMETER     6     /* 最大パラメーター数 */
#define MAX_ID          255     /* 最大ＩＤ数 */
#define MAX_INT          64		/* 最大割り込み番号 */
#define MAX_INCLUDE_C    64		/* 最大読み込みファイル数(Ｃ言語用) */



/* ------------------------------------ */
/*             関数宣言                 */
/* ------------------------------------ */

/* コマンド解析 */
int  AnalizeCommand(char *(*pppPara[MAX_PARAMETER]), int nPara, char *pBuf);
void AnalizeDefInt(char *pBuf);
void AnalizeIncludeC(char *pBuf);
void AnalizeMaxTskPri(char *pBuf);
void AnalizeSysStack(char *pBuf);

/* メモリの開放 */
void FreeParameter(char *ppPara[MAX_PARAMETER]);
void FreeTable(char ***pppTable);

/* 文字列処理 */
void CutSpace(char *pBuf);
int  ClipParameter(char *ppPar[], int nNum, char *pBuf);
int  StrToInt(int *pNum, char *pBuf);
int  StrToLong(long *pNum, char *pBuf);

/* ファイル出力 */
void Write_Def_ID(void);		/* 定義ファイル出力 */
void Write_C_Cfg(void);			/* Ｃ言語部出力 */
void Write_Asm_Cfg(void);		/* アセンブリ言語部出力 */



/* ------------------------------------ */
/*             変数宣言                 */
/* ------------------------------------ */

char szCfgFile[MAX_PATH]  = "hos.cfg";		/* コンフィグレーションファイル */
char szAsmFile[MAX_PATH]  = "cfg_asm.src";	/* アセンブリ言語出力ファイル名 */
char szCFile[MAX_PATH]    = "cfg_c.c";		/* Ｃ言語出力ファイル名 */
char szDefCFile[MAX_PATH] = "defid.h";		/* 定数定義ヘッダファイル名 */

int  nLine;			/* 解析中の行番号 */
int  bErr = FALSE;	/* エラー */

char *pMaxTskPri = NULL;		/* 最大優先度(デフォルトで８) */

/* 各パラメーター を格納 */
char **pppTaskTable[MAX_ID];	/* タスク生成データテーブル */
char **pppSplTable[MAX_ID];		/* スタックプール生成データテーブル */
char **pppSemTable[MAX_ID];		/* セマフォ生成データテーブル */
char **pppFlgTable[MAX_ID];		/* イベントフラグ生成データテーブル */
char **pppMbxTable[MAX_ID];		/* メッセージボックス生成データテーブル */
char **pppMpfTable[MAX_ID];		/* 固定長メモリプール生成データテーブル */
char *ppIntTable[MAX_INT];		/* 割り込みベクタのテーブル */
int  nTaskCount;	/* タスクの数 */
int  nSplCount;		/* スタックプールの数 */
int  nSemCount;		/* セマフォの数 */
int  nFlgCount;		/* イベントフラグの数 */
int  nMbxCount;		/* メイルボックスの */
int  nMpfCount;		/* 固定長メモリプールの数 */

char *ppIncludeCTable[MAX_INCLUDE_C];	/* Ｃ言語のインクルードファイル */
int  nIncCCount;		/* Ｃ言語インクルードファイル数 */

unsigned long SystemStack = 0xfff10;	/* システムスタックのアドレス */

int  bUseTimer = TRUE;		/* タイマを使うかどうか */



/* ------------------------------------ */
/*             メイン                   */
/* ------------------------------------ */

/* メイン関数 */
int main(int argc, char *argv[])
{
	FILE *fp;
	char szBuf[256];
	int  i;
	
	/* ロゴ表示 */
	fprintf(stderr, "HOS-H8h Configrater Ver1.01 by Ryuz\n");
	
	/* コマンドライン解析 */
	if ( argc == 2 )
		strcpy(szCfgFile, argv[1]);
	
	
	/* ファイルオープン */
	if ( (fp = fopen(szCfgFile, "r")) == NULL ) {
		fprintf(stderr, "%s が開けません\n", szCfgFile);
		exit(1);
	}
	
	/* ファイルの解析 */
	nLine = 1;
	while ( fgets(szBuf, sizeof(szBuf), fp) != NULL ) {
		/* 余分な空白を削除 */
		CutSpace(szBuf);
		
		/* コマンド名に応じて解析 */
		if ( strncmp(szBuf, "CRE_TSKSP", 9) == 0 ) {
			int nID;
			nID = AnalizeCommand(pppTaskTable, 5, &szBuf[9]);
			if ( nID != -1 )
				pppTaskTable[nID][5] = malloc(1);	/* インチキ (^^;; */
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
			fprintf(stdout, "line(%d): 文法エラーです\n", nLine);
			bErr = TRUE;
		}
		nLine++;
	}
	
	fclose(fp);
	
	/* ファイル出力 */
	Write_Def_ID();
	Write_C_Cfg();
	Write_Asm_Cfg();
	
	/* メモリ開放 */
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
/*         ファイル出力                 */
/* ------------------------------------ */

/* 定義ファイル出力 */
void Write_Def_ID(void)
{
	FILE *fp;
	int  i;
	
	/* ファイルオープン */
	if ( (fp = fopen(szDefCFile, "w")) == NULL ) {
		fprintf(stderr, "%s の作成に失敗しました\n", szDefCFile);
		exit(1);
	}
	
	fprintf(fp, "/* HOS-H8h configuration-file to define ID */\n\n");
	fprintf(fp, "#ifndef __HOSCFG__DEFID_H_\n");
	fprintf(fp, "#define __HOSCFG__DEFID_H_\n\n\n");
	
	/* タスクID 出力 */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppTaskTable[i] != NULL && pppTaskTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppTaskTable[i][0], i + 1);
	}
	fprintf(fp, "\n");
	
	/* スタックプールID 出力 */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppSplTable[i] != NULL && pppSplTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppSplTable[i][0], i + 1);
	}
	fprintf(fp, "\n");
	
	/* セマフォID 出力 */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppSemTable[i] != NULL && pppSemTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppSemTable[i][0], i + 1);
	}
	fprintf(fp, "\n");
	
	/* イベントフラグID 出力 */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppFlgTable[i] != NULL && pppFlgTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppFlgTable[i][0], i + 1);
	}
	fprintf(fp, "\n");
	
	/* メールボックスID 出力 */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppMbxTable[i] != NULL && pppMbxTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppMbxTable[i][0], i + 1);
	}
	fprintf(fp, "\n");

	/* 固定長メモリプールID 出力 */
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppMpfTable[i] != NULL && pppMpfTable[i][0][0] != '\0' ) 
			fprintf(fp, "#define %s\t%d\n", pppMpfTable[i][0], i + 1);
	}
	fprintf(fp, "\n");

	fprintf(fp, "\n#endif /* __HOSCFG__DEFID_H_ */\n");
}


/* コンフィギュレーションファイルＣ言語部出力 */
void Write_C_Cfg(void)
{
	FILE *fp;
	char *pPri;
	int  nMaxID;
	int  i;
	
	/* ファイルオープン */
	if ( (fp = fopen(szCFile, "w")) == NULL ) {
		fprintf(stderr, "%s の作成に失敗しました\n", szDefCFile);
		exit(1);
	}
	
	/* コメント出力 */
	fprintf(fp, "/* HOS-H8用 コンフィギュレーションファイルＣ言語部 */\n\n");
	
	/* インクルードファイル出力 */
	fprintf(fp, "#include <itron.h>\n");
	fprintf(fp, "#include \"%s\"\n", szDefCFile);
	for ( i = 0; i < nIncCCount; i++ )
		fprintf(fp, "#include %s\n", ppIncludeCTable[i]);
	
	
	/* ------- システムスタックアドレス -------- */
	fprintf(fp, "\n\n/* システムスタック初期アドレス */\n");
	fprintf(fp, "void *const __system_stack = (void *)(0x%05lx);\n",
					SystemStack);
	
	
	
	/* ------- レディーキュー出力 -------- */
	fprintf(fp, "\n");
	if ( pMaxTskPri == NULL )
		pPri = "8";
	else
		pPri = pMaxTskPri;
	fprintf(fp, "\n\n/* レディーキュー */\n");
	fprintf(fp, "T_QUE   rdyque[%s];\n", pPri);
	fprintf(fp, "const H rdqcnt = %s;\n", pPri);
	
	
	/* ------- スタックプールデータ出力 -------- */
	fprintf(fp, "\n");
	
	/* 最大スタックプールID検索 */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppSplTable[i] != NULL ) {
			nSplCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		/* スタック領域出力 */
		fprintf(fp, "\n\n/* スタックプール用スタック領域 */\n");
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppSplTable[i] != NULL ) {
				fprintf(fp, "VH StackPool%d[(%s) / 2];\n",
									i + 1, pppSplTable[i][2]);
			}
		}
		
		/* スタックプール定義 */
		fprintf(fp, "\n\n/* スタックプール登録 */\n");
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
	fprintf(fp, "const H  spcbcnt = %d;\t/* スタックプール数 */\n", nMaxID);
	
	
	
	/* ------- タスクデータ出力 -------- */
	fprintf(fp, "\n");
	
	/* 最大タスクID検索 */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppTaskTable[i] != NULL ) {
			nTaskCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		/* タスク用スタック領域出力 */
		fprintf(fp, "\n\n/* スタック領域 */\n");
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppTaskTable[i] != NULL && pppTaskTable[i][5] == NULL ) {
				fprintf(fp, "VH Stack%d[(%s) / 2];\n",
								i + 1, pppTaskTable[i][4]);
			}
		}
		
		/* タスク定義 */
		fprintf(fp, "\n/* タスク登録 */\n");
		fprintf(fp, "T_TCB  tcbtbl[%d];\n", nMaxID);
		fprintf(fp, "const T_TCBS tcbstbl[%d] = {\n", nMaxID);
		for ( i = 0; i < nMaxID; i++ ) {
			if ( pppTaskTable[i] == NULL )
				fprintf(fp, "\t\t{}");
			else {
				if ( pppTaskTable[i][5] == NULL ) {
					/* 自前でスタックを持つ */
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
					/* スタックプールを使う */
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
	fprintf(fp, "const H  tcbcnt = %d;\t/* タスク数 */\n", nMaxID);
	
	
	/* ------- セマフォデータ出力 -------- */
	fprintf(fp, "\n");
	
	/* 最大セマフォID検索 */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppSemTable[i] != NULL ) {
			nSemCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		/* セマフォ定義 */
		fprintf(fp, "\n\n/* セマフォ登録 */\n");
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
	fprintf(fp, "const H  scbcnt = %d;\t/* セマフォ数 */\n", nMaxID);


	/* ------- フラグデータ出力 -------- */
	fprintf(fp, "\n");
	
	/* 最大イベントフラグID検索 */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppFlgTable[i] != NULL ) {
			nFlgCount++;
			nMaxID = i + 1;
		}
	}

	if ( nMaxID > 0 ) {
		/* イベントフラグ定義 */
		fprintf(fp, "\n\n/* イベントフラグ登録 */\n");
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
	fprintf(fp, "const H  fcbcnt = %d;\t/* イベントフラグ数 */\n", nMaxID);

	/* ------- メイルボックスデータ出力 -------- */
	fprintf(fp, "\n");

	/* 最大メールボックスID検索 */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppMbxTable[i] != NULL ) {
			nMbxCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		fprintf(fp, "\n\n/* メールボックス用バッファ */\n");
		for ( i = 0; i < nMaxID; i++ )
			fprintf(fp, "VP msgbuf%d[%s];\n", i, pppMbxTable[i][2]);
		
		/* メールボックス定義 */
		fprintf(fp, "\n/* メールボックス登録 */\n");
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
	fprintf(fp, "const H  mcbcnt = %d;\t/* メールボックス数 */\n", nMaxID);
	
	
	/* ------- 固定長メモリプールデータ出力 -------- */
	fprintf(fp, "\n");
	
	/* 最大固定長メモリプールID検索 */
	nMaxID = 0;
	for ( i = 0; i < MAX_ID; i++ ) {
		if ( pppMpfTable[i] != NULL ) {
			nMpfCount++;
			nMaxID = i + 1;
		}
	}
	
	if ( nMaxID > 0 ) {
		fprintf(fp, "\n\n/* 固定長メモリプール用メモリ */\n");
		for ( i = 0; i < nMaxID; i++ )
			fprintf(fp, "UB mpfblk%d[(%s) * (%s)];\n", i,
						pppMpfTable[i][2], pppMpfTable[i][3]);
		
		/* 固定長メモリプール定義 */
		fprintf(fp, "\n/* 固定長メモリプール登録 */\n");
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
	fprintf(fp, "const H  fmcbcnt = %d;\t/* 固定長メモリプール数 */\n", nMaxID);	
	
	/* 初期化ルーチン出力 */
	fprintf(fp, "\n\n/* 初期化 */\n"
				"void __initialize(void)\n{\n"
			);
	if ( bUseTimer )
		fprintf(fp, "\t__ini_tim();\t/* タイマ初期化 */\n");
	if ( nTaskCount > 0 )
		fprintf(fp, "\t__ini_tsk();\t/* タスク初期化 */\n");
	if ( nSemCount > 0 )
		fprintf(fp, "\t__ini_sem();\t/* セマフォ初期化 */\n");
	if ( nFlgCount > 0 )
		fprintf(fp, "\t__ini_flg();\t/* イベントフラグ初期化 */\n");
	if ( nMbxCount > 0 )
		fprintf(fp, "\t__ini_mbx();\t/* メイルボックス初期化 */\n");
	if ( nMpfCount > 0 )
		fprintf(fp, "\t__ini_mpf();\t/* 固定長メモリプール初期化 */\n");
	fprintf(fp, "}\n");

	fclose(fp);
}


/* アセンブリ言語部出力 */
void Write_Asm_Cfg(void)
{
	FILE *fp;
	int  i;
	
	/* ファイルオープン */
	if ( (fp = fopen(szAsmFile, "w")) == NULL ) {
		fprintf(stderr, "%s の作成に失敗しました\n", szAsmFile);
		exit(1);
	}
	
	/* IMPORT宣言出力 */
	fprintf(fp, "; コンフィギュレーター アセンブラ部\n\n"
				"\t\t.CPU     300HA\n\n"
				"\t\t.IMPORT\t_hos_start\n"
				"\t\t.IMPORT\t_int_default\n"
				"\t\t.IMPORT\t_int_trap\n");
	for ( i = 1; i < MAX_INT; i++ ) {
		if ( ppIntTable[i] != NULL )
			fprintf(fp, "\t\t.IMPORT\t_%s\n", ppIntTable[i]);
	}
	
	/* ベクタテーブル出力 */
	fprintf(fp, "\n\t\t.SECTION  VECT,DATA,ALIGN=2\n\n"
				"; -----------------------------------------------\n"
				";          割り込みベクタテーブル\n"
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
	
	/* 割り込みハンドラ出力 */
	fprintf(fp,
				"\n\n\t\t.SECTION  P,CODE,ALIGN=2\n\n"
				"; -----------------------------------------------\n"
				";          割り込みハンドラ\n"
				"; -----------------------------------------------\n\n"
			);
	for ( i = 1; i < MAX_INT; i++ ) {
		if ( ppIntTable[i] != NULL ) {
			fprintf(fp, "INT%02X:\t\tPUSH.L\tER1\n"
						"\t\tMOV.L\t#_%s,ER1\n"
						"\t\tjmp\t@_int_trap\n\n", i, ppIntTable[i]);
		}
	}
	
	
	/* テーブルが無い場合 Warning 対策にアドレスのみ割り振り */
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
/*           コマンド解析               */
/* ------------------------------------ */

/* コマンド解析 (CRE_TSK, CRE_SEM, CRE_FLG, CRE_MBX) */
/* 各パラメーターを pppTable[id][pram] に文字列のまま格納 */
int AnalizeCommand(char ***pppTable, int nPara, char *pBuf)
{
	char **ppPar;
	int  nID;
	
	/* メモリ確保 */
	ppPar = (char **)calloc(MAX_PARAMETER, sizeof(char *));
	if ( ppPar == NULL ) {
		fprintf(stderr, "メモリが足りません\n");
		exit(1);
	}
	
	/* パラメーター分解 */
	if ( !ClipParameter(ppPar, nPara, pBuf) ) {
		FreeParameter(ppPar);
		return -1;
	}
	
	/* IDチェック */
	if ( StrToInt(&nID, ppPar[0]) ) {
		nID--;
		if ( nID < 0 || nID >= MAX_ID ) {
			/* ID番号が不正 */
			fprintf(stdout, "line(%d): 不正なＩＤ番号です\n", nLine);
			FreeParameter(ppPar);
			bErr = TRUE;
			return -1;
		}
		if ( pppTable[nID] != NULL ) {
			/* ID番号が重複 */
			fprintf(stdout, "line(%d): ＩＤ番号が重複しています\n", nLine);
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
			/* オブジェクトが多すぎる */
			fprintf(stdout, "line(%d): オブジェクトが多すぎます\n", nLine);
			FreeParameter(ppPar);
			bErr = TRUE;
			return -1;
		}
	}
	
	/* 登録 */
	pppTable[nID] = ppPar;
	
	return nID;
}


/* DEF_INT の解析 */
void AnalizeDefInt(char *pBuf)
{
	char *ppPara[2] = {NULL, NULL};
	int  nIntNum;

	/* コマンド切り出し */
	if ( !ClipParameter(ppPara, 2, pBuf) ) {
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		if ( ppPara[1] != NULL )  free(ppPara[1]);
		return;
	}

	/* 割り込み番号チェック */
	if ( !StrToInt(&nIntNum, ppPara[0])
			|| nIntNum < 1 || nIntNum >= MAX_INT ) {
		/* 割り込み番号が不正 */
		fprintf(stdout, "line(%d): illegal intrrupt-number.\n", nLine);
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		if ( ppPara[1] != NULL )  free(ppPara[1]);
		bErr = TRUE;
		return;
	}
	if ( ppIntTable[nIntNum] != NULL ) {
		/* 割り込み番号が重複 */
		fprintf(stdout, "line(%d): 割り込み番号が重複しています\n", nLine);
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		if ( ppPara[1] != NULL )  free(ppPara[1]);
		bErr = TRUE;
		return;
	}

	/* 割り込みセット */
	ppIntTable[nIntNum] = ppPara[1];
	free(ppIntTable[0]);
}


/* INCLUDE_C の解析 */
void AnalizeIncludeC(char *pBuf)
{
	char *ppPara[1] = {NULL};
	
	if ( !ClipParameter(ppPara, 1, pBuf) ) {
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		return;
	}
	
	if ( nIncCCount >= MAX_INCLUDE_C ) {
		/* インクルードファイルが多すぎる */
		fprintf(stdout, "line(%d): インクルードファイルが多すぎます\n",
					nLine);
		free(ppPara[0]);
		bErr = TRUE;
		return;
	}
	
	ppIncludeCTable[nIncCCount++] = ppPara[0];
}


/* MAX_TSKPRI の解析 */
void AnalizeMaxTskPri(char *pBuf)
{
	char *ppPara[1] = {NULL};
	
	if ( !ClipParameter(ppPara, 1, pBuf) ) {
		if ( ppPara[0] != NULL )  free(ppPara[0]);
		return;
	}
	
	pMaxTskPri = ppPara[0];
}


/* SYSTEM_STACK の解析 */
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
		/* パラメーターが不正 */
		fprintf(stdout, "line(%d): パラメーターが不正です\n", nLine);
	}
	
	free(ppPara[0]);
}


/* ------------------------------------ */
/*           メモリ開放                 */
/* ------------------------------------ */

/* パラメータ保存部分の開放 */
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

/* パラメーターテーブルの開放 */
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
/*            文字列処理                */
/* ------------------------------------ */

/* 前後の空白の除去 */
void CutSpace(char *pBuf)
{
	char *p;
	
	/* 後ろの空白カット */
	p = &pBuf[strlen(pBuf) - 1];
	while ( p >= pBuf && (*p == ' ' || *p == '\t' || *p == '\n') )
		p--;
	*(p + 1) = '\0';
	
	/* 前半の空白カット */
	p = pBuf;
	while ( *p == ' ' || *p == '\t' )
		p++;
	memmove(pBuf, p, strlen(p) + 1);
}


/* 括弧で囲まれた領域からパラメータを切り出す */
int ClipParameter(char *ppPar[], int nNum, char *pBuf)
{
	char *pBase;
	int  nParNest = 1;
	int  i;

	/* 始まりの '(' チェック */
	CutSpace(pBuf);
	if ( *pBuf++ != '(' ) {
		/* 文法エラー */
		fprintf(stdout, "line(%d): 文法エラー\n", nLine);
		bErr = TRUE;
		return FALSE;
	}
	
	/* 引数分解 */
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
				/* メモリ不足 */
				fprintf(stderr, "メモリが足りません\n");
				exit(1);
			}
			strcpy(ppPar[i], pBase);
		}
		else {
			/* パラメーター数が異常 */
			fprintf(stdout, "line(%d): パラメータ数が異常です\n", nLine);
			bErr = TRUE;
			return FALSE;
		}
	}
	
	return TRUE;
}


/* 文字列を数値に変換する(16進対応) */
int StrToInt(int *pNum, char *pBuf)
{
	/* 空白カット */
	CutSpace(pBuf);
	
	/* 数値かどうかチェック */
	if ( pBuf[0] < '0' || pBuf[0] > '9' )
		return FALSE;
	
	/* １６進かどうかチェック */
	if ( pBuf[0] == '0' && (pBuf[1] == 'x' || pBuf[1] == 'X') )
		return (sscanf(&pBuf[2], "%x", pNum) == 1);
	
	return (sscanf(&pBuf[0], "%d", pNum) == 1);
}


/* 文字列を数値に変換する(16進対応) */
int StrToLong(long *pNum, char *pBuf)
{
	/* 空白カット */
	CutSpace(pBuf);
	
	/* 数値かどうかチェック */
	if ( pBuf[0] < '0' || pBuf[0] > '9' )
		return FALSE;
	
	/* １６進かどうかチェック */
	if ( pBuf[0] == '0' && (pBuf[1] == 'x' || pBuf[1] == 'X') )
		return (sscanf(&pBuf[2], "%lx", pNum) == 1);
	
	return (sscanf(&pBuf[0], "%ld", pNum) == 1);
}
