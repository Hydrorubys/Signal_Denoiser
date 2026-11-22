
import numpy as np
import soundfile as sf
from pathlib import Path

# Fichiers sources
SIGNAL_FILE = "note_mono.wav"
NOISE_FILE = "bruit_mono.wav"

# SNR à tester (en dB)
SNR_VALUES = [-5, 0, 5, 10, 15, 20]

# Lecture des fichiers
signal, fe_signal = sf.read(SIGNAL_FILE)
noise, fe_noise = sf.read(NOISE_FILE)

if fe_signal != fe_noise:
    raise ValueError("Les fréquences d'échantillonnage sont différentes.")

# On utilise la durée du signal de guitare
N = len(signal)

if len(noise) < N:
    raise ValueError("Le fichier de bruit est trop court.")

# Même durée pour les deux signaux
noise = noise[:N]

# Puissances moyennes
Ps = np.mean(signal**2)
Pb = np.mean(noise**2)

print(f"Puissance du signal : {Ps:.8f}")
print(f"Puissance du bruit  : {Pb:.8f}")

# Création du dossier de sortie
Path("tests_snr").mkdir(exist_ok=True)

for snr in SNR_VALUES:

    # Calcul du coefficient de bruit
    alpha = np.sqrt(Ps / (Pb * 10**(snr / 10)))

    # Mélange
    mixed = signal + alpha * noise

    # Vérification du SNR obtenu
    measured_snr = 10 * np.log10(
        Ps / np.mean((alpha * noise)**2)
    )

    # Export
    filename = f"tests_snr/note_{snr}dB.wav"
    sf.write(filename, mixed, fe_signal, subtype="PCM_24")

    print(
        f"SNR demandé : {snr:>3} dB | "
        f"SNR obtenu : {measured_snr:>6.2f} dB | "
        f"alpha : {alpha:.4f}"
    )
