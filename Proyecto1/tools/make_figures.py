#!/usr/bin/env python3
"""Topic 7.2: turn the consolidated combat logs into the report's figures,
tables and numbers.

Usage (from Proyecto1/, after tools/run_experiments.py):
    python3 tools/make_figures.py

Reads  report/data/*.csv
Writes report/figures/*.pdf   one figure per question (included by LaTeX)
       report/generated/*.tex tables and a \\dato{key} lookup of every
                              number quoted in the text, so re-running the
                              experiments and this script keeps the report
                              consistent without editing it by hand.

Requires pandas, numpy and matplotlib.
"""
import math
import os
import re

import matplotlib

matplotlib.use("Agg")  # no display needed: figures go straight to PDF
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402
import pandas as pd  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
PROJECT = os.path.dirname(HERE)
DATA_DIR = os.path.join(PROJECT, "report", "data")
FIG_DIR = os.path.join(PROJECT, "report", "figures")
GEN_DIR = os.path.join(PROJECT, "report", "generated")
TICKS_HEADER = os.path.join(PROJECT, "include", "core", "Ticks.hpp")

# Display order and Spanish names used in every figure and table.
STRUCTURES = ["linked_list", "sorted_list", "dynamic_array", "sorted_array",
              "bst", "avl", "min_heap", "hash_table"]
NAMES = {
    "linked_list": "Lista enlazada",
    "sorted_list": "Lista ordenada",
    "dynamic_array": "Arreglo dinámico",
    "sorted_array": "Arreglo ordenado",
    "bst": "ABB",
    "avl": "AVL",
    "min_heap": "Montículo mínimo",
    "hash_table": "Tabla hash",
}
COLORS = dict(zip(STRUCTURES, plt.get_cmap("tab10").colors))

# Theoretical cost of one maintenance operation (insert or erase) as a
# function of registry size n, for the workload the game produces: ids
# arrive in increasing order and the oldest enemies leave first.
MODELS = {
    "const": ("O(1)", lambda n: np.zeros_like(n, dtype=float)),
    "log": (r"O(\log n)", lambda n: np.log2(n + 1.0)),
    "linear": ("O(n)", lambda n: np.asarray(n, dtype=float)),
}
THEORY = {
    "linked_list": "linear",    # erase walks to the oldest, at the tail
    "sorted_list": "linear",    # insert walks to the end (largest id)
    "dynamic_array": "linear",  # erase is a linear search
    "sorted_array": "linear",   # binary search, but shifting is O(n)
    "bst": "linear",            # increasing ids degenerate it to a chain
    "avl": "log",               # height stays O(log n)
    "min_heap": "log",          # the oldest (erased) is the min, the root
    "hash_table": "const",      # O(1) expected per bucket
}

# Size bins (log-spaced) used to average cost over many tower-waves.
SIZE_BINS = [0, 1, 2, 4, 8, 16, 32, 64, 128]
# A wave counts as "keeping up" (demand fully served) above this ratio of
# ticks with an empty queue, and as "saturated" below the second one.
KEEPING_UP_RATIO = 0.95
SATURATED_RATIO = 0.5
# Question 3: waves compared side by side (load just before and while
# both structures saturate).
Q3_WAVES = [14, 15, 16, 17]
# Question 5: only buckets snapshotted when the table was this big or
# bigger, so the distribution isn't dominated by near-empty tables.
BUCKET_MIN_SIZE = 40

plt.rcParams.update({
    "font.size": 9,
    "axes.titlesize": 9,
    "axes.labelsize": 9,
    "legend.fontsize": 8,
    "figure.dpi": 150,
    "axes.grid": True,
    "grid.alpha": 0.3,
})


# --------------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------------

def read_constant(name):
    """Read an integer constexpr from Ticks.hpp, so the prediction uses the
    same budget the game does.

    Args:
        name: Constant name, e.g. "STEPS_PER_TICK".
    Returns:
        Its integer value.
    """
    with open(TICKS_HEADER, encoding="utf-8") as handle:
        match = re.search(rf"constexpr int {name}\s*=\s*(\d+);",
                          handle.read())
    if not match:
        raise RuntimeError(f"{name} not found in {TICKS_HEADER}")
    return int(match.group(1))


def load(name):
    """Load one consolidated combat log from report/data/.

    Args:
        name: File name without extension.
    Returns:
        The log as a DataFrame.
    """
    return pd.read_csv(os.path.join(DATA_DIR, f"{name}.csv"))


def with_maintenance_cost(df):
    """Keep rows with maintenance and add their mean cost per operation.

    Args:
        df: A combat log.
    Returns:
        A copy with columns ops (inserts + erases) and cost (steps/op).
    """
    df = df.copy()
    df["ops"] = df.inserts + df.erases
    df = df[df.ops > 0].copy()
    df["cost"] = (df.steps_insert + df.steps_erase) / df.ops
    return df


def fit_model(n, cost, weights, model):
    """Weighted least squares fit of cost = a + b * f(n).

    Args:
        n: Registry sizes.
        cost: Measured steps per operation.
        weights: Operations behind each measurement.
        model: Key of MODELS.
    Returns:
        (a, b, r2), with b = 0 for the constant model.
    """
    f = MODELS[model][1](n)
    w = np.sqrt(weights)
    if model == "const":
        a = np.average(cost, weights=weights)
        b = 0.0
    else:
        design = np.c_[np.ones_like(f), f] * w[:, None]
        (a, b), *_ = np.linalg.lstsq(design, cost * w, rcond=None)
    predicted = a + b * f
    mean = np.average(cost, weights=weights)
    ss_res = np.sum(weights * (cost - predicted) ** 2)
    ss_tot = np.sum(weights * (cost - mean) ** 2)
    return a, b, 1.0 - ss_res / ss_tot


def model_cost(fit, model, n):
    """Evaluate a fitted cost model.

    Args:
        fit: (a, b, r2) from fit_model.
        model: Key of MODELS.
        n: Registry sizes.
    Returns:
        Predicted steps per operation.
    """
    a, b, _ = fit
    predicted = a + b * MODELS[model][1](np.asarray(n, dtype=float))
    # A fitted line can dip below 1 near n = 0, but no operation costs
    # less than one step.
    return np.maximum(predicted, 1.0)


def binned(df, value, weight):
    """Weighted mean and spread of a value per registry-size bin.

    Args:
        df: Rows with avg_size, value and weight columns.
        value: Column to average.
        weight: Column used as weight.
    Returns:
        DataFrame indexed by bin with columns n (weighted mean size),
        mean, std and rows.
    """
    df = df.assign(bin=pd.cut(df.avg_size, SIZE_BINS))

    def summarize(group):
        w = group[weight]
        mean = np.average(group[value], weights=w)
        return pd.Series({
            "n": np.average(group.avg_size, weights=w),
            "mean": mean,
            "std": np.sqrt(np.average((group[value] - mean) ** 2, weights=w)),
            "rows": len(group),
        })

    return df.groupby("bin", observed=True)[
        ["avg_size", value, weight]].apply(summarize)


class Numbers:
    """Collects every number quoted in the report as \\dato{key}."""

    def __init__(self):
        self.values = {}  # key -> LaTeX text
        self.raw = {}     # key -> number, for values derived from others

    def put(self, key, value, decimals=1):
        """Store a number, formatted with a decimal comma.

        Args:
            key: Name used in LaTeX as \\dato{key}.
            value: Number to store.
            decimals: Decimal places to keep.
        """
        self.raw[key] = value
        self.values[key] = fmt(value, decimals)

    def write(self, path):
        """Write the lookup table as LaTeX definitions.

        Args:
            path: Destination .tex file.
        """
        with open(path, "w", encoding="utf-8") as out:
            out.write("% Generated by tools/make_figures.py -- do not edit.\n")
            for key, text in sorted(self.values.items()):
                out.write(f"\\expandafter\\def\\csname dato@{key}"
                          f"\\endcsname{{{text}}}\n")


def write_table(path, header, rows, align, super_header=None):
    """Write a booktabs tabular.

    Args:
        path: Destination .tex file.
        header: Column titles.
        rows: Lists of already formatted cells.
        align: Column alignment spec, e.g. "lrr".
        super_header: Optional LaTeX line (with its rules) placed above the
            column titles, e.g. to group columns under a common label.
    """
    with open(path, "w", encoding="utf-8") as out:
        out.write("% Generated by tools/make_figures.py -- do not edit.\n")
        out.write(f"\\begin{{tabular}}{{{align}}}\n\\toprule\n")
        if super_header:
            out.write(super_header + "\n")
        out.write(" & ".join(header) + " \\\\\n\\midrule\n")
        for row in rows:
            out.write(" & ".join(row) + " \\\\\n")
        out.write("\\bottomrule\n\\end{tabular}\n")


def fmt(value, decimals=1):
    """Format a number for a table cell with a decimal comma.

    Args:
        value: Number to format (NaN becomes a dash).
        decimals: Decimal places to keep.
    Returns:
        The LaTeX-ready text.
    """
    if value is None or (isinstance(value, float) and np.isnan(value)):
        return "---"
    # Spanish style: thin space for thousands, decimal comma, real minus.
    text = (f"{abs(value):,.{decimals}f}".replace(",", "\\,")
            .replace(".", "{,}"))
    return f"$-${text}" if value < 0 and float(f"{value:.{decimals}f}") else text


def save(fig, name):
    """Save a figure as PDF in report/figures/ and close it.

    Args:
        fig: Figure to save.
        name: File name without extension.
    """
    fig.savefig(os.path.join(FIG_DIR, f"{name}.pdf"), bbox_inches="tight")
    plt.close(fig)


# --------------------------------------------------------------------------
# Context: normal games
# --------------------------------------------------------------------------

def context(numbers):
    """Waves reached per structure under the normal rules.

    Args:
        numbers: Collector for quoted values.
    """
    games = load("normal_game")
    reached = games.groupby(["structure", "seed"]).wave.max()
    summary = reached.groupby("structure").agg(["mean", "std", "min", "max"])
    numbers.put("seeds", games.seed.nunique(), 0)
    rows = []
    for s in sorted(STRUCTURES, key=lambda s: summary.loc[s, "mean"]):
        stats = summary.loc[s]
        rows.append([NAMES[s], fmt(stats["mean"], 2), fmt(stats["std"], 2),
                     fmt(stats["min"], 0), fmt(stats["max"], 0)])
    write_table(os.path.join(GEN_DIR, "context_waves.tex"),
                ["Estructura", "Media", "Desv.", "Mín.", "Máx."], rows,
                "lrrrr")


# --------------------------------------------------------------------------
# Question 1: cost per operation vs registry size
# --------------------------------------------------------------------------

def question1(numbers):
    """Measured cost per maintenance operation vs n, with the fitted
    theoretical curve of each structure.

    Args:
        numbers: Collector for quoted values.
    Returns:
        {structure: (fit, model)} for question 2's prediction.
    """
    df = with_maintenance_cost(load("load_curve"))
    fits = {}
    rows = []
    fig, axes = plt.subplots(2, 4, figsize=(7.2, 3.9), sharex=True)
    for ax, s in zip(axes.flat, STRUCTURES):
        part = df[df.structure == s]
        n, cost, w = part.avg_size.values, part.cost.values, part.ops.values
        model = THEORY[s]
        all_fits = {m: fit_model(n, cost, w, m) for m in MODELS}
        fits[s] = (all_fits[model], model)

        bins = binned(part, "cost", "ops")
        ax.errorbar(bins.n, bins["mean"], yerr=bins["std"], fmt="o", ms=3,
                    color=COLORS[s], capsize=2, lw=0.8, label="medido")
        grid = np.linspace(0, part.avg_size.max(), 200)
        ax.plot(grid, model_cost(all_fits[model], model, grid), "k--",
                lw=1, label=f"${MODELS[model][0]}$")
        ax.set_title(NAMES[s])
        ax.set_ylim(bottom=0)  # same honest baseline in every panel
        ax.legend(loc="lower right", frameon=False)

        a, b, r2 = all_fits[model]
        rows.append([NAMES[s], f"${MODELS[model][0]}$", fmt(a, 2),
                     "---" if model == "const" else fmt(b, 3),
                     fmt(all_fits["log"][2], 3),
                     fmt(all_fits["linear"][2], 3)])
        numbers.put(f"q1-{s}-a", a, 2)
        numbers.put(f"q1-{s}-b", b, 3)
        numbers.put(f"q1-{s}-r2", r2, 3)
        numbers.put(f"q1-{s}-insert",
                    part.steps_insert.sum() / part.inserts.sum(), 1)
        numbers.put(f"q1-{s}-erase",
                    part.steps_erase.sum() / part.erases.sum(), 1)
        for regime, part_regime in [
                ("up", part[part.empty_queue_ratio >= KEEPING_UP_RATIO]),
                ("sat", part[part.empty_queue_ratio < SATURATED_RATIO])]:
            if part_regime.empty:
                continue  # e.g. the hash table never saturates
            for key in ["log", "linear"]:
                numbers.put(f"q1-{s}-{regime}-{key}", fit_model(
                    part_regime.avg_size.values, part_regime.cost.values,
                    part_regime.ops.values, key)[2], 3)
        top = bins.iloc[-1]
        numbers.put(f"q1-{s}-top-n", top.n, 0)
        numbers.put(f"q1-{s}-top-cost", top["mean"], 1)
    for ax in axes[1]:
        ax.set_xlabel("tamaño del registro $n$")
    for ax in axes[:, 0]:
        ax.set_ylabel("pasos por operación")
    fig.tight_layout()
    save(fig, "q1_cost_vs_size")
    write_table(os.path.join(GEN_DIR, "q1_fits.tex"),
                ["Estructura", "Modelo teórico", "$a$", "$b$",
                 "$R^2$ con $\\log n$", "$R^2$ con $n$"], rows, "llrrrr")
    numbers.put("q1-rows", len(df), 0)
    numbers.put("q1-max-n", df.avg_size.max(), 0)
    return fits


# --------------------------------------------------------------------------
# Question 2: saturation point, predicted vs observed
# --------------------------------------------------------------------------

def question2(numbers, fits):
    """Compare the size at which each structure's queue stops emptying
    with the one where supply STEPS_PER_TICK * TICKS_PER_SECOND / cost(n)
    meets the demand of the rotating enemies.

    Args:
        numbers: Collector for quoted values.
        fits: Question 1's fitted cost models.
    """
    steps_per_second = (read_constant("STEPS_PER_TICK")
                        * read_constant("TICKS_PER_SECOND"))
    numbers.put("steps-per-second", steps_per_second, 0)
    df = with_maintenance_cost(load("load_curve"))
    df = df[df.ticks > 0].copy()
    df["ops_per_s"] = df.ops / (df.ticks / read_constant("TICKS_PER_SECOND"))

    # Demand per enemy in range, from waves where the queue kept up (so
    # every requested operation was actually performed): ops/s = k * n.
    keeping_up = df[df.empty_queue_ratio >= KEEPING_UP_RATIO]
    k = ((keeping_up.ops_per_s * keeping_up.avg_size).sum()
         / (keeping_up.avg_size ** 2).sum())
    numbers.put("q2-k", k, 2)
    numbers.put("q2-keeping-up", KEEPING_UP_RATIO * 100, 0)
    numbers.put("q2-saturated", SATURATED_RATIO * 100, 0)

    # Observed: first saturated wave of each (structure, seed, slot).
    ordered = df.sort_values("wave")
    first = (ordered[ordered.empty_queue_ratio < SATURATED_RATIO]
             .groupby(["structure", "seed", "slot"]).first().reset_index())
    towers = df.groupby("structure")[["seed", "slot"]].apply(
        lambda g: len(g.drop_duplicates()))

    rows = []
    fig, ax = plt.subplots(figsize=(6.4, 4.0))
    grid = np.linspace(1, 160, 400)
    ax.plot(grid, k * grid, "k-", lw=1.6, label=f"demanda $k\\,n$")
    for s in STRUCTURES:
        fit, model = fits[s]
        supply = steps_per_second / model_cost(fit, model, grid)
        ax.plot(grid, supply, color=COLORS[s], lw=1.3, label=NAMES[s])
        # Predicted n*: supply(n) = k n, solved on the grid.
        crossing = np.where(supply <= k * grid)[0]
        predicted = grid[crossing[0]] if len(crossing) else np.nan
        if not np.isnan(predicted):
            ax.plot(predicted, k * predicted, "o", mfc="none",
                    color=COLORS[s], ms=7, mew=1.5)
        observed = first[first.structure == s].avg_size
        saturated_towers = len(observed)
        if saturated_towers:
            median = observed.median()
            ax.plot(median, k * median, "x", color=COLORS[s], ms=7, mew=2)
        else:
            median = np.nan
        rows.append([NAMES[s], fmt(predicted, 0), fmt(median, 0),
                     fmt(observed.quantile(0.25), 0) + "--"
                     + fmt(observed.quantile(0.75), 0)
                     if saturated_towers else "---",
                     f"{saturated_towers}/{towers[s]}"])
        numbers.put(f"q2-{s}-pred", predicted, 0)
        numbers.put(f"q2-{s}-obs", median, 0)
        numbers.put(f"q2-{s}-towers", saturated_towers, 0)
    numbers.put("q2-towers", towers.max(), 0)
    ax.set_xlabel("tamaño del registro $n$")
    ax.set_ylabel("operaciones por segundo")
    ax.set_yscale("log")
    ax.set_ylim(10, 5000)
    ax.plot([], [], "ko", mfc="none", label="$n^*$ predicho")
    ax.plot([], [], "kx", mew=2, label="$n$ observado")
    ax.legend(ncol=4, loc="upper center", bbox_to_anchor=(0.5, -0.18),
              frameon=False)
    fig.tight_layout()
    save(fig, "q2_saturation")
    write_table(os.path.join(GEN_DIR, "q2_saturation.tex"),
                ["Estructura", "$n^*$ predicho", "$n$ observado (mediana)",
                 "Rango intercuartil", "Torres saturadas"], rows, "lrrrr")


# --------------------------------------------------------------------------
# Question 3: sorted array vs linked list
# --------------------------------------------------------------------------

def question3(numbers):
    """Per-operation step breakdown of the sorted array and the linked
    list, and their cost as n grows.

    Args:
        numbers: Collector for quoted values.
    """
    df = with_maintenance_cost(load("load_curve"))
    pair = ["linked_list", "sorted_array"]
    categories = [("comparisons", "comparaciones"),
                  ("pointer_hops", "saltos de puntero"),
                  ("shifts", "corrimientos")]

    fig, (left, right) = plt.subplots(1, 2, figsize=(7.2, 3.0))
    per_op = {}
    for s in pair:
        part = df[df.structure == s]
        ops = part.ops.sum()
        # Every query of both structures is a single pointer hop (peek);
        # remove it so the bars show maintenance only.
        hops = part.pointer_hops.sum() - part.steps_query.sum()
        per_op[s] = {"comparisons": part.comparisons.sum() / ops,
                     "pointer_hops": hops / ops,
                     "shifts": part.shifts.sum() / ops}
        for key, _ in categories:
            numbers.put(f"q3-{s}-{key}", per_op[s][key], 2)
        numbers.put(f"q3-{s}-total", sum(per_op[s].values()), 1)
        numbers.put(f"q3-{s}-insert",
                    part.steps_insert.sum() / part.inserts.sum(), 1)
        numbers.put(f"q3-{s}-erase",
                    part.steps_erase.sum() / part.erases.sum(), 1)

    bottom = np.zeros(len(pair))
    for (key, label), color in zip(categories, ["#4c72b0", "#dd8452",
                                                "#55a868"]):
        values = np.array([per_op[s][key] for s in pair])
        left.bar([NAMES[s] for s in pair], values, bottom=bottom,
                 color=color, label=label)
        bottom += values
    left.set_ylabel("pasos por operación de mantenimiento")
    left.set_ylim(0, bottom.max() * 1.35)
    left.legend(frameon=False, loc="upper center", ncol=1)
    left.set_title("Desglose del costo")

    for s in pair:
        bins = binned(df[df.structure == s], "cost", "ops")
        right.plot(bins.n, bins["mean"], "o-", color=COLORS[s], ms=3,
                   label=NAMES[s])
    right.set_xlabel("tamaño del registro $n$")
    right.set_ylabel("pasos por operación")
    right.set_title("Costo según el tamaño")
    right.legend(frameon=False)
    fig.tight_layout()
    save(fig, "q3_sorted_vs_linked")

    # Same load (same wave), side by side: registry size, share of ticks
    # with an empty queue, steps and effective shots per match.
    logs = load("load_curve")
    seeds = logs.seed.nunique()
    rows = []
    for wave in Q3_WAVES:
        cells = [str(wave)]
        for s in pair:
            part = logs[(logs.structure == s) & (logs.wave == wave)]
            cells += [fmt(part.avg_size.mean(), 1),
                      fmt(part.empty_queue_ratio.mean() * 100, 0) + "\\,\\%",
                      fmt(part.steps_total.sum() / seeds / 1000, 1),
                      fmt(part.effective_shots.sum() / seeds, 0)]
            numbers.put(f"q3-{s}-w{wave}-n", part.avg_size.mean(), 1)
            numbers.put(f"q3-{s}-w{wave}-ksteps",
                        part.steps_total.sum() / seeds / 1000, 0)
            numbers.put(f"q3-{s}-w{wave}-shots",
                        part.effective_shots.sum() / seeds, 0)
        rows.append(cells)
    write_table(os.path.join(GEN_DIR, "q3_waves.tex"),
                ["Oleada", "$n$", "Cola vacía", "Kpasos", "Disparos",
                 "$n$", "Cola vacía", "Kpasos", "Disparos"], rows,
                "r@{\\hspace{10pt}}rrrr@{\\hspace{14pt}}rrrr",
                super_header=(" & \\multicolumn{4}{c}{" + NAMES[pair[0]]
                              + "} & \\multicolumn{4}{c}{" + NAMES[pair[1]]
                              + "} \\\\\n\\cmidrule(lr){2-5}"
                              "\\cmidrule(lr){6-9}"))

    # When each one saturates (first wave with the queue busy most of
    # the time), and how far each gets in normal games.
    ordered = logs.sort_values("wave")
    first = (ordered[ordered.empty_queue_ratio < SATURATED_RATIO]
             .groupby(["structure", "seed", "slot"]).first().reset_index())
    games = load("normal_game").groupby(["structure", "seed"]).wave.max()
    for s in pair:
        numbers.put(f"q3-{s}-satwave",
                    first[first.structure == s].wave.mean(), 1)
        numbers.put(f"q3-{s}-gamewave", games[s].mean(), 2)
    numbers.put("q3-shift-share", per_op["sorted_array"]["shifts"]
                / sum(per_op["sorted_array"].values()) * 100, 0)


# --------------------------------------------------------------------------
# Question 4: counted steps vs measured time
# --------------------------------------------------------------------------

def question4(numbers):
    """Fit real time = alpha * operations + beta * steps per structure and
    show where time per step stops being constant.

    Args:
        numbers: Collector for quoted values.
    """
    df = load("load_curve")
    df["ops"] = df.inserts + df.erases + df.queries
    df = df[df.ops > 0].copy()
    df["ns"] = df.real_us * 1000.0
    df["ns_per_step"] = df.ns / df.steps_total.replace(0, np.nan)

    rows = []
    fig, ax = plt.subplots(figsize=(6.2, 3.4))
    for s in STRUCTURES:
        part = df[df.structure == s]
        design = np.c_[part.ops, part.steps_total]
        (alpha, beta), *_ = np.linalg.lstsq(design, part.ns, rcond=None)
        predicted = design @ np.array([alpha, beta])
        r2 = 1 - (((part.ns - predicted) ** 2).sum()
                  / ((part.ns - part.ns.mean()) ** 2).sum())
        rows.append([NAMES[s], fmt(alpha, 1), fmt(beta, 2), fmt(r2, 3),
                     fmt(part.ns.sum() / part.steps_total.sum(), 1)])
        numbers.put(f"q4-{s}-alpha", alpha, 0)
        numbers.put(f"q4-{s}-beta", beta, 1)

        valid = part[part.steps_total > 0].assign(
            weight=lambda g: g.steps_total)
        bins = binned(valid, "ns_per_step", "weight")
        ax.plot(bins.n, bins["mean"], "o-", color=COLORS[s], ms=3,
                label=NAMES[s])
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("tamaño del registro $n$")
    ax.set_ylabel("nanosegundos por paso")
    ax.legend(ncol=2, frameon=False)
    fig.tight_layout()
    save(fig, "q4_ns_per_step")
    write_table(os.path.join(GEN_DIR, "q4_time_fit.tex"),
                ["Estructura", "$\\alpha$ (ns/op.)", "$\\beta$ (ns/paso)",
                 "$R^2$", "ns/paso promedio"], rows, "lrrrr")


# --------------------------------------------------------------------------
# Question 5: hash buckets under the Hive
# --------------------------------------------------------------------------

def question5(numbers):
    """Bucket-length distribution of the hash table under the Hive, with
    the original and the corrected hash function.

    Args:
        numbers: Collector for quoted values.
    """
    buckets = load("hive_buckets")
    big = buckets[buckets["size"] >= BUCKET_MIN_SIZE]
    numbers.put("q5-min-size", BUCKET_MIN_SIZE, 0)
    labels = {"default": "original (identidad)",
              "mixed": "corregida (SplitMix64)"}
    table_labels = {"default": "Original (identidad)",
                    "mixed": "Corregida (SplitMix64)"}

    fig, axes = plt.subplots(1, 2, figsize=(7.2, 2.8), sharey=True)
    for ax, hash_name in zip(axes, ["default", "mixed"]):
        dist = (big[big.hash == hash_name].groupby("chain_length")
                .buckets.sum())
        fraction = dist / dist.sum()
        ax.bar(fraction.index, fraction.values, width=0.8,
               color="#c44e52" if hash_name == "default" else "#4c72b0")
        if hash_name == "mixed":
            # Uniform hashing predicts Poisson(lambda) chain lengths, with
            # lambda = elements per bucket over the same snapshots.
            snapshots = big[big.hash == hash_name]
            per_snapshot = snapshots.drop_duplicates(
                ["seed", "wave", "slot"])
            total_buckets = dist.sum()
            load_factor = per_snapshot["size"].sum() / total_buckets
            lengths = np.arange(0, dist.index.max() + 2)
            poisson = (np.exp(-load_factor) * load_factor ** lengths
                       / np.array([math.factorial(k) for k in lengths]))
            ax.plot(lengths, poisson, "ko", ms=4, label="Poisson($\\lambda$)")
            ax.legend(frameon=False)
            numbers.put("q5-lambda", load_factor, 2)
            for k in range(4):
                numbers.put(f"q5-poisson-{k}", poisson[k] * 100, 1)
                numbers.put(f"q5-mixed-frac-{k}",
                            fraction.get(k, 0) * 100, 1)
        ax.set_yscale("log")
        ax.set_title(f"Función {labels[hash_name]}")
        ax.set_xlabel("largo de la cadena")
        numbers.put(f"q5-{hash_name}-empty", fraction.get(0, 0) * 100, 1)
        numbers.put(f"q5-{hash_name}-maxchain", dist.index.max(), 0)
    axes[0].set_ylabel("fracción de cubetas")
    fig.tight_layout()
    save(fig, "q5_buckets")

    logs = with_maintenance_cost(load("hive_hash"))
    rows = []
    for hash_name in ["default", "mixed"]:
        part = logs[logs.hash == hash_name]
        peak = part.loc[part.max_size.idxmax()]
        cost = ((part.steps_insert + part.steps_erase).sum()
                / part.ops.sum())
        rows.append([table_labels[hash_name], fmt(peak.max_size, 0),
                     fmt(peak.peak_bucket_count, 0),
                     fmt(peak.peak_used_buckets, 0),
                     fmt(peak.peak_max_bucket, 0), fmt(cost, 1),
                     fmt(part.empty_queue_ratio.mean() * 100, 0) + "\\,\\%"])
        numbers.put(f"q5-{hash_name}-cost", cost, 1)
        numbers.put(f"q5-{hash_name}-used", peak.peak_used_buckets, 0)
        numbers.put(f"q5-{hash_name}-count", peak.peak_bucket_count, 0)
        numbers.put(f"q5-{hash_name}-peak", peak.max_size, 0)
    numbers.put("q5-speedup", numbers.raw["q5-default-cost"]
                / numbers.raw["q5-mixed-cost"], 1)
    write_table(os.path.join(GEN_DIR, "q5_hash.tex"),
                ["Función", "$n$ máx.", "Cubetas", "Usadas",
                 "Cadena máx.", "Pasos/op.", "Cola vacía"], rows,
                "lrrrrrr")


# --------------------------------------------------------------------------
# Question 6: tree height under the Swarm
# --------------------------------------------------------------------------

def question6(numbers):
    """Height vs elements for BST and AVL under the Swarm, and the cost of
    AVL rotations against the steps they save.

    Args:
        numbers: Collector for quoted values.
    """
    trees = load("swarm_trees")
    trees = trees[trees.max_size > 0]

    fig, ax = plt.subplots(figsize=(5.6, 3.3))
    for s in ["bst", "avl"]:
        points = trees[trees.structure == s].groupby("max_size").peak_height
        ax.plot(points.mean().index, points.mean().values, "o", ms=3,
                color=COLORS[s], label=NAMES[s])
    n = np.arange(1, trees.max_size.max() + 1)
    bst_range = n[n <= trees[trees.structure == "bst"].max_size.max()]
    ax.plot(bst_range, bst_range, "k:", lw=1, label="$h = n$")
    ax.plot(n, np.log2(n + 1), "k--", lw=1, label=r"$h = \log_2(n+1)$")
    ax.plot(n, 1.44 * np.log2(n + 2), "k-.", lw=1,
            label=r"$h = 1{,}44\log_2(n+2)$")
    ax.set_xlabel("elementos en el árbol $n$ (inserciones netas)")
    ax.set_ylabel("altura $h$")
    ax.legend(frameon=False)
    fig.tight_layout()
    save(fig, "q6_height")

    maintenance = with_maintenance_cost(trees)
    rows = []
    per_op = {}
    for s in ["bst", "avl"]:
        part = maintenance[maintenance.structure == s]
        ops = part.ops.sum()
        per_op[s] = (part.steps_insert + part.steps_erase).sum() / ops
        rotations = part.rotations.sum() / ops
        rows.append([NAMES[s], fmt(part.max_size.max(), 0),
                     fmt(part.peak_height.max(), 0),
                     fmt(part.steps_insert.sum() / part.inserts.sum(), 1),
                     fmt(part.steps_erase.sum() / part.erases.sum(), 1),
                     fmt(per_op[s], 2), fmt(rotations, 2)])
        numbers.put(f"q6-{s}-maxn", part.max_size.max(), 0)
        numbers.put(f"q6-{s}-height", part.peak_height.max(), 0)
        numbers.put(f"q6-{s}-cost", per_op[s], 2)
        numbers.put(f"q6-{s}-insert",
                    part.steps_insert.sum() / part.inserts.sum(), 1)
        numbers.put(f"q6-{s}-erase",
                    part.steps_erase.sum() / part.erases.sum(), 1)
        numbers.put(f"q6-{s}-rotations", rotations, 2)
    saved = per_op["bst"] - per_op["avl"]
    numbers.put("q6-saved", saved, 2)
    numbers.put("q6-payback", saved / numbers.raw["q6-avl-rotations"], 0)
    write_table(os.path.join(GEN_DIR, "q6_trees.tex"),
                ["Árbol", "$n$ máx.", "Altura máx.", "Pasos/inserción",
                 "Pasos/borrado", "Pasos/op.", "Rotaciones/op."], rows,
                "lrrrrrr")


def main():
    os.makedirs(FIG_DIR, exist_ok=True)
    os.makedirs(GEN_DIR, exist_ok=True)
    numbers = Numbers()
    context(numbers)
    fits = question1(numbers)
    question2(numbers, fits)
    question3(numbers)
    question4(numbers)
    question5(numbers)
    question6(numbers)
    numbers.write(os.path.join(GEN_DIR, "numbers.tex"))
    print(f"Figures in {os.path.relpath(FIG_DIR, PROJECT)}/, tables and "
          f"numbers in {os.path.relpath(GEN_DIR, PROJECT)}/")


if __name__ == "__main__":
    main()