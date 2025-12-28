#!/usr/bin/env python3
"""
Convert Logisim 'v3.0 hex words addressed' .hex into a .txt file:
one 32-bit word per line, 8 hex digits, lowercase (e.g., 01400513).

Example:
  python3 hex_to_txt.py mem.hex instruction.txt --start 0 --count 6
  python3 hex_to_txt.py mem.hex all_words.txt
"""

import argparse
import re
from pathlib import Path

HEADER_RE = re.compile(r'^\s*v3\.0\s+hex\s+words\s+addressed\s*$', re.IGNORECASE)
LINE_RE   = re.compile(r'^\s*([0-9a-fA-F]+)\s*:\s*(.*?)\s*$')
WORD_RE   = re.compile(r'\b[0-9a-fA-F]{1,8}\b')  # Logisim words are 1..8 hex digits

def parse_logisim_hex_words(path: Path) -> dict[int, int]:
    """
    Returns a dict: word_address -> 32-bit word value.
    Word addresses are in *words* (not bytes), as in the Logisim format.
    """
    mem: dict[int, int] = {}
    with path.open("r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()

    # Optionally skip the header line if present
    i = 0
    if lines and HEADER_RE.match(lines[0]):
        i = 1

    for line in lines[i:]:
        line = line.strip()
        if not line:
            continue

        m = LINE_RE.match(line)
        if not m:
            # Not an address line -> ignore (robust against comments/garbage)
            continue

        base_addr_hex = m.group(1)
        data_part = m.group(2)

        base_addr = int(base_addr_hex, 16)  # word address
        words = WORD_RE.findall(data_part)

        for offset, w in enumerate(words):
            val = int(w, 16) & 0xFFFF_FFFF
            mem[base_addr + offset] = val

    return mem

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("input_hex", type=Path, help="Logisim .hex input (v3.0 hex words addressed)")
    ap.add_argument("output_txt", type=Path, help="Output .txt (one word per line)")
    ap.add_argument("--start", type=lambda s: int(s, 0), default=0,
                    help="Start word address (decimal or 0x...). Default: 0")
    ap.add_argument("--count", type=int, default=None,
                    help="How many words to write. Default: write all from start to max addr")
    ap.add_argument("--fill-missing", action="store_true",
                    help="If there are holes, fill them with 00000000 instead of skipping.")
    args = ap.parse_args()

    mem = parse_logisim_hex_words(args.input_hex)
    if not mem:
        raise SystemExit("No words parsed. Is the input file in Logisim 'v3.0 hex words addressed' format?")

    start = args.start
    max_addr = max(mem.keys())

    if args.count is None:
        end = max_addr
    else:
        end = start + args.count - 1

    out_lines = []
    for addr in range(start, end + 1):
        if addr in mem:
            out_lines.append(f"{mem[addr]:08x}")
        else:
            if args.fill_missing:
                out_lines.append("00000000")
            # else: skip missing addresses

    args.output_txt.write_text("\n".join(out_lines) + ("\n" if out_lines else ""), encoding="utf-8")
    print(f"Wrote {len(out_lines)} lines to {args.output_txt}")

if __name__ == "__main__":
    main()