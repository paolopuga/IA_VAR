#!/usr/bin/env python3
import subprocess, csv, random, time, os
from concurrent.futures import ThreadPoolExecutor, as_completed

# Parámetros de la búsqueda
ITERATIONS = 10
BOUNDS = {
    "safe":     (0.0,  5.0, 0.5),
    "goal":     (0.0, 10.0, 1.0),
    "home":     (0.0, 10.0, 1.0),
    "eaten":    (0.0, 10.0, 0.5),
    "bounced":  (0.0, 10.0, 0.5),
    "gmove":    (0.0, 10.0, 0.5),
    "queue":    (0.0,  5.0, 0.5)
}

NINJAS       = [1,2,3]
POSITIONS    = [1,2]               # 1: IA p1, 2: IA p2
TOTAL_MATCHES = len(NINJAS) * len(POSITIONS)
LOG_DIR      = "log_temp"
os.makedirs(LOG_DIR, exist_ok=True)

def write_csv(params):
    with open("bonuses.csv","w",newline="") as f:
        w = csv.writer(f)
        w.writerow(["name","value"])
        for k,v in params.items():
            w.writerow([k,v])

def run_one(ninja, pos, ia_id):
    time.sleep(0.1)
    logfile = f"{LOG_DIR}/log_ninja{ninja}_pos{pos}.txt"
    if pos == 1:
        cmd = [
            "./ParchisGame",
            "--p1","AI",str(ia_id),"MejoraTest",
            "--p2","Ninja",str(ninja),f"Ninja{ninja}",
            "--no-gui"
        ]
    else:
        cmd = [
            "./ParchisGame",
            "--p1","Ninja",str(ninja),f"Ninja{ninja}",
            "--p2","AI",str(ia_id),"MejoraTest",
            "--no-gui"
        ]
    try:
        with open(logfile,"w") as lf:
            subprocess.run(cmd, text=True, stdout=lf, stderr=lf, check=True)
        out = open(logfile).read()
        if pos == 1:
            res = 1 if "Ha ganado el jugador 1" in out else 0
        else:
            res = 1 if "Ha ganado el jugador 2" in out else 0
    except subprocess.CalledProcessError:
        res = 0
    return ninja, pos, res

def random_neighbor(cur):
    nxt = dict(cur)
    k = random.choice(list(cur.keys()))
    lo,hi,step = BOUNDS[k]
    nxt[k] = max(lo, min(hi, cur[k] + random.choice([-step,step])))
    return nxt

def tune(ia_id=0):
    with open("bonuses.csv") as f:
        initial = {r["name"]: float(r["value"]) for r in csv.DictReader(f)}
    best, best_score = initial.copy(), 0

    det_file   = open("detailed_results.csv","w",newline="")
    det_w      = csv.writer(det_file)
    det_w.writerow(["iteration","ninja","pos","result"])

    hist_file  = open("tuning_history.csv","w",newline="")
    hist_w     = csv.writer(hist_file)
    hist_w.writerow(list(best.keys()) + ["wins"])

    best_file  = open("best_params.csv","w",newline="")
    best_w     = csv.writer(best_file)
    best_w.writerow(list(best.keys()) + ["wins"])

    for it in range(ITERATIONS):
        if it == 0:
            candidate = best.copy()
        else:
            candidate = random_neighbor(best)
            write_csv(candidate)

        wins      = 0
        remaining = TOTAL_MATCHES

        executor  = ThreadPoolExecutor(max_workers=6)
        future_map = {}
        for n in NINJAS:
            for p in POSITIONS:
                fut = executor.submit(run_one, n, p, ia_id)
                future_map[fut] = (n,p)
                time.sleep(0.1)

        # collect results with early exit
        for fut in as_completed(future_map):
            n,p = future_map[fut]
            _,_,res = fut.result()
            wins    += res
            remaining -= 1
            det_w.writerow([it, n, p, res])
            if wins + remaining <= best_score:
                executor.shutdown(wait=False, cancel_futures=True)
                break
        else:
            executor.shutdown(wait=True)

        hist_w.writerow([candidate[k] for k in best.keys()] + [wins])

        print(f"Iter {it:02d}: {wins}/{TOTAL_MATCHES} wins", end="")
        if wins > best_score:
            best_score, best = wins, candidate.copy()
            best_w.writerow([best[k] for k in best.keys()] + [best_score])
            print("  <-- NEW BEST")
            if wins == TOTAL_MATCHES:
                break
        else:
            print()

    det_file.close()
    hist_file.close()
    best_file.close()
    print(f"\nMejor final: {best_score}/{TOTAL_MATCHES} con {best}")

if __name__=="__main__":
    tune(ia_id=0)
