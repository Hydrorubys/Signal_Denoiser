
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

snr_values = [-5, 0, 5, 10, 15, 20]

plt.figure(figsize=(12, 6))

for snr in snr_values:
    filename = f"tests_snr/yin_note_{snr}dB.csv"

    data = pd.read_csv(filename)

    # Les valeurs -1 correspondent à une absence de détection
    detected = data["f0"] > 0

    plt.plot(
        data.loc[detected, "temps"],
        data.loc[detected, "f0"],
        label=f"SNR = {snr} dB"
    )

plt.axhline(81.5, linestyle="--", color="black",
            label="Fondamentale de référence")

plt.xlabel("Temps (s)")
plt.ylabel("Fréquence estimée (Hz)")
plt.title("Estimation de la fondamentale par YIN")
plt.ylim(50, 110)
plt.grid()
plt.legend()
plt.show()
