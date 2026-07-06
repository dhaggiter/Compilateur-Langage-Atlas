#ifndef SYMTABLE_H
#define SYMTABLE_H

#define MAX_SYMBOLS 256
#define MAX_NAME    64

typedef enum { GENRE_CONST, GENRE_VAR } Genre;
typedef enum { TYPE_ENT, TYPE_BOOL } Type;

typedef struct {
    char  name[MAX_NAME];
    Genre genre;
    Type  type;
    int   adresse;
    int   valeur;
} Symbol;

typedef struct {
    Symbol tab[MAX_SYMBOLS];
    int    count;
    int    next_adr;
} SymTable;

void ts_init   (SymTable *ts);
int  ts_lookup (SymTable *ts, const char *name);   /* -1 si absent */
int  ts_put    (SymTable *ts, const char *name, Genre g, Type t);
int  ts_getAdr (SymTable *ts, const char *name);
int  ts_getVal (SymTable *ts, int adr);
Genre ts_getGenre(SymTable *ts, const char *name);
Type  ts_getType (SymTable *ts, const char *name);
void ts_setVal (SymTable *ts, int adr, int val);
void ts_print  (SymTable *ts);

#endif
