
import librosa
import soundfile as sf
import numpy as np

FE = 44100

def prepare_file(input_file, output_file):
    # Chargement et rééchantillonnage
    audio, fe = librosa.load(
        input_file,
        sr=FE,
        mono=False
    )

    # Conversion stéréo vers mono
    if audio.ndim == 2:
        audio = np.mean(audio, axis=0)

    # Export WAV mono
    sf.write(output_file, audio, FE, subtype="PCM_24")

    print(f"\nFichier : {input_file}")
    print(f"Fréquence : {FE} Hz")
    print(f"Durée : {len(audio) / FE:.2f} s")
    print(f"Nombre d'échantillons : {len(audio)}")

    power = np.mean(audio**2)
    rms = np.sqrt(power)

    print(f"Puissance moyenne : {power:.8f}")
    print(f"Valeur RMS : {rms:.6f}")

    print(f"Amplitude maximale : {np.max(np.abs(audio)):.4f}")


prepare_file("Media/bruit_ampli.wav", "bruit_mono.wav")
prepare_file("Media/note_La.mp3", "note_mono.wav")
