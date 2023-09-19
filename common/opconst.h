#ifndef _OPCONST_H_
#define _OPCONST_H_

/************************
 命令コード
************************/
#define SOP_MESSAGE		(0)
#define SOP_CHECKVAR	(1)
#define SOP_BLOCKEND	(2)
#define SOP_IFTRUE		(3)
#define SOP_IFFALSE		(4)

/************************
 パラメータ数
************************/
#define OPP_COUNT_MESSAGE	(4)
#define OPP_COUNT_CHECKVAR	(3)

/************************
 パラメータ番号
************************/
// MESSAGE
#define OPP_MESSAGE_SPEAKER		(0)
#define OPP_MESSAGE_VOICE		(1)
#define OPP_MESSAGE_MESSAGE		(2)
#define OPP_MESSAGE_MSGBOX		(3)
// CHECKVAR
#define OPP_CHECKVAR_VARIABLE	(0)
#define OPP_CHECKVAR_CONDITION	(1)
#define OPP_CHECKVAR_VALUE		(2)

/************************
 CheckVarのcondition
************************/
#define CHECKVAR_COND_EQ		(0)		// =  equal
#define CHECKVAR_COND_NOTEQ		(1)		// != not equal
#define CHECKVAR_COND_GT		(2)		// >  greater than
#define CHECKVAR_COND_EQGT		(3)		// >= equal or greater than
#define CHECKVAR_COND_LT		(4)		// <  lesser than
#define CHECKVAR_COND_EQLT		(5)		// <= equal or lesser than
#define IS_VALID_CVCOND(n)	(n>=0&&n<=5)

extern char *g_cvcond[];

#endif

/*
	Message {
		params {
			str speaker;
			str voice;
			str message;
			int msgbox;
		};
	};

	CheckVar {
		params {
			str variable;
			int	condition;
			str value;
		};
		children {
			IfTrue {
				BlockEnd;
			};
			IfFalse {
				BlockEnd;
			};
		};
	};
*/