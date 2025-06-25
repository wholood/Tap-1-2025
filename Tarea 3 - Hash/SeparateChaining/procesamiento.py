import os
import re
import numpy as np

def parse_file(filepath):
    """
    Parsea un archivo de salida de experimento de tabla hash y extrae los tiempos.
    Asume el formato de salida proporcionado (SeparateChaining1.txt, SeparateChaining2.txt, SeparateChaining3.txt).
    Retorna un diccionario anidado:
    {
        (M, num_operaciones): {
            'inserciones': [tiempo1, tiempo2, ...],
            'busquedas': [tiempo1, tiempo2, ...],
            'eliminaciones': [tiempo1, tiempo2, ...],
            'promedio_total': [tiempo1, tiempo2, ...],
            'promedio_por_op': [tiempo1, tiempo2, ...]
        },
        ...
    }
    """
    data = {}
    
    with open(filepath, 'r') as f:
        content = f.read()

    # Dividir el contenido por los delimitadores principales de M y Operaciones
    # Buscamos "====== Experimentos para M = X ======" o "====== Operaciones = Y ======"
    sections = re.split(r'(====== Experimentos para M = \d+ ======|====== Operaciones = \d+ ======)', content)
    
    current_M = None
    current_ops = None

    for i in range(len(sections)):
        section_header = sections[i].strip()
        
        m_match = re.search(r'====== Experimentos para M = (\d+) ======', section_header)
        if m_match:
            current_M = int(m_match.group(1))
            continue

        ops_match = re.search(r'====== Operaciones = (\d+) ======', section_header)
        if ops_match:
            current_ops = int(ops_match.group(1))
            # Inicializa la entrada para esta combinación (M, num_operaciones) si no existe
            if (current_M, current_ops) not in data:
                data[(current_M, current_ops)] = {
                    'inserciones': [],
                    'busquedas': [],
                    'eliminaciones': [],
                    'promedio_total': [],
                    'promedio_por_op': []
                }
            continue

        # El cuerpo de la sección que contiene los resultados de los experimentos
        section_body = sections[i]

        # --- CORRECCIONES APLICADAS AQUÍ EN LAS REGEX PARA SeparateChainingX.txt ---
        # Volvemos a requerir el ':' después de 'Experimento'
        # Corregimos 'operacian' a 'operacion'
        
        # Experimento: Uso dominado por inserciones
        insert_match = re.search(r'Experimento: Uso dominado por inserciones \(M inicial: \d+\) ======\n.+?Inserciones \(\d+ elementos\): (\d+) microsegundos', section_body, re.DOTALL)
        if insert_match:
            if current_M is not None and current_ops is not None:
                data[(current_M, current_ops)]['inserciones'].append(int(insert_match.group(1)))

        # Experimento: Uso dominado por busquedas
        search_match = re.search(r'Experimento: Uso dominado por b(u|u)squedas \(M inicial: \d+\) ======\n.+?B(u|u)squedas \(\d+ intentos, \d+ encontrados\): (\d+) microsegundos', section_body, re.DOTALL)
        if search_match:
            if current_M is not None and current_ops is not None:
                data[(current_M, current_ops)]['busquedas'].append(int(search_match.group(3)))

        # Experimento: Uso dominado por eliminaciones
        delete_match = re.search(r'Experimento: Uso dominado por eliminaciones \(M inicial: \d+\) ======\n.+?Eliminaciones \(\d+ intentos, \d+ eliminados\): (\d+) microsegundos', section_body, re.DOTALL)
        if delete_match:
            if current_M is not None and current_ops is not None:
                data[(current_M, current_ops)]['eliminaciones'].append(int(delete_match.group(1)))

        # Experimento: Uso promedio (Tiempo total)
        avg_total_match = re.search(r'Experimento: Uso promedio \(M inicial: \d+\) ======\n.+?Tiempo total: (\d+) microsegundos', section_body, re.DOTALL)
        if avg_total_match:
            if current_M is not None and current_ops is not None:
                data[(current_M, current_ops)]['promedio_total'].append(int(avg_total_match.group(1)))
        
        # Experimento: Uso promedio (Tiempo promedio por operación)
        # Corregido a 'operacion'
        avg_per_op_match = re.search(r'Tiempo promedio por operacion: ([\d.]+) microsegundos', section_body, re.DOTALL)
        if avg_per_op_match:
            if current_M is not None and current_ops is not None:
                data[(current_M, current_ops)]['promedio_por_op'].append(float(avg_per_op_match.group(1)))

    return data


def calculate_averages(all_files_data):
    """
    Calcula los promedios de los tiempos de los experimentos a partir de los datos de multiples archivos.
    Retorna un diccionario:
    {
        (M, num_operaciones): {
            'inserciones_avg': promedio_tiempo,
            'busquedas_avg': promedio_tiempo,
            'eliminaciones_avg': promedio_tiempo,
            'promedio_total_avg': promedio_tiempo,
            'promedio_por_op_avg': promedio_tiempo
        },
        ...
    }
    """
    averaged_data = {}
    
    # Recopilar todas las claves (M, num_operaciones) unicas de todos los archivos
    unique_keys = set()
    for file_data in all_files_data:
        unique_keys.update(file_data.keys())

    # Iterar sobre cada clave unica y calcular promedios
    valid_keys = [k for k in unique_keys if all(x is not None for x in k)]
    
    for key in sorted(list(valid_keys)): # Ordenar para una salida consistente
        M, num_ops = key
        
        # Recopilar todos los tiempos para esta clave y tipo de experimento
        all_insertions = []
        all_busquedas = []
        all_eliminaciones = []
        all_promedio_total = []
        all_promedio_por_op = []

        for file_data in all_files_data:
            if key in file_data:
                all_insertions.extend(file_data[key]['inserciones'])
                all_busquedas.extend(file_data[key]['busquedas'])
                all_eliminaciones.extend(file_data[key]['eliminaciones'])
                all_promedio_total.extend(file_data[key]['promedio_total'])
                all_promedio_por_op.extend(file_data[key]['promedio_por_op'])
        
        # Calcular los promedios, evitando la división por cero
        avg_inserciones = np.mean(all_insertions) if all_insertions else 0
        avg_busquedas = np.mean(all_busquedas) if all_busquedas else 0
        avg_eliminaciones = np.mean(all_eliminaciones) if all_eliminaciones else 0
        avg_promedio_total = np.mean(all_promedio_total) if all_promedio_total else 0
        avg_promedio_por_op = np.mean(all_promedio_por_op) if all_promedio_por_op else 0

        averaged_data[key] = {
            'inserciones_avg': avg_inserciones,
            'busquedas_avg': avg_busquedas,
            'eliminaciones_avg': avg_eliminaciones,
            'promedio_total_avg': avg_promedio_total,
            'promedio_por_op_avg': avg_promedio_por_op
        }
    
    return averaged_data


def write_averages_to_file(averaged_data, output_filepath):
    """
    Escribe los promedios calculados en un archivo de texto en formato de lista.
    """
    with open(output_filepath, 'w') as f:
        f.write("--- PROMEDIOS DE RENDIMIENTO DE TABLAS HASH ---\n\n")
        
        # Ordenar las claves para una salida consistente
        for (M, ops) in sorted(averaged_data.keys()):
            data = averaged_data[(M, ops)]
            f.write(f"Resultados para M = {M}, Operaciones = {ops}:\n")
            f.write(f"  - Inserciones promedio: {data['inserciones_avg']:.2f} microsegundos\n")
            f.write(f"  - Busquedas promedio: {data['busquedas_avg']:.2f} microsegundos\n")
            f.write(f"  - Eliminaciones promedio: {data['eliminaciones_avg']:.2f} microsegundos\n")
            f.write(f"  - Tiempo total promedio: {data['promedio_total_avg']:.2f} microsegundos\n")
            f.write(f"  - Tiempo promedio por operacion: {data['promedio_por_op_avg']:.2f} microsegundos\n\n")
        
        f.write("Procesamiento completado sobre las 3 ejecuciones por experimento.\n")


if __name__ == "__main__":
    # DIRECTORIO DE ENTRADA:
    # Por defecto, el script buscará los archivos .txt en el mismo directorio
    # desde donde se ejecuta. Si tus archivos .txt están en una subcarpeta
    # (ej., 'resultados_doble_hash/'), especifica la ruta aquí.
    input_directory = './' # Ajusta esto si tus archivos están en otra carpeta

    # NOMBRE DEL ARCHIVO DE SALIDA:
    output_filename = 'promedios_experimentos.txt'
    output_filepath = os.path.join(input_directory, output_filename)

    print(f"Buscando archivos .txt en: {os.path.abspath(input_directory)}")
    
    # Define explícitamente los archivos a procesar
    # ¡IMPORTANTE! Cambiado a los nombres de SeparateChaining
    txt_files_to_process = ['SeparateChaining1.txt', 'SeparateChaining2.txt', 'SeparateChaining3.txt']
    
    # Filtrar solo los archivos que existen en el input_directory
    txt_files = [f for f in txt_files_to_process if os.path.isfile(os.path.join(input_directory, f))]
    
    if not txt_files:
        print(f"No se encontraron los archivos especificados ({', '.join(txt_files_to_process)}) en '{input_directory}'. Asegurate de que los archivos estén ahí.")
        exit()

    all_data_from_files = []
    print(f"Archivos encontrados para procesar: {txt_files}")
    
    for filename in txt_files:
        filepath = os.path.join(input_directory, filename)
        print(f"Procesando archivo: {filepath}")
        file_data = parse_file(filepath)
        all_data_from_files.append(file_data)

    print("Calculando promedios...")
    averaged_results = calculate_averages(all_data_from_files)

    print(f"Guardando promedios en: {output_filepath}")
    write_averages_to_file(averaged_results, output_filepath)

    print("Procesamiento completado.")