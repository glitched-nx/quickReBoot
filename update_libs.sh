#!/bin/bash
################################################################################
# Datei: update_libs.sh
# Autor: ppkantorski
# Beschreibung: 
#   Dieses Skript automatisiert den Prozess der Aktualisierung der "libultra" 
#   und "libtesla" Bibliotheken. Es führt die folgenden Schritte aus:
#   - Navigiert in das Verzeichnis, in dem sich das Skript befindet.
#   - Entfernt vorhandene "libultra" und "libtesla" Verzeichnisse im "/lib" Ordner.
#   - Initialisiert ein temporäres Git-Repository, um selektiv nur die notwendigen 
#     Verzeichnisse aus dem Ultrahand-Overlay-Repository mittels Sparse Checkout zu klonen.
#   - Verschiebt die heruntergeladenen Verzeichnisse an die richtigen Stellen im 
#     "/lib" Ordner des Projekts.
#   - Bereinigt temporäre Dateien und Verzeichnisse.
#
# Verwendung:
#   1. Platzieren Sie dieses Skript im Stammverzeichnis des Overlay-Projekts.
#   2. Stellen Sie sicher, dass das Skript Ausführungsrechte hat:
#      chmod +x update_libs.sh
#   3. Führen Sie das Skript aus:
#      ./update_libs.sh
#
# Hinweise:
#   - Stellen Sie sicher, dass Sie eine aktive Internetverbindung haben, da dieses 
#     Skript aus einem entfernten Git-Repository zieht.
#   - Erfordert, dass Git installiert und richtig konfiguriert ist.
#
# Lizenz:
#   Dieses Skript wurde für die Overlay-Entwicklung von ppkantorski erstellt und wird 
#   unter der GPLv2-Lizenz verteilt.
################################################################################

# Navigiert in das Verzeichnis, in dem sich dieses Skript befindet
cd "$(dirname "$0")"

# Erstellt das /lib Verzeichnis, falls es nicht existiert
mkdir -p lib

# Entfernt vorhandene libultra und libtesla Verzeichnisse, falls sie existieren
rm -rf lib/libultra
rm -rf lib/libtesla

# Initialisiert ein neues Git-Repository
git init temp-lib

# Fügt das Ultrahand-Overlay-Repository als Remote hinzu
cd temp-lib
git remote add -f origin https://github.com/ppkantorski/Ultrahand-Overlay.git

# Aktiviert Sparse-Checkout
git config core.sparseCheckout true

# Gibt die Verzeichnisse an, die ausgecheckt werden sollen
echo "lib/libultra/*" >> .git/info/sparse-checkout
echo "lib/libtesla/*" >> .git/info/sparse-checkout

# Checkt die angegebenen Verzeichnisse aus
git pull origin main

# Verschiebt die Verzeichnisse an den richtigen Ort
cd ..
mkdir -p lib/libultra lib/libtesla
mv temp-lib/lib/libultra/* lib/libultra/
mv temp-lib/lib/libtesla/* lib/libtesla/

# Bereinigt temporäre Dateien und Verzeichnisse
rm -rf temp-lib
