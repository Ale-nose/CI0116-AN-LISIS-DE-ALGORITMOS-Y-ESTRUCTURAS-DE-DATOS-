#!/usr/bin/env python3
"""Topic 7.1: run every headless match the report needs and consolidate
the combat logs into one CSV per experiment.

Usage (from Proyecto1/, after `make` in src/):
    python3 tools/run_experiments.py [--seeds 30] [--jobs N]

Experiments (each over the same N seeds):
  normal_game.csv   8 structures, normal rules (defeat ends the match).
  load_curve.csv    8 structures, --ignore-defeat: all 20 waves under a
                    growing load (questions 1-4).
  hive_hash.csv     hash table, only Hive, default vs mixed hash (Q5);
  hive_buckets.csv  its bucket-length distributions (Q5).
  swarm_trees.csv   BST and AVL, only Swarm (Q6).

Only the standard library is used. Files go to report/data/.
"""
import argparse
import concurrent.futures
import os
import subprocess
import sys
import tempfile

STRUCTURES = ["linked_list", "sorted_list", "dynamic_array", "sorted_array",
              "bst", "avl", "min_heap", "hash_table"]
HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(HERE)
BINARY = os.path.join(PROJECT, "src", "bin", "Overflow")
OUT_DIR = os.path.join(PROJECT, "report", "data")
PROGRESS_EVERY = 50  # print progress every this many finished matches


def build_runs(seeds):
    """Every (experiment, extra_output, args) run, in a fixed order."""
    runs = []
    for seed in seeds:
        for core in STRUCTURES:
            runs.append(("normal_game", None, [
                "--seed", str(seed), "--core", core]))
            runs.append(("load_curve", None, [
                "--seed", str(seed), "--core", core, "--ignore-defeat"]))
        for hash_name in ["default", "mixed"]:
            runs.append(("hive_hash", "hive_buckets", [
                "--seed", str(seed), "--core", "hash_table", "--only",
                "hive", "--hash", hash_name, "--ignore-defeat"]))
        for core in ["bst", "avl"]:
            runs.append(("swarm_trees", None, [
                "--seed", str(seed), "--core", core, "--only", "swarm",
                "--ignore-defeat"]))
    return runs


def run_one(index, run, tmp_dir):
    name, extra, args = run
    out = os.path.join(tmp_dir, f"{index:05d}.csv")
    command = [BINARY, "--headless", *args, "--out", out]
    extra_out = None
    if extra:
        extra_out = os.path.join(tmp_dir, f"{index:05d}_extra.csv")
        command += ["--buckets-out", extra_out]
    result = subprocess.run(command, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"{' '.join(command)}\n{result.stderr}")
    return name, out, extra, extra_out


def append_csv(destination, source, write_header):
    with open(source, encoding="utf-8") as handle:
        lines = handle.readlines()
    with open(destination, "a", encoding="utf-8") as out:
        out.writelines(lines if write_header else lines[1:])


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--seeds", type=int, default=30)
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 1)
    options = parser.parse_args()

    if not os.path.exists(BINARY):
        sys.exit(f"Binary not found: {BINARY} (run `make` in src/ first)")

    runs = build_runs(range(1, options.seeds + 1))
    os.makedirs(OUT_DIR, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp_dir:
        with concurrent.futures.ThreadPoolExecutor(options.jobs) as pool:
            futures = [pool.submit(run_one, i, run, tmp_dir)
                       for i, run in enumerate(runs)]
            results = []
            for done, future in enumerate(futures, start=1):
                results.append(future.result())
                if done % PROGRESS_EVERY == 0 or done == len(runs):
                    print(f"{done}/{len(runs)} matches", flush=True)

        # Concatenate in run order so the output never depends on
        # which match happened to finish first.
        started = set()
        for name, out, extra, extra_out in results:
            for experiment, path in [(name, out), (extra, extra_out)]:
                if experiment is None:
                    continue
                destination = os.path.join(OUT_DIR, f"{experiment}.csv")
                if experiment not in started:
                    open(destination, "w").close()
                append_csv(destination, path, experiment not in started)
                started.add(experiment)

    for experiment in sorted(started):
        path = os.path.join(OUT_DIR, f"{experiment}.csv")
        with open(path, encoding="utf-8") as handle:
            rows = sum(1 for _ in handle) - 1
        print(f"  report/data/{experiment}.csv: {rows} rows")


if __name__ == "__main__":
    main()