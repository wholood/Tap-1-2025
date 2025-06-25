import os
import matplotlib.pyplot as plt
import re

# Configuración de carpetas y colores
estructuras = ['binomial', 'binario', 'fibonacci']
colores = {'binomial': 'blue', 'binario': 'green', 'fibonacci': 'red'}
experimentos = ['exp1', 'exp2', 'exp3']

# Crear carpeta para guardar gráficos
os.makedirs('graficas', exist_ok=True)

# Diccionario para almacenar todos los datos
datos = {e: {exp: {} for exp in experimentos} for e in estructuras}

# Función para extraer valores numéricos de las cadenas
def extraer_valores(linea):
    return [float(x) for x in re.findall(r'=\s*([\d.]+)', linea)]

# Procesar cada archivo
for estructura in estructuras:
    archivo = os.path.join(estructura, 'procesado.txt')
    
    if not os.path.exists(archivo):
        print(f"Advertencia: Archivo no encontrado - {archivo}")
        continue
        
    with open(archivo, 'r') as f:
        contenido = f.read()
    
    # Experimento 1: Solo inserciones
    exp1 = re.search(r'=== Experimento 1 ===(.*?)(?===)', contenido, re.DOTALL)
    if exp1:
        for linea in exp1.group(1).splitlines():
            if 'M=' in linea:
                valores = extraer_valores(linea)
                if len(valores) >= 2:
                    M = int(re.search(r'M=(\d+)', linea).group(1))
                    datos[estructura]['exp1'].setdefault('M', []).append(M)
                    datos[estructura]['exp1'].setdefault('tiempo_total', []).append(valores[0])
                    datos[estructura]['exp1'].setdefault('tiempo_insercion', []).append(valores[1])
    
    # Experimento 2: Operaciones mixtas
    exp2 = re.search(r'=== Experimento 2 ===(.*?)(?===)', contenido, re.DOTALL)
    if exp2:
        for linea in exp2.group(1).splitlines():
            if 'O=' in linea:
                valores = extraer_valores(linea)
                if len(valores) >= 2:
                    O = int(re.search(r'O=(\d+)', linea).group(1))
                    datos[estructura]['exp2'].setdefault('O', []).append(O)
                    datos[estructura]['exp2'].setdefault('tiempo_total', []).append(valores[0])
                    datos[estructura]['exp2'].setdefault('tiempo_operacion', []).append(valores[1])
    # Experimento 3: Operaciones detalladas
    exp3 = re.search(r'=== Experimento 3 ===(.*)', contenido, re.DOTALL)
    if exp3:
        current_N = None
        for linea in exp3.group(1).splitlines():
            if 'N=' in linea:
                current_N = int(re.search(r'N=(\d+)', linea).group(1))
                datos[estructura]['exp3'].setdefault('N', []).append(current_N)
            elif 'Insercion' in linea:
                valores = extraer_valores(linea)
                if valores and current_N is not None:
                    datos[estructura]['exp3'].setdefault('insercion', []).append(valores[2])
            elif 'Consulta' in linea:
                valores = extraer_valores(linea)
                if valores and current_N is not None:
                    datos[estructura]['exp3'].setdefault('consulta', []).append(valores[2])
            elif 'Extraccion' in linea:
                valores = extraer_valores(linea)
                if valores and current_N is not None:
                    datos[estructura]['exp3'].setdefault('extraccion', []).append(valores[2])
            elif 'Union' in linea:
                valores = extraer_valores(linea)
                if valores and current_N is not None:
                    datos[estructura]['exp3'].setdefault('union', []).append(valores[2])

# Función para graficar comparaciones
def graficar_comparacion(exp, metrica, titulo, eje_y, escala_log_x=True, escala_log_y=False):
    plt.figure(figsize=(10, 6))
    
    for estructura in estructuras:
        if exp not in datos[estructura] or not datos[estructura][exp]:
            continue
            
        x = []
        y = []
        
        # Determinar el tipo de datos en el eje X
        if exp == 'exp1':
            x_key = 'M'
        elif exp == 'exp2':
            x_key = 'O'
        else:
            x_key = 'N'
        
        # Recopilar datos válidos
        for i in range(len(datos[estructura][exp].get(x_key, []))):
            if metrica in datos[estructura][exp] and i < len(datos[estructura][exp][metrica]):
                x.append(datos[estructura][exp][x_key][i])
                y.append(datos[estructura][exp][metrica][i])
        
        if x and y:
            plt.plot(
                x, y, 
                'o-',  
                label=f'{estructura.capitalize()}',
                linewidth=2,
                markersize=8
            )
    
    if escala_log_x:
        plt.xscale('log')
    if escala_log_y:
        plt.yscale('log')
    
    plt.title(titulo, fontsize=14)
    plt.xlabel('Tamaño del problema', fontsize=12)
    plt.ylabel(eje_y, fontsize=12)
    plt.grid(True)  
    plt.legend()
    plt.tight_layout()
    
    # Guardar gráfico
    nombre_archivo = f'graficas/{exp}_{metrica}.png'
    plt.savefig(nombre_archivo, dpi=300)
    plt.close()
    print(f"Gráfico guardado: {nombre_archivo}")

# Generar todas las gráficas
# Experimento 1
graficar_comparacion(
    exp='exp1',
    metrica='tiempo_insercion',
    titulo='Experimento 1: Promedio de Tiempo por Inserción para 3 Ejecuciones',
    eje_y='Tiempo/inserción (ms)',
    escala_log_y=True
)

# Experimento 2
graficar_comparacion(
    exp='exp2',
    metrica='tiempo_operacion',
    titulo='Experimento 2: Promedio de Tiempo por Operación para 3 Ejecuciones',
    eje_y='Tiempo/operación (ms)',
    escala_log_y=True
)
# Experimento 3: Todas las operaciones en la misma gráfica
def graficar_comparacion_exp3_operaciones():
    plt.figure(figsize=(10, 6))
    operaciones = ['insercion', 'consulta', 'extraccion', 'union']
    colores_op = {'insercion': 'blue', 'consulta': 'orange', 'extraccion': 'green', 'union': 'purple'}
    for estructura in estructuras:
        if 'exp3' not in datos[estructura] or not datos[estructura]['exp3']:
            continue
        x = datos[estructura]['exp3'].get('N', [])
        for operacion in operaciones:
            # No graficar 'union' para binario
            if estructura == 'binario' and operacion == 'union':
                continue
            y = datos[estructura]['exp3'].get(operacion, [])
            if x and y:
                plt.plot(
                    x, y,
                    'o-',
                    label=f'{estructura.capitalize()} - {operacion.capitalize()}',
                    linewidth=2,
                    markersize=8,
                    color=colores_op[operacion] if estructura == estructuras[0] else None
                )
    plt.xscale('log')
    plt.title('Experimento 3: Promedio de Tiempo por Operación Ordenada para 3 Ejecuciones', fontsize=14)
    plt.xlabel('Tamaño del problema', fontsize=12)
    plt.ylabel('Tiempo promedio (µs)', fontsize=12)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    nombre_archivo = 'graficas/exp3_operaciones.png'
    plt.savefig(nombre_archivo, dpi=300)
    plt.close()
    print(f"Gráfico guardado: {nombre_archivo}")

graficar_comparacion_exp3_operaciones()
print("Proceso completado. Todos los gráficos guardados en la carpeta 'graficas'")