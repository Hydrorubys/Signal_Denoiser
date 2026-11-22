import soundfile as sf

for filename in ["bruit_mono.wav", "Media/note_La.mp3"]:
    info = sf.info(filename)

    print(f"\n{filename}")
    print("Fréquence :", info.samplerate, "Hz")
    print("Canaux :", info.channels)
    print("Durée :", info.duration, "s")
    print("Échantillons :", info.frames)
