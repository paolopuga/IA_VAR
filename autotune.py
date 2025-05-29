#!/usr/bin/env python3
import subprocess
import csv
import random
import time
import os
from concurrent.futures import ThreadPoolExecutor, as_completed

# Parámetros de la búsqueda
ITERATIONS = 50
# Rango y paso de mutación para cada bonus
BOUNDS = {
    "safe":     (0.0, 5.0, 0.5),
    "goal":     (0.0, 10.0, 1.0),
    "opp_safe": (0.0, 5.0, 0.5),
    "opp_goal": (0.0, 10.0, 1.0)
}

# Ninjas y posiciones
NINJAS = [1,2,3]
POSITIONS = [1,2]  # 1: IA es p1, 2: IA es p2

# Base de directorios de logs
LOG_DIR = "log_temp"
os.makedirs(LOG_DIR, exist_ok=True)

def write_csv(params):
    """Sobrescribe bonuses.csv con los valores del diccionario."""
    with open("bonuses.csv","w",newline="") as f:
        w = csv.writer(f)
        w.writerow(["name","value"])
        for k,v in params.items():
            w.writerow([k,v])

def run_one(ninja, pos, ia_id, params):
    """
    Ejecuta una partida contra `ninja` en posición `pos`,
    con IA de id `ia_id`. Devuelve 1 si gana la IA, 0 si pierde o error.
    Además escribe log en log_temp/log_ninjaX_posY.txt.
    """
    logfile = f"{LOG_DIR}/log_ninja{ninja}_pos{pos}.txt"
    cmd = []
    if pos == 1:
        cmd = ["./ParchisGame",
               "--p1","AI",str(ia_id),"MejoraTest",
               "--p2","Ninja",str(ninja),f"Ninja{ninja}",
               "--no-gui"]
    else:
        cmd = ["./ParchisGame",
               "--p1","Ninja",str(ninja),f"Ninja{ninja}",
               "--p2","AI",str(ia_id),"MejoraTest",
               "--no-gui"]
    try:
        with open(logfile,"w") as lf:
            proc = subprocess.run(cmd, text=True, stdout=lf, stderr=lf, check=True)
        # Leer las últimas líneas para determinar el ganador
        with open(logfile) as lf:
            data = lf.read()
        if pos == 1:
            return 1 if "Ha ganado el Jugador 1" in data else 0
        else:
            return 1 if "Ha ganado el Jugador 2" in data else 0
    except subprocess.CalledProcessError:
        # si falla, contar como derrota
        return 0

def random_neighbor(cur):
    """Mutación simple de un parámetro."""
    nxt = dict(cur)
    key = random.choice(list(cur.keys()))
    lo, hi, step = BOUNDS[key]
    delta = random.choice([-step, step])
    nxt[key] = max(lo, min(hi, cur[key] + delta))
    return nxt

def tune(ia_id=0):
    # Lee configuración inicial
    with open("bonuses.csv") as f:
        reader = csv.DictReader(f)
        current = {r["name"]:float(r["value"]) for r in reader}

    best, best_score = current.copy(), 0
    history = []

    for it in range(ITERATIONS):
        candidate = random_neighbor(best)
        write_csv(candidate)

        # Lanza las 6 partidas en paralelo con ThreadPool (I/O bound)
        wins = 0
        futures = []
        with ThreadPoolExecutor(max_workers=6) as ex:
            for ninja in NINJAS:
                for pos in POSITIONS:
                    futures.append(ex.submit(run_one, ninja, pos, ia_id, candidate))
                    time.sleep(0.1)  # leve pausa entre lanzamientos
            for f in as_completed(futures):
                wins += f.result()

        history.append((candidate.copy(), wins))
        print(f"Iter {it:03d}: {wins}/6 wins", end="")
        if wins > best_score:
            best_score, best = wins, candidate
            print("  <-- NEW BEST")
            if best_score == 6:
                break
        else:
            print()

    # Guardar historial
    with open("tuning_history.csv","w",newline="") as f:
        w = csv.writer(f)
        w.writerow(list(best.keys())+["wins"])
        for p,s in history:
            w.writerow([p[k] for k in best.keys()]+[s])

    print("\nFINAL: best_score=", best_score)
    print("best_params=", best)
    return best, best_score

if __name__=="__main__":
    tune(ia_id=0)
