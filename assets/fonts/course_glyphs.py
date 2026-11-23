"""Check or regenerate the three LVGL bitmap fonts used on the course screens."""
import argparse
from pathlib import Path
import re
import shutil
import subprocess

HERE = Path(__file__).resolve().parent
MAIN = HERE.parents[1] / "main"
LVGL = Path("../../managed_components/lvgl__lvgl")
LATIN = LVGL / "src/libs/freetype/LiberationSans-Regular.ttf"
CHINESE = LVGL / "tests/src/test_files/fonts/noto/NotoSansSC-Regular.ttf"
FONTS = {
    "font_polish_20": (LATIN, 20),
    "font_cn_16": (CHINESE, 16),
    # The combining tilde in this face overlays the preceding IPA vowel in LVGL.
    "font_ipa_16": (LVGL / "scripts/built_in_font/DejaVuSans.ttf", 16),
}


def screen_glyphs():
    course = (MAIN / "polish_course.c").read_text(encoding="utf-8")
    ui = (MAIN / "polish_learn.c").read_text(encoding="utf-8")
    rows = re.findall(r'^\s*\{\s*"([^"]+)",\s*"([^"]+)",\s*"([^"]+)"\s*\},', course, re.M)
    if not rows:
        raise ValueError("No course cards found")
    polish, meanings, ipa = (set("".join(column)) for column in zip(*rows))
    chinese = set(chr(cp) for cp in range(32, 127)) | meanings
    for literal in re.findall(r'"([^"\n]*)"', ui + "\n" + course):
        if any('\u3400' <= c <= '\u9fff' for c in literal):
            chinese.update(literal)
    return {"font_polish_20": polish, "font_cn_16": chinese, "font_ipa_16": ipa}


def generate(glyphs):
    npx = shutil.which("npx")
    if not npx:
        raise SystemExit("Font generation requires Node.js and npx")
    for name, (font, size) in FONTS.items():
        if not (HERE / font).is_file():
            raise SystemExit(f"Missing {font.name}. Install the pinned LVGL component first.")
        subprocess.run([
            npx, "--yes", "lv_font_conv@1.5.3",
            "--font", font.as_posix(), "--symbols", "".join(sorted(glyphs[name])),
            "--size", str(size), "--bpp", "4", "--no-compress", "--no-prefilter",
            "--no-kerning", "--format", "lvgl", "--lv-include", "lvgl.h",
            "--output", name + ".c",
        ], cwd=HERE, check=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--generate", action="store_true", help="rebuild fonts from LVGL assets")
    args = parser.parse_args()
    glyphs = screen_glyphs()
    if args.generate:
        generate(glyphs)
    for name, needed in glyphs.items():
        source = (HERE / (name + ".c")).read_text(encoding="utf-8")
        available = {chr(int(cp, 16)) for cp in re.findall(r'/\* U\+([0-9a-fA-F]+) ', source)}
        missing = needed - available
        if missing:
            raise SystemExit(f"{name} missing glyphs: {''.join(sorted(missing))}")
        print(f"{name}: {len(needed)} required glyphs covered")


if __name__ == "__main__":
    main()
