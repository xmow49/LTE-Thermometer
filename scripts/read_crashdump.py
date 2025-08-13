#!/usr/bin/env python3
"""
Script pour décompresser et analyser un coredump ESP32
Usage: python3 read_crashdump.py crash.txt
"""

import sys
import base64
import zlib
import json
import subprocess
import tempfile
import os

def decompress_coredump(input_file):
    """
    Décompresse un coredump compressé et encodé en base64
    """
    try:
        with open(input_file, 'r') as f:
            data = f.read().strip()
        
        # Vérifier si c'est du JSON ou du base64 brut
        if data.startswith('{'):
            # C'est du JSON, extraire le champ coredump
            json_data = json.loads(data)
            base64_data = json_data.get('coredump', '')
            if not base64_data:
                print("Erreur: Pas de champ 'coredump' trouvé dans le JSON")
                return None
        else:
            # C'est du base64 brut
            base64_data = data
        
        print("Décodage base64...")
        compressed_data = base64.b64decode(base64_data)
        print(f"Taille des données compressées: {len(compressed_data)} bytes")
        
        print("Décompression zlib...")
        decompressed_data = zlib.decompress(compressed_data)
        print(f"Taille des données décompressées: {len(decompressed_data)} bytes")
        
        return decompressed_data
        
    except Exception as e:
        print(f"Erreur lors de la décompression: {e}")
        return None

def analyze_coredump(decompressed_data):
    """
    Analyse le coredump avec idf.py coredump-info
    """
    try:
        # Créer un fichier temporaire pour le coredump décompressé
        with tempfile.NamedTemporaryFile(mode='wb', suffix='.bin', delete=False) as temp_file:
            temp_file.write(decompressed_data)
            temp_file_path = temp_file.name
        
        print(f"Fichier temporaire créé: {temp_file_path}")
        print("Analyse du coredump avec idf.py...")
        print("=" * 60)
        
        # Exécuter idf.py coredump-info
        result = subprocess.run(
            ['idf.py', 'coredump-info', '-c', temp_file_path],
            capture_output=True,
            text=True,
            cwd='/workspaces/LTE-Thermometer'
        )
        
        # Afficher la sortie
        if result.stdout:
            print("STDOUT:")
            print(result.stdout)
        
        if result.stderr:
            print("STDERR:")
            print(result.stderr)
        
        if result.returncode != 0:
            print(f"Erreur: idf.py a retourné le code {result.returncode}")
        
        # Nettoyer le fichier temporaire
        os.unlink(temp_file_path)
        print(f"Fichier temporaire supprimé: {temp_file_path}")
        
    except Exception as e:
        print(f"Erreur lors de l'analyse: {e}")

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 read_crashdump.py <crash_file>")
        print("Exemple: python3 read_crashdump.py crash.txt")
        sys.exit(1)
    
    input_file = sys.argv[1]
    
    if not os.path.exists(input_file):
        print(f"Erreur: Le fichier '{input_file}' n'existe pas")
        sys.exit(1)
    
    print(f"Lecture du fichier: {input_file}")
    
    # Décompresser le coredump
    decompressed_data = decompress_coredump(input_file)
    if decompressed_data is None:
        sys.exit(1)
    
    # Analyser le coredump
    analyze_coredump(decompressed_data)

if __name__ == "__main__":
    main()
