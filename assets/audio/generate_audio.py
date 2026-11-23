"""Synthesize Polish course recordings with eSpeak NG and resample them with FFmpeg."""
import argparse
import subprocess
import tempfile
from pathlib import Path

from pack_audio import HERE, MANIFEST, course_rows, read_clip


def synthesize(word, native_wav, wav):
    # Pin the built-in Polish voice and the random seed for reproducible recordings.
    subprocess.run(
        ["espeak-ng", "-D", "-b", "1", "-v", "pl", "-s", "145", "-a", "90",
         "-z", "-w", native_wav, "--", word],
        check=True,
    )
    subprocess.run(
        ["ffmpeg", "-nostdin", "-v", "error", "-y", "-i", native_wav, "-ar", "11025",
         "-ac", "1", "-c:a", "pcm_s16le", "-fflags", "+bitexact", wav],
        check=True,
    )
    read_clip(wav)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--all", action="store_true", help="also regenerate existing course clips")
    args = parser.parse_args()
    words = MANIFEST.read_text(encoding="utf-8").splitlines()
    ids = {word: i for i, word in enumerate(words)}
    if len(ids) != len(words):
        raise ValueError("Duplicate recording name in words.txt")
    raw = HERE / "raw"
    raw.mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(dir=HERE) as tmp:
        native_wav = Path(tmp) / "native.wav"
        wav = Path(tmp) / "clip.wav"
        for word, _, _ in course_rows():
            index = ids.get(word, len(words))
            destination = raw / f"{index:03d}.wav"
            if not args.all and word in ids and destination.is_file():
                continue
            synthesize(word, native_wav, wav)
            wav.replace(destination)
            if word not in ids:
                ids[word] = index
                words.append(word)
                manifest = Path(tmp) / "words.txt"
                manifest.write_text("\n".join(words) + "\n", encoding="utf-8")
                manifest.replace(MANIFEST)
            print(f"{destination.name}  {word}", flush=True)


if __name__ == "__main__":
    main()
