#!/bin/bash



# Vérification des arguments et option d'aide
if [[ "$1" == "-h" || $# -lt 3 ]]; then
    echo "Usage: $0 <fichier_csv> <type_station> <type_consommateur> [id_centrale]"
    echo "Types de station : hvb, hva, lv"
    echo "Types de consommateur : comp, indiv, all"
    echo "Exemple : $0 data.csv hva comp 1"
    echo "- Les options interdites : hvb indiv, hvb all, hva indiv, hva all"
    exit 0
fi

# Paramètres
CSV_FILE="$1"
TYPE_STATION="$2"
TYPE_CONSOMMATEUR="$3"
ID_CENTRALE="${4:-}" # Optionnel

# Validation des paramètres
if [[ ! -f "$CSV_FILE" ]]; then
    echo "Erreur : Le fichier $CSV_FILE n'existe pas."
    exit 1
fi

if [[ "$TYPE_STATION" != "hvb" && "$TYPE_STATION" != "hva" && "$TYPE_STATION" != "lv" ]]; then
    echo "Erreur : Type de station invalide. Utilisez hvb, hva ou lv."
    exit 1
fi

if [[ "$TYPE_CONSOMMATEUR" != "comp" && "$TYPE_CONSOMMATEUR" != "indiv" && "$TYPE_CONSOMMATEUR" != "all" ]]; then
    echo "Erreur : Type de consommateur invalide. Utilisez comp, indiv ou all."
    exit 1
fi

if [[ "$TYPE_STATION" == "hvb" && "$TYPE_CONSOMMATEUR" != "comp" ]]; then
    echo "Erreur : Les options hvb indiv ou hvb all sont interdites."
    exit 1
fi

if [[ "$TYPE_STATION" == "hva" && "$TYPE_CONSOMMATEUR" != "comp" ]]; then
    echo "Erreur : Les options hva indiv ou hva all sont interdites."
    exit 1
fi

# Création des dossiers nécessaires
mkdir -p tmp tests
rm -rf tmp/*

# Vérification de l'exécutable C
if [[ ! -f codeC/main ]]; then
    echo "Compilation du programme C..."
    (cd codeC && make)
    if [[ $? -ne 0 ]]; then
        echo "Erreur : Échec de la compilation du programme C."
        exit 1
    fi
fi

# Filtrage des données CSV si nécessaire
FILTERED_FILE="tmp/filtered_data.csv"
if [[ -n "$ID_CENTRALE" ]]; then
    echo "Filtrage des données pour la centrale $ID_CENTRALE..."
    grep "^$ID_CENTRALE" "$CSV_FILE" > "$FILTERED_FILE"
else
    cp "$CSV_FILE" "$FILTERED_FILE"
fi

# Définition du fichier de sortie
OUTPUT_FILE="tests/${TYPE_STATION}_${TYPE_CONSOMMATEUR}.csv"

# Appel du programme C
echo "Traitement des données avec le programme C..."
START_TIME=$(date +%s.%N)
codeC/main "$FILTERED_FILE" "$OUTPUT_FILE"
EXIT_CODE=$?
END_TIME=$(date +%s.%N)

if [[ $EXIT_CODE -ne 0 ]]; then
    echo "Erreur : Le programme C a rencontré une erreur."
    exit 1
fi

# Calcul et affichage de la durée d'exécution
DURATION=$(echo "$END_TIME - $START_TIME" | bc)
echo "Traitement terminé en $DURATION secondes."
echo "Les résultats sont sauvegardés dans $OUTPUT_FILE."

# Création du fichier minmax pour lv all
if [[ "$TYPE_STATION" == "lv" && "$TYPE_CONSOMMATEUR" == "all" ]]; then
    echo "Traitement des 10 postes LV les plus chargés et les moins chargés..."
    awk -F':' '{print $1 ":" $2 ":" $3 ":" $2-$3}' "$OUTPUT_FILE" | sort -t':' -k4n > "tmp/sorted_lv.csv"
    head -n 10 "tmp/sorted_lv.csv" > "tests/lv_all_minmax.csv"
    tail -n 10 "tmp/sorted_lv.csv" >> "tests/lv_all_minmax.csv"
    echo "Les résultats des postes LV sont dans tests/lv_all_minmax.csv."
fi

# Nettoyage des fichiers temporaires
rm -rf tmp/*

exit 0
