%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atlas.tab.h"


%}

%option noyywrap

DIGIT    [0-9]
LETTER   [a-zA-Z_]
ID       {LETTER}({LETTER}|{DIGIT})*
INT      {DIGIT}+
STRING   '[^']*'

%%

"//"[^\n]*          { /* commentaire fin de ligne */ }
\n                  { yylineno++; }
[ \t\r]+            { /* espaces */ }

"programme"         { return PROGRAMME; }
"var"               { return VAR; }
"const"             { return CONST; }
"debut"             { return DEBUT; }
"fin"               { return FIN; }
"ent"               { return ENT; }
"bool"              { return BOOL_KW; }
"passer"            { return PASSER; }
"lire"              { return LIRE; }
"lireRC"            { return LIRERC; }
"ecrire"            { return ECRIRE; }
"ecrireRC"          { return ECRIRERC; }
"imprimer"          { return IMPRIMER; }
"imprimerRC"        { return IMPRIMERRC; }
"si"                { return SI; }
"alors"             { return ALORS; }
"sinon"             { return SINON; }
"fsi"               { return FSI; }
"pour"              { return POUR; }
"depuis"            { return DEPUIS; }
"jusqua"            { return JUSQUA; }
"faire"             { return FAIRE; }
"fpour"             { return FPOUR; }
"tantque"           { return TANTQUE; }
"ftq"               { return FTQ; }
"et"                { return ET; }
"ou"                { return OU; }
"non"               { return NON; }
"puiss"             { return PUISS; }
"valabs"            { return VALABS; }
"vrai"              { yylval.ival = 1; return ENTIER; }
"faux"              { yylval.ival = 0; return ENTIER; }

":="                { return AFFECT; }
"<="                { return PP_EGAL; }
">="                { return PG_EGAL; }
"<>"                { return DIF; }
"="                 { return EGAL; }
"<"                 { return INF; }
">"                 { return SUP; }
"+"                 { return PLUS; }
"-"                 { return MOINS; }
"*"                 { return MULT; }
"/"                 { return DIV; }
"%"                 { return MOD; }
"("                 { return LPAREN; }
")"                 { return RPAREN; }
";"                 { return SEMICOLON; }
","                 { return COMMA; }
"."                 { return DOT; }

{INT}               { yylval.ival = atoi(yytext); return ENTIER; }

{STRING}            {
                        /* copier sans les apostrophes */
                        int len = strlen(yytext)-2;
                        yylval.sval = (char*)malloc(len+1);
                        strncpy(yylval.sval, yytext+1, len);
                        yylval.sval[len] = '\0';
                        return CHAINE;
                    }

{ID}                {
                        yylval.sval = strdup(yytext);
                        return ID;
                    }

.                   { fprintf(stderr, "Ligne %d: caractère inconnu '%s'\n", yylineno, yytext); }

%%
