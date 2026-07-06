# Compilateur Atlas → MAP
Ce projet porte sur la conception et l’implémentation complète d’un compilateur
pour le langage de programmation Atlas, un langage pédagogique à typage statique
simple. Le compilateur traduit les programmes sources Atlas vers un code objet
destiné à une Machine à Pile (MAP), puis ce code est exécuté par un simulateur
développé en C.
Le travail réalisé couvre l’intégralité de la chaîne de compilation : analyse lexicale
avec Flex, analyse syntaxique et sémantique avec Bison, génération de code inter
médiaire MAP, et simulation d’exécution. Une attention particulière a été portée
aux règles sémantiques : gestion de la table des symboles, vérification des types,
détection des erreurs sémantiques (déclarations multiples, variables non déclarées,
incompatibilités de types, lvalues non valides).
Les objectifs assignés à ce projet sont quadruples :
1. Simulateur MAP : Réaliser en C un simulateur de la machine à pile
capable d’exécuter le code objet produit par le compilateur.
2. Compilateur Atlas : Écrire une grammaire attribuée pour le langage Atlas
à l’aide de Bison, enrichie par les actions sémantiques et la génération du
code objet.
3. Analyseur lexical : Écrire un analyseur lexical pour le langage Atlas à
l’aide de Flex.
4. Intégration : Écrire un programme principal qui permet d’afficher le code
objet MAP d’un programme source Atlas, puis de l’exécuter.
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
