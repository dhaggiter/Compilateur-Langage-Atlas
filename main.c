#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"
#include "codegen.h"
#include "simulator.h"

/* Déclarés dans atlas.y */
extern SymTable TS;
extern CodeGen  CG;
extern FILE *yyin;
extern int yyparse(void);

static void change_ext(const char *src, char *dst, const char *new_ext) {
    /* Remplace ou ajoute l'extension */
    strcpy(dst, src);
    char *dot = strrchr(dst, '.');
    if (dot) *dot = '\0';
    strcat(dst, new_ext);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr,
            "Usage: %s <fichier.atlas> [--no-run] [--show-ts]\n"
            "  --no-run   : compiler uniquement (ne pas exécuter)\n"
            "  --show-ts  : afficher la table des symboles\n",
            argv[0]);
        return 1;
    }

    /* Options */
    int do_run   = 1;
    int show_ts  = 0;
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--no-run")  == 0) do_run  = 0;
        if (strcmp(argv[i], "--show-ts") == 0) show_ts = 1;
    }

    /* Ouvrir le fichier source */
    yyin = fopen(argv[1], "r");
    if (!yyin) { perror(argv[1]); return 1; }

    /* Initialiser les structures */
    ts_init(&TS);
    cg_init(&CG);

    /* Analyse lexicale + syntaxique + sémantique + génération de code */
    printf("=== Compilation de '%s' ===\n", argv[1]);
    yyparse();
    fclose(yyin);

    /* Fichier cible .map */
    char map_file[512];
    change_ext(argv[1], map_file, ".map");
    cg_write(&CG, map_file);
    printf("\n=== Code MAP généré : '%s' ===\n", map_file);
    cg_print(&CG);

    /* Table des symboles */
    if (show_ts) ts_print(&TS);

    /* Exécution */
    if (do_run) {
        printf("\n=== Exécution ===\n");
        map_run(&CG, &TS);
        printf("\n=== Fin d'exécution ===\n");
    }

    return 0;
}
