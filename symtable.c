#include "symtable.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ts_init(SymTable *ts) {
    ts->count    = 0;
    ts->next_adr = 0;
}

int ts_lookup(SymTable *ts, const char *name) {
    for (int i = 0; i < ts->count; i++)
        if (strcmp(ts->tab[i].name, name) == 0)
            return i;
    return -1;
}

int ts_put(SymTable *ts, const char *name, Genre g, Type t) {
    if (ts->count >= MAX_SYMBOLS) {
        fprintf(stderr, "Erreur: table des symboles pleine\n");
        exit(1);
    }
    int idx = ts->count++;
    strncpy(ts->tab[idx].name, name, MAX_NAME-1);
    ts->tab[idx].genre   = g;
    ts->tab[idx].type    = t;
    ts->tab[idx].adresse = ts->next_adr++;
    ts->tab[idx].valeur  = 0;
    return idx;
}

int ts_getAdr(SymTable *ts, const char *name) {
    int i = ts_lookup(ts, name);
    if (i < 0) { fprintf(stderr, "Erreur interne: symbole '%s' introuvable\n", name); exit(1); }
    return ts->tab[i].adresse;
}

int ts_getVal(SymTable *ts, int adr) {
    for (int i = 0; i < ts->count; i++)
        if (ts->tab[i].adresse == adr)
            return ts->tab[i].valeur;
    fprintf(stderr, "Erreur interne: adresse %d introuvable\n", adr);
    exit(1);
}

Genre ts_getGenre(SymTable *ts, const char *name) {
    int i = ts_lookup(ts, name);
    if (i < 0) { fprintf(stderr, "Erreur: symbole '%s' non déclaré\n", name); exit(1); }
    return ts->tab[i].genre;
}

Type ts_getType(SymTable *ts, const char *name) {
    int i = ts_lookup(ts, name);
    if (i < 0) { fprintf(stderr, "Erreur: symbole '%s' non déclaré\n", name); exit(1); }
    return ts->tab[i].type;
}

void ts_setVal(SymTable *ts, int adr, int val) {
    for (int i = 0; i < ts->count; i++)
        if (ts->tab[i].adresse == adr) { ts->tab[i].valeur = val; return; }
}

void ts_print(SymTable *ts) {
    printf("\n=== Table des symboles ===\n");
    printf("%-12s %-8s %-6s %-8s %-6s\n","Nom","Genre","Type","Adresse","Valeur");
    printf("------------------------------------------\n");
    for (int i = 0; i < ts->count; i++) {
        Symbol *s = &ts->tab[i];
        printf("%-12s %-8s %-6s %-8d %-6d\n",
            s->name,
            s->genre==GENRE_CONST?"Const":"Var",
            s->type==TYPE_ENT?"Ent":"Bool",
            s->adresse,
            s->valeur);
    }
    printf("==========================================\n\n");
}
