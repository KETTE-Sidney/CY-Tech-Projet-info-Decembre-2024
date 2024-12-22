#!/bin/bash

# Fonction pour afficher l'aide
show_help() {
    echo "Usage: $0 <data_file> <station_type> <consumer_type> [central_id]"
    echo
    echo "Arguments:"
    echo "  data_file       Chemin vers le fichier CSV d'entrée (obligatoire)"
    echo "  station_type    Type de station à traiter (obligatoire) : hvb, hva, lv"
    echo "  consumer_type   Type de consommateur à traiter (obligatoire) : comp, indiv, all"
    echo "                  Remarque : hvb indiv, hvb all, hva indiv, hva all ne sont pas autorisés."
    echo "  central_id      Optionnel : Filtrer les résultats par ID central."
    echo
    echo "Options:"
    echo "  -h              Afficher ce message d'aide."
    echo
    echo "Exemples:"
    echo "  $0 input/data.csv lv all"
    echo "  $0 input/data.csv hva comp 1"
    exit 0
}

# Afficher l'aide si demandé
if [[ "$1" == "-h" ]]; then
    show_help
fi

# Vérification du nombre d'arguments
if [[ $# -lt 3 ]]; then
    echo "Erreur : Arguments manquants."
    show_help
fi

# Affectation des arguments à des variables
DATA_FILE=$1
STATION_TYPE=$2
CONSUMER_TYPE=$3
CENTRAL_ID=${4:-"all"} # Défaut à "all" si non spécifié

# Validation du fichier d'entrée
if [[ ! -f "$DATA_FILE" ]]; then
    echo "Erreur : Fichier d'entrée '$DATA_FILE' introuvable."
    exit 1
fi

# Validation du type de station
if [[ "$STATION_TYPE" != "hvb" && "$STATION_TYPE" != "hva" && "$STATION_TYPE" != "lv" ]]; then
    echo "Erreur : Type de station invalide '$STATION_TYPE'. Doit être l'un des suivants : hvb, hva, lv."
    exit 1
fi

# Validation du type de consommateur
if [[ "$CONSUMER_TYPE" != "comp" && "$CONSUMER_TYPE" != "indiv" && "$CONSUMER_TYPE" != "all" ]]; then
    echo "Erreur : Type de consommateur invalide '$CONSUMER_TYPE'. Doit être l'un des suivants : comp, indiv, all."
    exit 1
fi

# Vérification des combinaisons invalides de type de station et de consommateur
if { [[ "$STATION_TYPE" == "hvb" || "$STATION_TYPE" == "hva" ]] && [[ "$CONSUMER_TYPE" == "all" || "$CONSUMER_TYPE" == "indiv" ]]; }; then
    echo "Erreur : Type de consommateur '$CONSUMER_TYPE' n'est pas autorisé avec le type de station '$STATION_TYPE'."
    exit 1
fi

# Création des répertoires nécessaires
mkdir -p tmp tests graphs

# Filtrage des données
FILTERED_FILE="tmp/filtered_data.csv"
awk -F ";" -v station="$STATION_TYPE" -v consumer="$CONSUMER_TYPE" -v central="$CENTRAL_ID" '
BEGIN { OFS = ":" }
{
    if (central == "all" || $1 == central) {
        if (station == "hvb" && consumer == "comp" && $2 != "-") {
            print $2, $7, $8
        } else if (station == "hva" && consumer == "comp" && $3 != "-") {
            print $3, $7, $8
        } else if (station == "lv" && consumer == "comp" && $4 != "-" && ($5 != "-" || $7 != "-")) {
            print $4, $7, $8
        } else if (station == "lv" && consumer == "indiv" && $4 != "-" && ($6 != "-" || $7 != "-")) {
            print $4, $7, $8
        } else if (station == "lv" && consumer == "all" && $4 != "-") {
            print $4, $7, $8
        }
    }
}' "$DATA_FILE" > "$FILTERED_FILE"

# Vérification si le fichier filtré est vide
if [[ ! -s "$FILTERED_FILE" ]]; then
    echo "Erreur : Aucune donnée trouvée correspondant aux filtres."
    exit 1
fi

# Préparation du nom du fichier de sortie
OUTPUT_FILE="tests/${STATION_TYPE}_${CONSUMER_TYPE}"
if [[ "$CENTRAL_ID" != "all" ]]; then
    OUTPUT_FILE="${OUTPUT_FILE}_${CENTRAL_ID}"
fi
OUTPUT_FILE="${OUTPUT_FILE}.csv"

# Compilation du programme C si nécessaire
cd codeC
if [[ ! -f main ]]; then
    echo "Compilation du programme C..."
    make
    if [[ $? -ne 0 ]]; then
        echo "Erreur : La compilation a échoué."
        exit 1
    fi
fi
cd ..

# Appel du programme C
./codeC/main "$FILTERED_FILE" "$OUTPUT_FILE" "$STATION_TYPE"

# Gestion spéciale pour lv all avec graphique et minmax
if [[ "$STATION_TYPE" == "lv" && "$CONSUMER_TYPE" == "all" ]]; then
    MINMAX_FILE="tests/lv_all_minmax.csv"
    GRAPH_FILE="graphs/lv_all_minmax.png"

    echo "StationID:Capacity:Consumption" > "$MINMAX_FILE"

    # Extraire les 10 postes LV avec la consommation la plus élevée
    tail -n +2 "$OUTPUT_FILE" | sort -t: -k3nr | head -n 10 >> "$MINMAX_FILE"

    # Extraire les 10 postes LV avec la consommation la plus faible
    tail -n +2 "$OUTPUT_FILE" | sort -t: -k3n | head -n 10 >> "$MINMAX_FILE"

    # Générer un graphique avec GnuPlot
    gnuplot -e "
    set terminal png size 1000,700;
    set output '$GRAPH_FILE';
    set title 'Top and Bottom 10 LV Stations by Consumption';
    set xlabel 'Station ID';
    set ylabel 'Energy (kWh)';
    set style data histogram;
    set style histogram cluster gap 1;
    set style fill solid border -1;
    set boxwidth 0.8;
    plot '$MINMAX_FILE' using 2:xtic(1) title 'Capacity', \
         '' using 3 title 'Consumption';
    "
    echo "Graphique généré : $GRAPH_FILE"
fi

# Afficher un message de succès
echo "Traitement terminé. Résultats enregistrés dans : $OUTPUT_FILE."

