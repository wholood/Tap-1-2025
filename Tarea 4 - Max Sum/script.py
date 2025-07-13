import subprocess
import matplotlib.pyplot as plt
import re

archivos = {
    "Kadane": "kadane.cpp",
    "FuerzaBruta": "fuerzabruta.cpp"
}

for nombre, archivo in archivos.items():
    print(f"Compilando {archivo}...")
    resultado = subprocess.run(["g++", archivo, "-o", nombre.replace(" ", "_")], capture_output=True, text=True)
    if resultado.returncode != 0:
        print(f"Error al compilar {archivo}:\n{resultado.stderr}")
        exit(1)

tiempos = {}

for nombre in archivos:
    print(f"Ejecutando {nombre}...")
    ejecutable = f"./{nombre.replace(' ', '_')}"
    resultado = subprocess.run([ejecutable], capture_output=True, text=True)
    if resultado.returncode != 0:
        print(f"Error al ejecutar {nombre}:\n{resultado.stderr}")
        exit(1)

    print(f"Resultados de {nombre}:\n{resultado.stdout}")
    patron = r"Tiempo promedio para N=(\d+): ([\deE\.\+]+) microsegundos"
    tiempos[nombre] = [(int(n), float(t)) for n, t in re.findall(patron, resultado.stdout)]


plt.figure(figsize=(10, 6))
for nombre, datos in tiempos.items():
    ns, ts = zip(*datos)
    plt.plot(ns, ts, marker='o', label=nombre)

plt.title("Comparación de Tiempos Promedio")
plt.xlabel("Tamaño de la matriz (N)")
plt.ylabel("Tiempo promedio (microsegundos)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("comparativa_tiempos_combinado.png")
#plt.show() # Descomentar para mostrar el gráfico combinado


for nombre, datos in tiempos.items():
    plt.figure(figsize=(10, 6))
    ns, ts = zip(*datos)
    plt.plot(ns, ts, marker='o', label=nombre)
    plt.title(f"Tiempo Promedio para {nombre}")
    plt.xlabel("Tamaño de la matriz (N)")
    plt.ylabel("Tiempo promedio (microsegundos)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"tiempo_promedio_{nombre.replace(' ', '_')}.png")
    #plt.show() # Descomentar para mostrar cada gráfico individualmente

plt.show() # Muestra todos los gráficos generados al final