"""Generates build/compile_commands.json, so editors can resolve the Arduino includes.

arduino-cli only lists the generated copies of the example sketches in its compilation
database. This script also adds entries for the original files (.ino, .cpp...), so they
get the same include paths and defines.
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parent.parent
EXAMPLES_DIR = ROOT_DIR / 'examples'
DATABASES_DIR = ROOT_DIR / 'build' / 'compile-commands'
OUTPUT_FILE = ROOT_DIR / 'build' / 'compile_commands.json'
DEFAULT_FQBN = 'Seeeduino:samd:seeed_XIAO_m0:usbstack=tinyusb'

# The Arduino build adds these to the main sketch file, which is plain C++ otherwise.
INO_EXTRA_ARGUMENTS = ['-include', 'Arduino.h', '-x', 'c++']


def find_sketches() -> list[Path]:
    """Returns the example sketch folders (the ones containing a .ino file with the same name)."""
    return sorted(path.parent for path in EXAMPLES_DIR.glob('**/*.ino') if path.stem == path.parent.name)


def retarget_entry(entry: dict, source: Path, extra_arguments: list[str]) -> dict:
    """Returns a copy of a database entry that compiles the given source file instead."""
    arguments = [str(source) if argument == entry['file'] else argument for argument in entry['arguments']]
    return {
        'directory': entry['directory'],
        'file': str(source),
        'arguments': [arguments[0], *extra_arguments, *arguments[1:]],
    }


def map_sketch_entries(entries: list[dict], sketch: Path, build_path: Path) -> list[dict]:
    """Copies the entries of the generated sketch files to the original files they come from."""
    generated_dir = build_path / 'sketch'
    main_file = generated_dir / f'{sketch.name}.ino.cpp'
    mapped_entries = []

    for entry in entries:
        generated_file = Path(entry['file'])
        if generated_file == main_file:
            # All the .ino files of a sketch are merged into the main generated file.
            for ino_file in sorted(sketch.glob('*.ino')):
                mapped_entries.append(retarget_entry(entry, ino_file, INO_EXTRA_ARGUMENTS))
            continue

        try:
            relative_path = generated_file.relative_to(generated_dir)
        except ValueError:
            # Library or core file, already listed with its original path.
            continue
        mapped_entries.append(retarget_entry(entry, sketch / relative_path, []))

    return mapped_entries


def generate_sketch_entries(sketch: Path, fqbn: str) -> list[dict] | None:
    """Returns the compilation database of a sketch including its original files, or None if arduino-cli failed."""
    build_path = DATABASES_DIR / sketch.relative_to(EXAMPLES_DIR)
    result = subprocess.run(
        [
            'arduino-cli', 'compile',
            '--fqbn', fqbn,
            '--library', str(ROOT_DIR),
            '--only-compilation-database',
            '--build-path', str(build_path),
            str(sketch),
        ],
    )
    if result.returncode != 0:
        return None
    entries = json.loads((build_path / 'compile_commands.json').read_text(encoding='utf-8'))
    return entries + map_sketch_entries(entries, sketch, build_path)


def main() -> int:
    """Generates the merged compilation database of all the example sketches."""
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--fqbn', default=DEFAULT_FQBN, help='board to configure the editor for (default: %(default)s)')
    args = parser.parse_args()

    if shutil.which('arduino-cli') is None:
        print('arduino-cli not found, make sure it is installed and in your PATH', file=sys.stderr)
        return 1

    sketches = find_sketches()
    if not sketches:
        print(f'No sketch found in {EXAMPLES_DIR}', file=sys.stderr)
        return 1

    # Library and core files are listed once per sketch, so keep only their first entry.
    entries_by_file = {}
    for sketch in sketches:
        sketch_entries = generate_sketch_entries(sketch, args.fqbn)
        if sketch_entries is None:
            return 1
        for entry in sketch_entries:
            entries_by_file.setdefault(os.path.normcase(entry['file']), entry)

    OUTPUT_FILE.write_text(json.dumps(list(entries_by_file.values()), indent=2), encoding='utf-8')
    print(f'Wrote {OUTPUT_FILE.relative_to(ROOT_DIR)} for {len(sketches)} sketch(es), board {args.fqbn}')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
