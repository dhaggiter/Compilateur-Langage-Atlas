#include "codegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *opname(OpCode op) {
    switch(op) {
        case OP_OUVERTURE_BLOC: return "ouverture-bloc";
        case OP_FERMETURE_BLOC: return "fermeture-bloc";
        case OP_RESERVER_KST:   return "reserver-kst";
        case OP_RESERVER_VAR:   return "reserver-var";
        case OP_EMPILER_VAL:    return "empiler-val";
        case OP_EMPILER_ADR:    return "empiler-adr";
        case OP_VALEUR_PILE:    return "valeur-pile";
        case OP_AFFECT:         return "affect";
        case OP_LIRE:           return "lire";
        case OP_LIRERC:         return "lireRC";
        case OP_ECRIRE:         return "ecrire";
        case OP_ECRIRERC:       return "ecrireRC";
        case OP_IMPRIMER:       return "imprimer";
        case OP_IMPRIMERRC:     return "imprimerRC";
        case OP_PLUS:           return "plus";
        case OP_MOINS:          return "moins";
        case OP_MULT:           return "mult";
        case OP_DIV:            return "div";
        case OP_MOD:            return "mod";
        case OP_PUISS:          return "puiss";
        case OP_VALABS:         return "valabs";
        case OP_NEG:            return "neg";
        case OP_EGAL:           return "egal";
        case OP_PPS:            return "pps";
        case OP_PGS:            return "pgs";
        case OP_PP_EGAL:        return "pp-egal";
        case OP_PG_EGAL:        return "pg-egal";
        case OP_DIF:            return "dif";
        case OP_OU:             return "ou";
        case OP_ET:             return "et";
        case OP_NON:            return "non";
        case OP_BSF:            return "bsf";
        case OP_BSV:            return "bsv";
        case OP_BRA:            return "bra";
        default:                return "???";
    }
}

static int has_int_arg(OpCode op) {
    return op==OP_RESERVER_KST || op==OP_RESERVER_VAR ||
           op==OP_EMPILER_VAL  || op==OP_EMPILER_ADR  ||
           op==OP_BSF || op==OP_BSV || op==OP_BRA;
}

static int has_str_arg(OpCode op) {
    return op==OP_IMPRIMER || op==OP_IMPRIMERRC;
}

void cg_init(CodeGen *cg) { cg->count = 0; }

int cg_emit(CodeGen *cg, OpCode op, int arg, const char *str) {
    if (cg->count >= MAX_INSTRS) { fprintf(stderr, "Erreur: trop d'instructions\n"); exit(1); }
    int idx = cg->count++;
    cg->instrs[idx].op  = op;
    cg->instrs[idx].arg = arg;
    cg->instrs[idx].str[0] = '\0';
    if (str) strncpy(cg->instrs[idx].str, str, MAX_STR-1);
    return idx + 1;  /* numéro 1-based */
}

void cg_patch(CodeGen *cg, int idx, int new_arg) {
    /* idx est 1-based */
    cg->instrs[idx-1].arg = new_arg;
}

int cg_next(CodeGen *cg) { return cg->count + 1; }

static void print_instr(FILE *f, int num, Instr *ins) {
    if (has_str_arg(ins->op))
        fprintf(f, "%4d  %s '%s'\n", num, opname(ins->op), ins->str);
    else if (has_int_arg(ins->op))
        fprintf(f, "%4d  %s %d\n", num, opname(ins->op), ins->arg);
    else
        fprintf(f, "%4d  %s\n", num, opname(ins->op));
}

void cg_print(CodeGen *cg) {
    for (int i = 0; i < cg->count; i++)
        print_instr(stdout, i+1, &cg->instrs[i]);
}

void cg_write(CodeGen *cg, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) { perror(filename); return; }
    fprintf(f, "// fichier cible : %s\n", filename);
    for (int i = 0; i < cg->count; i++)
        print_instr(f, i+1, &cg->instrs[i]);
    fclose(f);
}
