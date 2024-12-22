#!/bin/bash

# Shell Script for the C-Wire Project
# Usage: ./c-wire.sh csv_file_path station_type consumer_type [plant_id]

# index fonction aide
aide() {
    echo "Usage: $0 <data_file> <station_type> <consumer_type> [central_id]"
    echo
    echo "Arguments:"
    echo "  data_file       Chemin vers le fichier CSV d'entrée (obligatoire)"
    echo "  station_type    Type de station à traiter (obligatoire):"
    echo "                  hvb, hva, lv"
    echo "  consumer_type   Type de consommateur à traiter (obligatoire):"
    echo "                  comp, indiv, all"
    echo "                  Remarque : hvb all, hvb indiv, hva all, hva indiv ne sont pas autorisés."
    echo "  central_id      Optionnel : Filtrer les résultats par ID central."
    echo
    echo "Options:"
    echo "  -h              Afficher ce message d'aide."
    echo
    echo "Examples:"
    echo "  $0 input/data.csv lv all"
    echo "  $0 input/data.csv hva comp 1"
    exit 0
}

# verification de demande d'aide
if [[ "$1" == "-h" ]]; then
    aide
fi

start=$(date +%s)

# verification des parametres
if [ "$#" -lt 3 ]; then
    echo "Erreur: nombre de parametre insuffisant."
    echo "Usage: ./c-wire.sh csv_file_path station_type consumer_type [plant_id]"
    exit 1
fi

# Recuperation des parametres
csv_file=$1
station_type=$2
consumer_type=$3
plant_id=${4:-"all"} # mettre à all par defaut si non fourni

# Verification du fichier d'entrée
if [ ! -f "$csv_file" ]; then
    echo "Error: le fichier $csv_file n'existe pas."
    exit 1
fi

# Verification du type de station
if [[ "$station_type" != "hvb" && "$station_type" != "hva" && "$station_type" != "lv" ]]; then
    echo "Erreur: Type de station invalide. Les options valides sont: hvb, hva, lv."
    exit 1
fi

# Verification du type de consommateur
if [[ "$consumer_type" != "comp" && "$consumer_type" != "indiv" && "$consumer_type" != "all" ]]; then
    echo "Erreur: Type de consommateur invalide. Les options valides sont: comp, indiv, all."
    exit 1
fi

# verification Plant ID (si fourni)
if [ "$plant_id" != "-1" ]; then
    echo "Checking plant ID: $plant_id"
    valid_id=$(awk -F';' -v id="$plant_id" '$1 == id {print $1; exit}' "$csv_file")
    if [ -z "$valid_id" ]; then
        echo "Erreur: plant ID $plant_id n'existe pas dans le fichier."
        exit 1
    fi
fi

# Interdiction de certaines options
if [[ "$station_type" == "hvb" && ( "$consumer_type" == "all" || "$consumer_type" == "indiv" ) ]]; then
    echo "Erreur: Options hvb all ou hvb indiv non autorise."
    exit 1
fi

if [[ "$station_type" == "hva" && ( "$consumer_type" == "all" || "$consumer_type" == "indiv" ) ]]; then
    echo "Erreur: Options hva all or hva indiv non autorise."
    exit 1
fi

# Creation des dossiers necessaires (entrée & sortie)
mkdir -p fil rst

# remove du dossier tmp
rm -f fil/*
rm -f rst/*

# Creation des fichiers temporaires
if [ "$plant_id" == "all" ]; then
    filtered_file="fil/filter_${station_type}_${consumer_type}.csv"
else
    filtered_file="fil/filter_${station_type}_${consumer_type}_${plant_id}.csv"
fi

# Filtrage des données du fichiers CSV
awk -F';' -v station="$station_type" -v consumer="$consumer_type" -v plant="$plant_id" '
BEGIN { OFS=";" }
{
    if (station == "hvb" && $2 != "-" && $6 == "-" && (consumer == "comp" || consumer == "all")) {
        if (plant == "-1" || $1 == plant) print $0;
    }
    else if (station == "hva" && $3 != "-" && $6 == "-" && (consumer == "comp" || consumer == "all")) {
        if (plant == "-1" || $1 == plant) print $0;
    }
    else if (station == "lv" && $4 != "-" && $1 != "Power plant" && (consumer == "comp" || consumer == "indiv" || consumer == "all")) {
        if (plant == "-1" || $1 == plant) print $0;
    }
}' "$csv_file" > "$filtered_file"

if [ ! -s "$filtered_file" ]; then
    echo "Erreur: Aucune donnee trouve pour ce parametre."
    exit 1
fi

echo "Donnees filtrees sauvegardees dans $filtered_file"

# Prepare du fichier de sortie
OUTPUT_FILE="tests/${STATION_TYPE}_${CONSUMER_TYPE}"
if [[ "$CENTRAL_ID" != "all" ]]; then
    OUTPUT_FILE="${OUTPUT_FILE}_${CENTRAL_ID}"
fi
OUTPUT_FILE="${OUTPUT_FILE}.csv"

# Verification et compilation du programme C
if [ ! -f projetwire ]; then
    echo "Compilation du programme C..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur: echec de compilation du programme C."
        exit 1
    fi
fi

# Execution du programme C
echo "Execution du programme..."
./projetwire "$station_type" "$consumer_type" "$plant_id"
if [ $? -ne 0 ]; then
    echo "Erreur: Echec d'execution C program."
    exit 1
fi

echo "Traitement termine."

end=$(date +%s)
duration=$((end - start))
echo "Temps d'execution: $duration s"
