"""Pack indexed recordings into the firmware in the current course order."""
import argparse
import re
import struct
import wave
from pathlib import Path

HERE = Path(__file__).resolve().parent
COURSE = HERE.parents[1] / "main" / "polish_course.c"
MANIFEST = HERE / "words.txt"


def course_rows():
    source = COURSE.read_text(encoding="utf-8")
    table = source.split("polish_card_t cards[] = {", 1)[1].split("};", 1)[0]
    rows = []
    for line in table.splitlines():
        line = line.strip()
        if not line or line.startswith("//"):
            continue
        match = re.fullmatch(r'\{\s*"([^"]+)",\s*"([^"]+)",\s*"([^"]+)"\s*\},', line)
        if not match:
            raise ValueError(f"Invalid course row: {line}")
        rows.append(match.groups())
    if not rows:
        raise ValueError("Empty course")
    return rows


def read_clip(path):
    with wave.open(str(path), "rb") as wav:
        if (wav.getnchannels(), wav.getsampwidth(), wav.getframerate()) != (1, 2, 11025):
            raise ValueError(f"{path}: expected 11025 Hz mono PCM16")
        pcm = wav.readframes(wav.getnframes())
        if not pcm or len(pcm) != wav.getnframes() * 2 or not any(pcm):
            raise ValueError(f"{path}: empty, silent or incomplete clip")
        return pcm


def pack(output):
    words = MANIFEST.read_text(encoding="utf-8").splitlines()
    recordings = {word: HERE / "raw" / f"{i:03d}.wav" for i, word in enumerate(words)}
    if len(recordings) != len(words):
        raise ValueError("Duplicate recording name in words.txt")
    course = course_rows()
    # An offset for each clip plus a sentinel. All PCM samples remain aligned.
    offsets = [4 * (len(course) + 1)]
    bank = bytearray()
    for word, _, _ in course:
        if word not in recordings:
            raise ValueError(f"Missing pronunciation for {word!r}")
        pcm = read_clip(recordings[word])
        bank.extend(pcm)
        offsets.append(offsets[-1] + len(pcm))
    output.write_bytes(struct.pack(f"<{len(offsets)}I", *offsets) + bank)
    print(f"Packed {len(course)} recordings ({len(bank):,} bytes PCM)")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output", type=Path)
    pack(parser.parse_args().output)
