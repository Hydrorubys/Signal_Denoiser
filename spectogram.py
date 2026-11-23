import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile

fe, signal = wavfile.read("under_the_bridge_filtre.wav")
fe1, signal1 = wavfile.read("Media/Under_the_bridge.wav")

fe0, signal0 = wavfile.read("Media/gammeLAm.wav")
feA, filtreA = wavfile.read("guitare_filtreA.wav")
feB, filtreB = wavfile.read("guitare_filtreB.wav")
feC, filtreC = wavfile.read("guitare_filtreC.wav")
feD, filtreD = wavfile.read("guitare_filtreD.wav")

plt.figure(figsize=(12, 6))

plt.specgram(
    signal,
    NFFT=1024,
    Fs=fe,
    noverlap=768,
    cmap="magma"
)

plt.ylim(0, 2000)
plt.xlabel("Temps (s)")
plt.ylabel("Fréquence (Hz)")
plt.title("Spectrogramme du signal original")
plt.colorbar(label="Puissance (dB)")

plt.show()

plt.figure(figsize=(12, 6))

plt.specgram(
    signal1,
    NFFT=1024,
    Fs=fe1,
    noverlap=768,
    cmap="magma"
)

plt.ylim(0, 1000)
plt.xlabel("Temps (s)")
plt.ylabel("Fréquence (Hz)")
plt.title("Spectrogramme du signal filtré")
plt.colorbar(label="Puissance (dB)")

plt.show()

plt.figure(figsize=(12, 6))

plt.specgram(
    filtreB,
    NFFT=1024,
    Fs=feB,
    noverlap=768,
    cmap="magma"
)

plt.ylim(0, 1000)
plt.xlabel("Temps (s)")
plt.ylabel("Fréquence (Hz)")
plt.title("Spectrogramme du signal filtré")
plt.colorbar(label="Puissance (dB)")

plt.show()

plt.figure(figsize=(12, 6))

plt.specgram(
    filtreC,
    NFFT=1024,
    Fs=feC,
    noverlap=768,
    cmap="magma"
)

plt.ylim(0, 1000)
plt.xlabel("Temps (s)")
plt.ylabel("Fréquence (Hz)")
plt.title("Spectrogramme du signal filtré")
plt.colorbar(label="Puissance (dB)")

plt.show()

plt.figure(figsize=(12, 6))

plt.specgram(
    filtreD,
    NFFT=1024,
    Fs=feD,
    noverlap=768,
    cmap="magma"
)

plt.ylim(0, 2000)
plt.xlabel("Temps (s)")
plt.ylabel("Fréquence (Hz)")
plt.title("Spectrogramme du signal filtré")
plt.colorbar(label="Puissance (dB)")

plt.show()
