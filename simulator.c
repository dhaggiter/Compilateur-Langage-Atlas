#include "simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PILE_SIZE 65536

static int  pile[PILE_SIZE];
static int  ip  = -1;   /* sommet de pile (index, -1 = vide) */
static int  CO  =  0;   /* compteur ordinale (0-based en interne) */

/* Macro d'accès */
#define TOP   pile[ip]
#define NEXT  pile[ip+1]

static void push(int v) {
    if (ip + 1 >= PILE_SIZE) { fprintf(stderr, "ERREUR: débordement de pile\n"); exit(1); }
    ip++;
    pile[ip] = v;
}

static int pop_val(void) {
    if (ip < 0) { fprintf(stderr, "ERREUR: pile vide (dépilage)\n"); exit(1); }
    return pile[ip--];
}

void map_run(CodeGen *cg, SymTable *ts) {
    ip = -1;
    CO = 0;

    while (CO < cg->count) {
        Instr *ins = &cg->instrs[CO];
        CO++;   /* avancement normal */

        switch (ins->op) {

        /* ---------- Gestion de bloc ---------- */
        case OP_OUVERTURE_BLOC:
            ip = -1;
            break;

        case OP_FERMETURE_BLOC:
            /* fin du programme */
            return;

        /* ---------- Réservation mémoire ---------- */
        case OP_RESERVER_KST: {
            /* Réserver N cases pour les constantes et les initialiser */
            int N = ins->arg;
            for (int i = 0; i < N; i++) {
                ip++;
                pile[ip] = ts_getVal(ts, i);
            }
            break;
        }

        case OP_RESERVER_VAR: {
            /* Réserver N cases à 0 pour les variables */
            int N = ins->arg;
            for (int i = 0; i < N; i++) {
                ip++;
                pile[ip] = 0;
            }
            break;
        }

        /* ---------- Empilements ---------- */
        case OP_EMPILER_VAL:
            push(ins->arg);
            break;

        case OP_EMPILER_ADR:
            push(ins->arg);
            break;

        /* ---------- Déréférencement ---------- */
        case OP_VALEUR_PILE:
            /* pile[ip] est une adresse → remplacer par la valeur */
            pile[ip] = pile[pile[ip]];
            break;

        /* ---------- Affectation ---------- */
        case OP_AFFECT: {
            int val = pop_val();
            int adr = pop_val();
            pile[adr] = val;
            break;
        }

        /* ---------- Lecture ---------- */
        case OP_LIRE: {
            int adr = pile[ip];   /* adresse au sommet */
            int v;
            if (scanf("%d", &v) != 1) {
                fprintf(stderr, "ERREUR: lecture entier impossible\n"); exit(1);
            }
            pile[adr] = v;
            ip--;   /* dépiler l'adresse */
            break;
        }

        case OP_LIRERC: {
            int adr = pile[ip];
            int v;
            if (scanf("%d", &v) != 1) {
                fprintf(stderr, "ERREUR: lecture entier impossible\n"); exit(1);
            }
            pile[adr] = v;
            ip--;
            /* sauter la fin de ligne */
            int c; while ((c = getchar()) != '\n' && c != EOF);
            break;
        }

        /* ---------- Écriture ---------- */
        case OP_ECRIRE:
            printf("%d ", pile[ip]);
            ip--;
            break;

        case OP_ECRIRERC:
            printf("%d\n", pile[ip]);
            ip--;
            break;

        case OP_IMPRIMER:
            printf("%s", ins->str);
            break;

        case OP_IMPRIMERRC:
            printf("%s\n", ins->str);
            break;

        /* ---------- Opérateurs arithmétiques binaires ---------- */
        case OP_PLUS:
            { int b = pop_val(); pile[ip] += b; break; }
        case OP_MOINS:
            { int b = pop_val(); pile[ip] -= b; break; }
        case OP_MULT:
            { int b = pop_val(); pile[ip] *= b; break; }
        case OP_DIV:
            { int b = pop_val();
              if (b == 0) { fprintf(stderr, "ERREUR: division par zéro\n"); exit(1); }
              pile[ip] /= b; break; }
        case OP_MOD:
            { int b = pop_val();
              if (b == 0) { fprintf(stderr, "ERREUR: modulo par zéro\n"); exit(1); }
              pile[ip] %= b; break; }
        case OP_PUISS:
            { int b = pop_val(); int a = pop_val();
              if (a == 0 && b == 0) { fprintf(stderr, "ERREUR: 0^0 illégal\n"); exit(1); }
              push((int)pow((double)a, (double)b)); break; }

        /* ---------- Opérateurs arithmétiques unaires ---------- */
        case OP_VALABS:
            pile[ip] = pile[ip] >= 0 ? pile[ip] : -pile[ip];
            break;
        case OP_NEG:
            pile[ip] = -pile[ip];
            break;

        /* ---------- Comparaisons ---------- */
        case OP_EGAL:
            { int b = pop_val(); pile[ip] = (pile[ip] == b) ? 1 : 0; break; }
        case OP_PPS:
            { int b = pop_val(); pile[ip] = (pile[ip] <  b) ? 1 : 0; break; }
        case OP_PGS:
            { int b = pop_val(); pile[ip] = (pile[ip] >  b) ? 1 : 0; break; }
        case OP_PP_EGAL:
            { int b = pop_val(); pile[ip] = (pile[ip] <= b) ? 1 : 0; break; }
        case OP_PG_EGAL:
            { int b = pop_val(); pile[ip] = (pile[ip] >= b) ? 1 : 0; break; }
        case OP_DIF:
            { int b = pop_val(); pile[ip] = (pile[ip] != b) ? 1 : 0; break; }

        /* ---------- Logique ---------- */
        case OP_OU:
            { int b = pop_val(); pile[ip] = (pile[ip] || b) ? 1 : 0; break; }
        case OP_ET:
            { int b = pop_val(); pile[ip] = (pile[ip] && b) ? 1 : 0; break; }
        case OP_NON:
            pile[ip] = pile[ip] ? 0 : 1;
            break;

        /* ---------- Branchements ---------- */
        case OP_BSF:
            /* branchement si faux (pile[ip] == 0) */
            if (pile[ip] == 0) CO = ins->arg - 1;   /* -1 car CO sera ++ au début */
            ip--;
            break;

        case OP_BSV:
            if (pile[ip] != 0) CO = ins->arg - 1;
            ip--;
            break;

        case OP_BRA:
            CO = ins->arg - 1;
            break;

        default:
            fprintf(stderr, "ERREUR: instruction MAP inconnue (op=%d)\n", ins->op);
            exit(1);
        }
    }
}
