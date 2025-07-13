import os
import re
import matplotlib.pyplot as plt

def parse_promedios_file(filepath):
    data = {}
    current_M = None
    current_Operaciones = None

    try:
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()

                # Coincidir con la línea de encabezado de resultados
                header_match = re.match(r'Resultados para M = (\d+), Operaciones = (\d+):', line)
                if header_match:
                    current_M = int(header_match.group(1))
                    current_Operaciones = int(header_match.group(2))
                    if current_M not in data:
                        data[current_M] = {}
                    data[current_M][current_Operaciones] = {}
                    continue

                # Coincidir con las métricas de tiempo
                if current_M is not None and current_Operaciones is not None:
                    inserciones_match = re.match(r'- Inserciones promedio: ([\d.]+) microsegundos', line)
                    busquedas_match = re.match(r'- Busquedas promedio: ([\d.]+) microsegundos', line)
                    eliminaciones_match = re.match(r'- Eliminaciones promedio: ([\d.]+) microsegundos', line)
                    total_promedio_match = re.match(r'- Tiempo total promedio: ([\d.]+) microsegundos', line)
                    promedio_por_op_match = re.match(r'- Tiempo promedio por operacion: ([\d.]+) microsegundos', line)

                    if inserciones_match:
                        data[current_M][current_Operaciones]['Inserciones'] = float(inserciones_match.group(1))
                    elif busquedas_match:
                        data[current_M][current_Operaciones]['Busquedas'] = float(busquedas_match.group(1))
                    elif eliminaciones_match:
                        data[current_M][current_Operaciones]['Eliminaciones'] = float(eliminaciones_match.group(1))
                    elif total_promedio_match:
                        data[current_M][current_Operaciones]['Total_Promedio'] = float(total_promedio_match.group(1))
                    elif promedio_por_op_match:
                        data[current_M][current_Operaciones]['Promedio_por_Op'] = float(promedio_por_op_match.group(1))

        return data
    except FileNotFoundError:
        print(f"Advertencia: Archivo no encontrado - {filepath}. Se omitirá esta implementación.")
        return {}
    except Exception as e:
        print(f"Error al leer el archivo {filepath}: {e}")
        return {}


def generate_comparison_graphs(data_by_implementation, output_dir='graficas_hash'):
    os.makedirs(output_dir, exist_ok=True)

    # Identificar todos los valores de M presentes en los datos
    all_M_values = sorted(list(set(
        M_val for impl_data in data_by_implementation.values()
        for M_val in impl_data.keys()
    )))

    colors = {
        'DoubleHash': 'blue',
        'LinealProbing': 'red',
        'QuadraticHash': 'green',
        'SeparateChaining': 'purple',
        'Map': 'orange',
        'Set': 'cyan'
    }

    # Gráficas para Tiempo Total Promedio
    for M_val in all_M_values:
        plt.figure(figsize=(12, 7))
        for impl_name, impl_data in data_by_implementation.items():
            if M_val in impl_data:
                # Obtener operaciones y tiempos para el M_val actual, ordenados por operaciones
                ops = sorted(impl_data[M_val].keys())
                total_promedio_times = [impl_data[M_val][op]['Total_Promedio'] for op in ops]

                plt.plot(ops, total_promedio_times,
                         marker='o', linestyle='-', label=f'{impl_name}', color=colors.get(impl_name, 'black'))
        plt.xscale('log')
        plt.xlabel('Número de Operaciones', fontsize=12)
        plt.ylabel('Tiempo Total Promedio (µs)', fontsize=12)
        plt.title(f'Comparativa de Tiempo Total Promedio (M={M_val})', fontsize=14)
        plt.legend(title='Implementación', loc='upper left')
        plt.grid(True, which="both", ls="-", alpha=0.6)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, f'total_promedio_M{M_val}.png'))
        plt.close()
        print(f"Gráfico guardado: {os.path.join(output_dir, f'total_promedio_M{M_val}.png')}")

    # Gráficas para Tiempo Promedio por Operación
    for M_val in all_M_values:
        plt.figure(figsize=(12, 7))
        for impl_name, impl_data in data_by_implementation.items():
            if M_val in impl_data:
                # Obtener operaciones y tiempos para el M_val actual, ordenados por operaciones
                ops = sorted(impl_data[M_val].keys())
                promedio_por_op_times = [impl_data[M_val][op]['Promedio_por_Op'] for op in ops]

                plt.plot(ops, promedio_por_op_times,
                         marker='o', linestyle='-', label=f'{impl_name}', color=colors.get(impl_name, 'black'))
        plt.xscale('log')
        plt.xlabel('Número de Operaciones', fontsize=12)
        plt.ylabel('Tiempo Promedio por Operación (µs)', fontsize=12)
        plt.title(f'Comparativa de Tiempo Promedio por Operación (M={M_val})', fontsize=14)
        plt.legend(title='Implementación', loc='upper left')
        plt.grid(True, which="both", ls="-", alpha=0.6)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, f'promedio_por_op_M{M_val}.png'))
        plt.close()
        print(f"Gráfico guardado: {os.path.join(output_dir, f'promedio_por_op_M{M_val}.png')}")


if __name__ == "__main__":
    implementations = {
        'DoubleHash': 'DoubleHash/promedios_experimentos.txt',
        'LinealProbing': 'LinealProbing/promedios_experimentos.txt',
        'QuadraticHash': 'QuadraticHash/promedios_experimentos.txt',
        'SeparateChaining': 'SeparateChaining/promedios_experimentos.txt',
        'Map': 'map/promedios_experimentos.txt',
        'Set': 'set/promedios_experimentos.txt',
    }

    data_by_implementation = {}
    for impl_name, file_path_relative in implementations.items():
        # Asegurarse de que la ruta sea relativa al script que se está ejecutando
        current_script_dir = os.path.dirname(os.path.abspath(__file__))
        full_filepath = os.path.join(current_script_dir, file_path_relative)

        print(f"Intentando leer datos para {impl_name} desde: {full_filepath}")

        # Parsear el archivo y almacenar los datos
        impl_data = parse_promedios_file(full_filepath)
        if impl_data:
            data_by_implementation[impl_name] = impl_data

    if not data_by_implementation:
        print("No se pudieron cargar datos de ninguna implementación. Asegúrate de que las rutas de los archivos sean correctas y el formato sea el esperado.")
    else:
        generate_comparison_graphs(data_by_implementation)
        print("\nGeneración de gráficas de comparativa completada.")