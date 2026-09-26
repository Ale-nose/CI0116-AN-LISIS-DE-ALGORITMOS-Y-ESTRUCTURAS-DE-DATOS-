#!/usr/bin/env python3
"""Section 6.1 check: a combat log loads without any prior transformation.

Usage: python3 tools/check_combat_log.py resultados.csv [more.csv ...]

Uses only the standard library, so it runs anywhere the report scripts
will. Several files (e.g. the 30 runs of Topic 7) can be checked at once.
"""
import csv
import sys

EXPECTED_COLUMNS = [
    "seed", "wave", "slot", "structure", "ticks", "max_size", "avg_size",
    "inserts", "erases", "queries", "steps_total", "steps_insert",
    "steps_erase", "steps_query", "comparisons", "pointer_hops", "shifts",
    "rotations", "real_us", "max_queue", "avg_queue", "empty_queue_ticks",
    "empty_queue_ratio", "effective_shots", "ghost_shots",
]
TEXT_COLUMNS = {"structure"}
DECIMAL_COLUMNS = {"avg_size", "real_us", "avg_queue", "empty_queue_ratio"}


def check_file(path):
    errors = []
    seen = set()
    rows = 0
    with open(path, newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        if reader.fieldnames != EXPECTED_COLUMNS:
            return [f"header mismatch: {reader.fieldnames}"], 0
        for line, row in enumerate(reader, start=2):
            rows += 1
            if None in row or None in row.values():
                errors.append(f"line {line}: wrong number of fields")
                continue
            values = {}
            for column in EXPECTED_COLUMNS:
                if column in TEXT_COLUMNS:
                    continue
                try:
                    values[column] = (float(row[column])
                                      if column in DECIMAL_COLUMNS
                                      else int(row[column]))
                except ValueError:
                    errors.append(f"line {line}: {column}={row[column]!r} "
                                  "is not a plain number")
            if len(values) != len(EXPECTED_COLUMNS) - len(TEXT_COLUMNS):
                continue
            key = (values["seed"], values["wave"], values["slot"])
            if key in seen:
                errors.append(f"line {line}: duplicate seed/wave/slot {key}")
            seen.add(key)
            by_category = (values["comparisons"] + values["pointer_hops"]
                           + values["shifts"] + values["rotations"])
            by_operation = (values["steps_insert"] + values["steps_erase"]
                            + values["steps_query"])
            if not values["steps_total"] == by_category == by_operation:
                errors.append(f"line {line}: steps_total={values['steps_total']}"
                              f" but categories={by_category}, "
                              f"operations={by_operation}")
    return errors, rows


def main(paths):
    if not paths:
        print(__doc__)
        return 2
    failed = False
    for path in paths:
        errors, rows = check_file(path)
        if errors:
            failed = True
            print(f"FAIL {path}: {len(errors)} problem(s)")
            for error in errors[:10]:
                print("   ", error)
        else:
            print(f"OK   {path}: {rows} rows, {len(EXPECTED_COLUMNS)} columns")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))