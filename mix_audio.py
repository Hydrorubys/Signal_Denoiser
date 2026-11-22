import numpy as np
import soundfile as sf

FE = 44100

# Chargement des fichiers mono
signal, fe_s = sf.read("note_mono.wav")
bruit, fe_b = sf.read("bruit_mono.wav")

assert fe_s == FE and fe_b == FE

# Même durée : on tronque le bruit
N = min(len(signal), len(bruit))
signal = signal[:N]
bruit = bruit[:N]

# Puissances moyennes
Ps = np.mean(signal**2)
Pb = np.mean(bruit**2)

# SNR souhaité
SNR_cible = 10

# Coefficient de bruit
alpha = np.sqrt(Ps / (Pb * 10**(SNR_cible / 10)))

# Mélange
melange = signal + alpha * bruit

# SNR réellement obtenu
SNR_reel = 10 * np.log10(Ps / np.mean((alpha * bruit)**2))

# Vérification de l'écrêtage
amplitude_max = np.max(np.abs(melange))

print(f"Durée : {N / FE:.2f} s")
print(f"Puissance signal : {Ps:.6f}")
print(f"Puissance bruit : {Pb:.6f}")
print(f"Coefficient alpha : {alpha:.4f}")
print(f"SNR obtenu : {SNR_reel:.2f} dB")
print(f"Amplitude maximale : {amplitude_max:.4f}")

if amplitude_max > 1:
    print("Attention : écrêtage possible !")

# Export
sf.write("signal_bruite.wav", melange, FE, subtype="PCM_24")
