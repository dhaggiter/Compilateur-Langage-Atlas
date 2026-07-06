# Compilateur Atlas → MAP

## Structure du projet

| Fichier | Rôle |
|---------|------|
| `atlas.l` | Analyseur lexical (Flex) |
| `atlas.y` | Grammaire attribuée + actions sémantiques + génération de code (Bison) |
| `symtable.h/c` | Table des symboles |
| `codegen.h/c` | Générateur de code MAP |
| `simulator.h/c` | Simulateur de la machine à pile MAP |
| `main.c` | Programme principal |

## Compilation

```bash
make
```

## Utilisation

```bash
./atlas <fichier.atlas> [--no-run] [--show-ts]
```

- `--no-run`  : compiler uniquement (ne pas simuler)
- `--show-ts` : afficher la table des symboles

## Exemples

```bash
echo "3 5" | ./atlas prog1.atlas          # somme : 8
echo "60"  | ./atlas prog2.atlas          # min(120, 100) = 100
echo "5"   | ./atlas prog3.atlas          # 5! = 120
echo "-1"  | ./atlas prog3.atlas          # impossible
```

## Langage Atlas (résumé)

- Types : `ent`, `bool`
- Constantes (`const`) et variables (`var`)
- Instructions : affectation (`:=`), `lire`, `ecrire`, `imprimer`, `si/alors/sinon/fsi`, `pour/depuis/jusqua/faire/fpour`, `tantque/faire/ftq`
- Commentaires : `// ...`

## Machine cible MAP

Machine à pile avec les ordres :
`ouverture-bloc`, `fermeture-bloc`, `reserver-kst N`, `reserver-var N`,
`empiler-val K`, `empiler-adr A`, `valeur-pile`, `affect`,
`lire`, `lireRC`, `ecrire`, `ecrireRC`, `imprimer`, `imprimerRC`,
`plus`, `moins`, `mult`, `div`, `mod`, `puiss`, `valabs`, `neg`,
`egal`, `pps`, `pgs`, `pp-egal`, `pg-egal`, `dif`, `ou`, `et`, `non`,
`bsf etiq`, `bsv etiq`, `bra etiq`
