# CY-Tech-Projet-info-Decembre-2024
Projet informatique CY Tech Decembre 2024

Le projet C-Wire est une application développée en C et Shell, visant à analyser les données de distribution d'électricité et à détecter les cas de surproduction ou de sous-production. Le programme s'appuie sur un fichier CSV volumineux contenant les données d'énergie produite et consommée, ainsi que la topologie des réseaux électriques.

Fonctionnalités

Extraction et filtrage des données via un script Shell.
Calcul des consommations et des capacités par station (HV-B, HV-A, LV) en utilisant un arbre AVL pour optimiser les performances.
Génération de fichiers CSV contenant :
L'identifiant des stations.
La capacité (kWh).
La consommation (kWh).
Analyse des postes LV pour identifier les 10 plus chargés et les 10 moins chargés.

Prérequis

Système d'exploitation : Linux ou tout environnement compatible Bash et GCC.
Outils nécessaires :
Compilateur GCC.
Make (pour la compilation).
Un terminal compatible Bash.
Fichiers inclus :
data.csv : Fichier d'entrée contenant les données de simulation.
c-wire.sh : Script Shell pour gérer les entrées, options, et appels au programme C.
codeC : Dossier contenant le code source C et le Makefile.
Installation

Clonez le dépôt :

git clone <URL_DU_DEPOT>
cd C-Wire

Compilez le programme C :

cd codeC
make

Utilisation

Script Shell

Le script Shell c-wire.sh est le point d'entrée principal. Il prend plusieurs arguments pour traiter les données.

Syntaxe :

./c-wire.sh <fichier_csv> <type_station> <type_consommateur> [id_centrale]


Arguments :

<fichier_csv> : Chemin du fichier CSV contenant les données d'entrée.
<type_station> :
hvb : Stations HV-B.
hva : Stations HV-A.
lv : Postes LV.
<type_consommateur> :
comp : Consommateurs entreprises.
indiv : Consommateurs particuliers.
all : Tous les consommateurs.
[id_centrale] : (Optionnel) Identifiant d'une centrale spécifique pour filtrer les résultats.

Exemple :

./c-wire.sh data.csv hvb comp
Traite les données pour les stations HV-B connectées aux entreprises.

Options interdites :

hvb indiv
hvb all
hva indiv
hva all
Résultats

Fichiers générés :
Résultats principaux :
Exemple : tests/hva_comp.csv
Identifiant des stations HV-A.
Capacité totale (kWh).
Consommation totale (kWh).
Postes LV (Option lv all) :
tests/lv_all_minmax.csv : Contient les 10 postes LV les plus et les moins chargés.

Format des fichiers CSV :

Les fichiers de sortie utilisent le 

format suivant :

StationID:Capacité:Consommation

Description des dossiers :

codeC/ : Contient le code source C et le Makefile.
tests/ : Contient les résultats générés par le programme.
tmp/ : Dossier temporaire (créé automatiquement par le script).

Arborescence:

C-Wire/
├── README.md
├── c-wire.sh
├── codeC/
│   ├── main.c
│   ├── avl.c
│   ├── avl.h
│   ├── utils.c
│   ├── utils.h
│   └── Makefile
├── data.csv
└── tests/
    ├── hva_comp.csv
    └── lv_all_minmax.csv

