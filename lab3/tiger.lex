%{
/* Lab2 Attention: You are only allowed to add code in this file and start at Line 26.*/
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"
#include "absyn.h"
#include "y.tab.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}

void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

/*
* Please don't modify the lines above.
* You can add C declarations of your own below.
*/

void adjustPos(void)
{
 charPos+=yyleng;
}

#include <stdio.h>
#include <ctype.h>
int comments = 0;
int idx = 0;
char text[2048];

char *getstr(const char *str)
{
	//optional: implement this function if you need it
	char *result = malloc(strlen(str) + 1);
	strcpy(result, str);
	return result;
}

%}
  /* You can add lex definitions here. */
%Start COMMENT STR
%% 
  /* 
  * Below is an example, which you can wipe out
  * and write reguler expressions and actions of your own.
  */ 

<STR>\\n {adjustPos(); text[idx] = '\n'; ++idx;}
<STR>\\t {adjustPos(); text[idx] = '\t'; ++idx;}
<STR>\\[\ \t\n\f]+\\ {adjustPos();}
<STR>\\[0-9]{3} {adjustPos(); text[idx] = atoi(yytext + 1); ++idx;}
<STR>\\\^[A-Z] {adjustPos(); text[idx] = yytext[2] - 'A' + 1; ++idx;}
<STR>\\\^@ {adjustPos(); text[idx] = 0; ++idx;}
<STR>\\\^\[ {adjustPos(); text[idx] = 27; ++idx;}
<STR>\\\" {adjustPos(); text[idx] = '\"'; ++idx;}
<STR>\\\\ {adjustPos(); text[idx] = '\\'; ++idx;}
<STR>\\ {adjustPos();}
<STR>\" {adjustPos(); text[idx] = '\0';yylval.sval = String(text); BEGIN INITIAL; if(strlen(text) == 0){yylval.sval = "(null)";} return STRING;}
<STR>\n {adjustPos(); EM_error(charPos, "didn't close the string");}
<STR>. {adjustPos(); strcpy(text + idx, yytext); idx += yyleng;}

<INITIAL>"/*" {adjust(); ++comments; BEGIN COMMENT;}
<INITIAL>"," {adjust(); return COMMA;}
<INITIAL>":" {adjust(); return COLON;}
<INITIAL>";" {adjust(); return SEMICOLON;}
<INITIAL>"(" {adjust(); return LPAREN;}
<INITIAL>")" {adjust(); return RPAREN;}
<INITIAL>"[" {adjust(); return LBRACK;}
<INITIAL>"]" {adjust(); return RBRACK;}
<INITIAL>"{" {adjust(); return LBRACE;}
<INITIAL>"}" {adjust(); return RBRACE;}
<INITIAL>"." {adjust(); return DOT;}
<INITIAL>"+" {adjust(); return PLUS;}
<INITIAL>"-" {adjust(); return MINUS;}
<INITIAL>"*" {adjust(); return TIMES;}
<INITIAL>"/" {adjust(); return DIVIDE;}
<INITIAL>"=" {adjust(); return EQ;}
<INITIAL>"<>" {adjust(); return NEQ;}
<INITIAL>"<" {adjust(); return LT;}
<INITIAL>"<=" {adjust(); return LE;}
<INITIAL>">" {adjust(); return GT;}
<INITIAL>">=" {adjust(); return GE;}
<INITIAL>"&" {adjust(); return AND;}
<INITIAL>"|" {adjust(); return OR;}
<INITIAL>":=" {adjust(); return ASSIGN;}
<INITIAL>"array" {adjust(); return ARRAY;}
<INITIAL>"if" {adjust(); return IF;}
<INITIAL>"then" {adjust(); return THEN;}
<INITIAL>"else" {adjust(); return ELSE;}
<INITIAL>"while" {adjust(); return WHILE;}
<INITIAL>"for" {adjust(); return FOR;}
<INITIAL>"to" {adjust(); return TO;}
<INITIAL>"do" {adjust(); return DO;}
<INITIAL>"let" {adjust(); return LET;}
<INITIAL>"in" {adjust(); return IN;}
<INITIAL>"end" {adjust(); return END;}
<INITIAL>"of" {adjust(); return OF;}
<INITIAL>"break" {adjust(); return BREAK;}
<INITIAL>"nil" {adjust(); return NIL;}
<INITIAL>"function" {adjust(); return FUNCTION;}
<INITIAL>"var" {adjust(); return VAR;}
<INITIAL>"type" {adjust(); return TYPE;}
<INITIAL>[a-zA-Z][0-9a-zA-Z_]* {adjust(); yylval.sval=String(yytext); return ID;}
<INITIAL>[0-9]+ {adjust(); yylval.ival=atoi(yytext);return INT;}
<INITIAL>\ |\t {adjust();}
<INITIAL>"\n" {adjust(); EM_newline();}
<INITIAL>"\"" {adjust(); idx = 0; BEGIN STR;}
<INITIAL>. {adjust(); EM_error(charPos, yytext);}

<COMMENT>"/*" {adjust(); ++comments;}
<COMMENT>"*/" {adjust(); --comments; if (comments == 0){BEGIN INITIAL;}}
<COMMENT>.|\n {adjust();}


