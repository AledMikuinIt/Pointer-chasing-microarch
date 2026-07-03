import csv
import matplotlib.pyplot as plt

# --- Lecture du CSV ---
n_list = []
temps_lin = []
temps_disp = []

with open("results.csv", "r", encoding="utf-8") as f:
    reader = csv.DictReader(f)
    for row in reader:
        n_list.append(int(row["N"]))
        temps_lin.append(int(row["Temps_lineaire"]))
        temps_disp.append(int(row["Temps_disperse"]))

print("Lecture OK")

# --- Graphique principal ---
plt.figure(figsize=(8, 5))
plt.plot(n_list, temps_lin, marker='o', label='Linéaire')
plt.plot(n_list, temps_disp, marker='o', label='Dispersé')
plt.xlabel('Nombre d’objets')
plt.ylabel('Temps (ns)')
plt.title('Temps de parcours (linéaire vs dispersé)')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("graph_temps.png")
plt.close()

# --- Graphique du ratio ---
ratio = [d / l for d, l in zip(temps_disp, temps_lin)]

plt.figure(figsize=(8, 5))
plt.plot(n_list, ratio, marker='o')
plt.xlabel('Nombre d’objets')
plt.ylabel('Ratio (dispersé / linéaire)')
plt.title('Impact de la dispersion')
plt.grid(True)
plt.tight_layout()
plt.savefig("graph_ratio.png")
plt.close()

print("Fichiers générés : graph_temps.png et graph_ratio.png")
