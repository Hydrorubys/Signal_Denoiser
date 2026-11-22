
import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf

# Chargement du bruit
noise, fe = sf.read("signal_bruite.wav")

# Passage en mono si nécessaire
if noise.ndim == 2:
    noise = np.mean(noise, axis=1)

# FFT
N = len(noise)
spectrum = np.fft.rfft(noise)
frequencies = np.fft.rfftfreq(N, 1/fe)

# Spectre en amplitude
amplitude = np.abs(spectrum) / N

# Affichage
plt.figure(figsize=(12, 5))
plt.plot(frequencies, amplitude)
plt.xlim(0, 1000)
plt.xlabel("Fréquence (Hz)")
plt.ylabel("Amplitude")
plt.title("Spectre du bruit d'amplificateur")
plt.grid()
plt.show()
