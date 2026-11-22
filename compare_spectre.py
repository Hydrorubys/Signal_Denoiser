
import numpy as np
import soundfile as sf
import matplotlib.pyplot as plt

FE = 44100
N = 2048
HOP = N // 2

# Lecture du bruit
audio, fe = sf.read("bruit_mono.wav")

# Fenêtre de Hann
window = np.hanning(N)

# Calcul du spectrogramme
spectrogram = []

for start in range(0, len(audio) - N + 1, HOP):
    segment = audio[start:start + N]
    fft = np.fft.rfft(segment * window)

    magnitude = np.abs(fft)
    spectrogram.append(magnitude)

spectrogram = np.array(spectrogram)

# Axes
times = np.arange(len(spectrogram)) * HOP / FE
frequencies = np.fft.rfftfreq(N, 1 / FE)

# Affichage
plt.figure(figsize=(12, 6))

plt.pcolormesh(
    times,
    frequencies,
    20 * np.log10(spectrogram.T + 1e-10),
    shading="auto"
)

plt.ylim(0, 1000)
plt.xlabel("Temps (s)")
plt.ylabel("Fréquence (Hz)")
plt.title("Spectrogramme du bruit d'ampli")
plt.colorbar(label="Amplitude (dB)")
plt.show()
