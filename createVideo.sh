#!/bin/bash

# Vérifie si un dossier d'images est spécifié
dossier=${1:-"."}  # Dossier par défaut est le répertoire courant
debut=${2:-0}       # Plage d'images, début par défaut à 0
fin=${3:-$(ls -1q $dossier/*.png | wc -l)} # Plage d'images, fin par défaut à la dernière image du dossier

# Extraire le nom du dossier sans le chemin complet
nom_dossier=$(basename "$dossier")

# Génère un fichier vidéo avec le nom du dossier comme nom de fichier de sortie
ffmpeg -framerate 60 -i "$dossier/%d.png" -start_number $debut -vframes $((fin - debut)) -c:v libx264 -pix_fmt yuv420p "$nom_dossier.mp4"

echo "Vidéo créée avec les images de $dossier, de $debut à $fin. Le fichier vidéo s'appelle $nom_dossier.mp4."
