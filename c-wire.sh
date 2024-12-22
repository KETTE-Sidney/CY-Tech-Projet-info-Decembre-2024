#!/bin/bash

# Fonction pour afficher l'aide
aide() {
    echo "Utilisation: $0 <data_file> <station_type> <consumer_type> [central_id]"
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

# Afficher l'aide sur demande
if [[ "$1" == "-h" ]]; then
    aide
fi

# Vérification du nombre d'arguments
if [[ $# -lt 3 ]]; then
    echo "Erreur : Arguments manquants."
    aide
fi

# Récupération des paramètres
DATA_FILE=$1
STATION_TYPE=$2
CONSUMER_TYPE=$3
CENTRAL_ID=${4:-"all"} # Défaut à "all" si non spécifié

# Vérification du fichier d'entrée
if [[ ! -f "$DATA_FILE" ]]; then
    echo "Erreur : Fichier d'entrée '$DATA_FILE' introuvable."
    exit 1
fi

# Vérification du type de station
if [[ "$STATION_TYPE" != "hvb" && "$STATION_TYPE" != "hva" && "$STATION_TYPE" != "lv" ]]; then
    echo "Erreur : Type de station invalide '$STATION_TYPE'. Doit être l'un des suivants : hvb, hva, lv."
    exit 1
fi

# Vérification du type de consommateur
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

# Exécution du programme C
./codeC/projetwire "$FILTERED_FILE" "$OUTPUT_FILE"

# Afficher un message de succès
echo "Traitement terminé. Résultats enregistrés dans : $OUTPUT_FILE."

