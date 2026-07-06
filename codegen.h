#ifndef CODEGEN_H
#define CODEGEN_H

#define MAX_INSTRS 4096
#define MAX_STR     256

typedef enum {
    OP_OUVERTURE_BLOC,
    OP_FERMETURE_BLOC,
    OP_RESERVER_KST,
    OP_RESERVER_VAR,
    OP_EMPILER_VAL,
    OP_EMPILER_ADR,
    OP_VALEUR_PILE,
    OP_AFFECT,
    OP_LIRE,
    OP_LIRERC,
    OP_ECRIRE,
    OP_ECRIRERC,
    OP_IMPRIMER,
    OP_IMPRIMERRC,
    OP_PLUS,
    OP_MOINS,
    OP_MULT,
    OP_DIV,
    OP_MOD,
    OP_PUISS,
    OP_VALABS,
    OP_NEG,
    OP_EGAL,
    OP_PPS,   /* < */
    OP_PGS,   /* > */
    OP_PP_EGAL, /* <= */
    OP_PG_EGAL, /* >= */
    OP_DIF,
    OP_OU,
    OP_ET,
    OP_NON,
    OP_BSF,
    OP_BSV,
    OP_BRA
} OpCode;

typedef struct {
    OpCode op;
    int    arg;          /* argument entier (N, K, A, etiq) */
    char   str[MAX_STR]; /* argument chaîne (imprimer) */
} Instr;

typedef struct {
    Instr instrs[MAX_INSTRS];
    int   count;
} CodeGen;

void cg_init    (CodeGen *cg);
int  cg_emit    (CodeGen *cg, OpCode op, int arg, const char *str);
void cg_patch   (CodeGen *cg, int idx, int new_arg);  /* rétro-patch d'une étiquette */
int  cg_next    (CodeGen *cg);                        /* prochain numéro d'instruction */
void cg_write   (CodeGen *cg, const char *filename);  /* écriture fichier .map */
void cg_print   (CodeGen *cg);

#endif
