%define parse.error verbose
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "codegen.h"

extern int yylineno;
int yylex(void);
void yyerror(const char *msg);

SymTable TS;
CodeGen  CG;

static int nb_const = 0;
static int nb_var   = 0;
static Type current_type = TYPE_ENT;

static void sem_error(const char *msg) {
    fprintf(stderr, "Erreur semantique (ligne %d): %s\n", yylineno, msg);
    exit(1);
}

/* Pile d'étiquettes pour gérer les branchements imbriqués */
#define ETIQ_MAX 256
static int etiq_stack[ETIQ_MAX];
static int etiq_top = 0;
static void push_etiq(int e) { etiq_stack[etiq_top++] = e; }
static int  pop_etiq(void)   { return etiq_stack[--etiq_top]; }
%}

%union {
    int   ival;
    char *sval;
}

%token PROGRAMME VAR CONST DEBUT FIN
%token ENT BOOL_KW
%token PASSER
%token LIRE LIRERC ECRIRE ECRIRERC IMPRIMER IMPRIMERRC
%token SI ALORS SINON FSI
%token POUR DEPUIS JUSQUA FAIRE FPOUR
%token TANTQUE FTQ
%token ET OU NON PUISS VALABS
%token AFFECT PP_EGAL PG_EGAL DIF EGAL INF SUP
%token PLUS MOINS MULT DIV MOD
%token LPAREN RPAREN SEMICOLON COMMA DOT

%token <ival> ENTIER
%token <sval> ID CHAINE

%type <ival> expression_const

%%

programme :
    PROGRAMME ID SEMICOLON partie_decl prologue liste_instrs FIN DOT
    { cg_emit(&CG, OP_FERMETURE_BLOC, 0, NULL); free($2); }
;

prologue :
    DEBUT
    {
        cg_emit(&CG, OP_OUVERTURE_BLOC, 0, NULL);
        if (nb_const > 0) cg_emit(&CG, OP_RESERVER_KST, nb_const, NULL);
        if (nb_var   > 0) cg_emit(&CG, OP_RESERVER_VAR, nb_var,   NULL);
    }
;

partie_decl :
    decl_const decl_var | decl_const | decl_var | /* vide */
;

decl_const : CONST liste_decl_const ;

liste_decl_const :
    liste_decl_const decl_une_const | decl_une_const
;

decl_une_const :
    type_kw ID EGAL expression_const SEMICOLON
    {
        if (ts_lookup(&TS, $2) != -1) sem_error("declaration multiple d'un meme objet");
        ts_put(&TS, $2, GENRE_CONST, current_type);
        ts_setVal(&TS, ts_getAdr(&TS, $2), $4);
        nb_const++;
        free($2);
    }
;

expression_const :
    ENTIER                                   { $$ = $1; }
|   MOINS ENTIER                             { $$ = -$2; }
|   ID
    {
        int idx = ts_lookup(&TS, $1);
        if (idx < 0) sem_error("constante non declaree");
        $$ = ts_getVal(&TS, ts_getAdr(&TS, $1));
        free($1);
    }
|   expression_const PLUS  expression_const  { $$ = $1 + $3; }
|   expression_const MOINS expression_const  { $$ = $1 - $3; }
|   expression_const MULT  expression_const  { $$ = $1 * $3; }
|   LPAREN expression_const RPAREN           { $$ = $2; }
;

decl_var : VAR liste_type_ids ;

liste_type_ids :
    liste_type_ids type_kw liste_ids SEMICOLON
|   type_kw liste_ids SEMICOLON
;

liste_ids :
    liste_ids COMMA ID
    {
        if (ts_lookup(&TS, $3) != -1) sem_error("declaration multiple d'un meme objet");
        ts_put(&TS, $3, GENRE_VAR, current_type);
        nb_var++;
        free($3);
    }
|   ID
    {
        if (ts_lookup(&TS, $1) != -1) sem_error("declaration multiple d'un meme objet");
        ts_put(&TS, $1, GENRE_VAR, current_type);
        nb_var++;
        free($1);
    }
;

type_kw :
    ENT     { current_type = TYPE_ENT;  }
|   BOOL_KW { current_type = TYPE_BOOL; }
;

/* ================================================================ */
liste_instrs :
    liste_instrs instruction | instruction
;

instruction :
    instr_affectation | instr_passer | instr_lire
|   instr_ecrire | instr_si | instr_pour | instr_tantque
;

instr_affectation :
    instr_lvalue AFFECT expression SEMICOLON
    { cg_emit(&CG, OP_AFFECT, 0, NULL); }
;

instr_lvalue :
    ID
    {
        int idx = ts_lookup(&TS, $1);
        if (idx < 0) sem_error("variable non declaree");
        if (ts_getGenre(&TS, $1) == GENRE_CONST)
            sem_error("affectation impossible a une constante");
        cg_emit(&CG, OP_EMPILER_ADR, ts_getAdr(&TS, $1), NULL);
        free($1);
    }
;

instr_passer : PASSER SEMICOLON { } ;

instr_lire :
    LIRE ID SEMICOLON
    {
        if (ts_lookup(&TS, $2) < 0) sem_error("variable non declaree");
        if (ts_getGenre(&TS, $2) == GENRE_CONST) sem_error("lire: lvalue requis");
        cg_emit(&CG, OP_EMPILER_ADR, ts_getAdr(&TS, $2), NULL);
        cg_emit(&CG, OP_LIRE, 0, NULL);
        free($2);
    }
|   LIRERC ID SEMICOLON
    {
        if (ts_lookup(&TS, $2) < 0) sem_error("variable non declaree");
        if (ts_getGenre(&TS, $2) == GENRE_CONST) sem_error("lireRC: lvalue requis");
        cg_emit(&CG, OP_EMPILER_ADR, ts_getAdr(&TS, $2), NULL);
        cg_emit(&CG, OP_LIRERC, 0, NULL);
        free($2);
    }
;

instr_ecrire :
    ECRIRE ID SEMICOLON
    {
        if (ts_lookup(&TS, $2) < 0) sem_error("identifiant non declare");
        cg_emit(&CG, OP_EMPILER_ADR, ts_getAdr(&TS, $2), NULL);
        cg_emit(&CG, OP_VALEUR_PILE, 0, NULL);
        cg_emit(&CG, OP_ECRIRE, 0, NULL);
        free($2);
    }
|   ECRIRERC ID SEMICOLON
    {
        if (ts_lookup(&TS, $2) < 0) sem_error("identifiant non declare");
        cg_emit(&CG, OP_EMPILER_ADR, ts_getAdr(&TS, $2), NULL);
        cg_emit(&CG, OP_VALEUR_PILE, 0, NULL);
        cg_emit(&CG, OP_ECRIRERC, 0, NULL);
        free($2);
    }
|   IMPRIMER CHAINE SEMICOLON
    { cg_emit(&CG, OP_IMPRIMER, 0, $2); free($2); }
|   IMPRIMERRC CHAINE SEMICOLON
    { cg_emit(&CG, OP_IMPRIMERRC, 0, $2); free($2); }
;

/* ================================================================
   SI : deux règles claires, FSI ferme toujours le bloc
   On utilise la pile d'étiquettes pour les mid-rule actions
   ================================================================ */

/* Marqueur : émet BSF après la condition */
marque_bsf :
    /* vide */ { push_etiq(cg_emit(&CG, OP_BSF, 0, NULL)); }
;

/* Marqueur : patch le BSF, émet BRA, push nouveau bra_idx */
marque_sinon :
    /* vide */ {
        int bra_idx = cg_emit(&CG, OP_BRA, 0, NULL);
        cg_patch(&CG, pop_etiq(), cg_next(&CG)); /* patch bsf → ici (début sinon) */
        push_etiq(bra_idx);
    }
;

instr_si :
    /* SI sans SINON */
    SI condition ALORS marque_bsf liste_instrs FSI
    { cg_patch(&CG, pop_etiq(), cg_next(&CG)); }
    
|   /* SI avec SINON */
    SI condition ALORS marque_bsf liste_instrs SINON marque_sinon liste_instrs FSI
    { cg_patch(&CG, pop_etiq(), cg_next(&CG)); }
;

/* ================================================================
   BOUCLE POUR
   ================================================================ */

marque_debut_boucle :
    /* vide */ { push_etiq(cg_next(&CG)); }
;

instr_pour :
    POUR ID
    {
        if (ts_lookup(&TS, $<sval>2) < 0) sem_error("variable de boucle non declaree");
        if (ts_getGenre(&TS, $<sval>2) == GENRE_CONST)
            sem_error("variable de boucle ne peut pas etre une constante");
        push_etiq(ts_getAdr(&TS, $<sval>2)); /* empile l'adresse de la var */
    }
    DEPUIS
    {
        /* Pour affecter vi à la variable */
        cg_emit(&CG, OP_EMPILER_ADR, etiq_stack[etiq_top-1], NULL);
    }
    expression
    {
        /* var := vi */
        cg_emit(&CG, OP_AFFECT, 0, NULL);
        /* debut du test: push */
        push_etiq(cg_next(&CG));
        /* Charger valeur de la variable */
        cg_emit(&CG, OP_EMPILER_ADR, etiq_stack[etiq_top-2], NULL);
        cg_emit(&CG, OP_VALEUR_PILE, 0, NULL);
    }
    JUSQUA expression
    {
        /* var <= vf */
        cg_emit(&CG, OP_PP_EGAL, 0, NULL);
        push_etiq(cg_emit(&CG, OP_BSF, 0, NULL)); /* push bsf_idx */
    }
    FAIRE liste_instrs FPOUR
    {
        int bsf_idx  = pop_etiq();
        int loop_top = pop_etiq();
        int var_adr  = pop_etiq();
        /* Incrément: var := var + 1 */
        cg_emit(&CG, OP_EMPILER_ADR, var_adr, NULL);
        cg_emit(&CG, OP_EMPILER_ADR, var_adr, NULL);
        cg_emit(&CG, OP_VALEUR_PILE, 0, NULL);
        cg_emit(&CG, OP_EMPILER_VAL, 1, NULL);
        cg_emit(&CG, OP_PLUS, 0, NULL);
        cg_emit(&CG, OP_AFFECT, 0, NULL);
        cg_emit(&CG, OP_BRA, loop_top, NULL);
        cg_patch(&CG, bsf_idx, cg_next(&CG));
        free($2);
    }
;

/* ================================================================
   BOUCLE TANT QUE
   ================================================================ */
instr_tantque :
    TANTQUE
    { push_etiq(cg_next(&CG)); }          /* debut test */
    condition FAIRE
    { push_etiq(cg_emit(&CG, OP_BSF, 0, NULL)); }
    liste_instrs FTQ
    {
        int bsf_idx  = pop_etiq();
        int loop_top = pop_etiq();
        cg_emit(&CG, OP_BRA, loop_top, NULL);
        cg_patch(&CG, bsf_idx, cg_next(&CG));
    }
;

/* ================================================================
   EXPRESSIONS
   ================================================================ */
expression :
    expression PLUS  terme4  { cg_emit(&CG, OP_PLUS,  0, NULL); }
|   expression MOINS terme4  { cg_emit(&CG, OP_MOINS, 0, NULL); }
|   terme4
;

terme4 :
    terme4 MULT terme3  { cg_emit(&CG, OP_MULT, 0, NULL); }
|   terme4 DIV  terme3  { cg_emit(&CG, OP_DIV,  0, NULL); }
|   terme4 MOD  terme3  { cg_emit(&CG, OP_MOD,  0, NULL); }
|   terme3
;

terme3 :
    MOINS terme2  { cg_emit(&CG, OP_NEG, 0, NULL); }
|   terme2
;

terme2 :
    facteur PUISS facteur               { cg_emit(&CG, OP_PUISS, 0, NULL); }
|   VALABS LPAREN expression RPAREN     { cg_emit(&CG, OP_VALABS, 0, NULL); }
|   facteur
;

facteur :
    ENTIER
        { cg_emit(&CG, OP_EMPILER_VAL, $1, NULL); }
|   ID
    {
        int idx = ts_lookup(&TS, $1);
        if (idx < 0) sem_error("identifiant non declare");
        cg_emit(&CG, OP_EMPILER_ADR, ts_getAdr(&TS, $1), NULL);
        cg_emit(&CG, OP_VALEUR_PILE, 0, NULL);
        free($1);
    }
|   LPAREN expression RPAREN
;

/* ================================================================
   CONDITIONS
   ================================================================ */
condition :
    condition OU  atome_bool  { cg_emit(&CG, OP_OU, 0, NULL); }
|   condition ET  atome_bool  { cg_emit(&CG, OP_ET, 0, NULL); }
|   NON atome_bool             { cg_emit(&CG, OP_NON, 0, NULL); }
|   atome_bool
;

atome_bool :
    expression EGAL    expression  { cg_emit(&CG, OP_EGAL,    0, NULL); }
|   expression INF     expression  { cg_emit(&CG, OP_PPS,     0, NULL); }
|   expression SUP     expression  { cg_emit(&CG, OP_PGS,     0, NULL); }
|   expression PP_EGAL expression  { cg_emit(&CG, OP_PP_EGAL, 0, NULL); }
|   expression PG_EGAL expression  { cg_emit(&CG, OP_PG_EGAL, 0, NULL); }
|   expression DIF     expression  { cg_emit(&CG, OP_DIF,     0, NULL); }
|   LPAREN condition RPAREN
;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Erreur syntaxique (ligne %d): %s\n", yylineno, msg);
    exit(1);
}
