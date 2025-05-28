import os
import re
import numpy as np
import matplotlib.pyplot as plt

def parse_experiment1(block):
    data = {}
    sections = re.split(r'-{2,}', block)
    for section in sections:
        section = section.strip()
        if not section:
            continue
        m_match = re.search(r'Para M=(\d+)', section)
        if m_match:
            m = int(m_match.group(1))
            total_time = int(re.search(r'Tiempo total= (\d+)', section).group(1))
            avg_time = float(re.search(r'Tiempo promedio por insercion= ([\d.]+)', section).group(1))
            data[m] = {'total': total_time, 'avg': avg_time}
    return data

def parse_experiment2(block):
    data = {}
    sections = re.split(r'-{2,}', block)
    for section in sections:
        section = section.strip()
        if not section:
            continue
        o_match = re.search(r'Para O=(\d+)', section)
        if o_match:
            o = int(o_match.group(1))
            total_time = int(re.search(r'Tiempo total= (\d+)', section).group(1))
            avg_time = float(re.search(r'Tiempo promedio por operacion= ([\d.]+)', section).group(1))
            inserciones = int(re.search(r'inserciones=(\d+)', section).group(1))
            consulta = int(re.search(r'consulta tope=(\d+)', section).group(1))
            obtener = int(re.search(r'obtener tope=(\d+)', section).group(1))
            data[o] = {
                'total': total_time,
                'avg': avg_time,
                'inserciones': inserciones,
                'consulta': consulta,
                'obtener': obtener
            }
    return data

def parse_experiment3(block):
    data = {}
    sections = re.split(r'-{2,}', block)
    for section in sections:
        section = section.strip()
        if not section:
            continue
        n_match = re.search(r'Para N=(\d+)', section)
        if n_match:
            n = int(n_match.group(1))
            ops = {
                'Insercion': r'Insercion: min=(\d+)microseg, max=(\d+)microseg, promedio=([\d.]+)microseg',
                'Consulta': r'Consulta: min=(\d+)microseg, max=(\d+)microseg, promedio=([\d.]+)microseg',
                'Extraccion': r'Extraccion: min=(\d+)microseg, max=(\d+)microseg, promedio=([\d.]+)microseg'
                # 'Union' is intentionally ignored
            }
            op_data = {}
            for op, pattern in ops.items():
                match = re.search(pattern, section)
                if match:
                    op_min = int(match.group(1))
                    op_max = int(match.group(2))
                    op_prom = float(match.group(3))
                    op_data[op.lower()] = {
                        'min': op_min,
                        'max': op_max,
                        'promedio': op_prom
                    }
            data[n] = op_data
    return data

def parse_content(content):
    experiments = {}
    blocks = re.split(r'={5,}', content)
    if len(blocks) >= 4:
        exp1_block = blocks[1].strip()
        experiments['experimento1'] = parse_experiment1(exp1_block)
        exp2_block = blocks[2].strip()
        experiments['experimento2'] = parse_experiment2(exp2_block)
        exp3_block = blocks[3].strip()
        experiments['experimento3'] = parse_experiment3(exp3_block)
    return experiments

def main():
    base_path = "C:/Users/ereis/OneDrive/Documentos/GitHub/Tap-1-2025/Tarea 2 - Heaps/binario"
    file_paths = [os.path.join(base_path, f"binario{i}.txt") for i in range(1, 4)]
    all_data = []
    for path in file_paths:
        if not os.path.exists(path):
            print(f"File {path} not found.")
            return
        with open(path, 'r', encoding='utf-8') as f:
            content = f.read()
            parsed = parse_content(content)
            all_data.append(parsed)
    
    # Average experiments
    averaged = {}
    # Experimento 1
    exp1_list = [d['experimento1'] for d in all_data]
    ms = sorted(exp1_list[0].keys())
    avg_exp1 = {}
    for m in ms:
        totals = [file[m]['total'] for file in exp1_list]
        avgs = [file[m]['avg'] for file in exp1_list]
        avg_exp1[m] = {
            'total': sum(totals)/3,
            'avg': sum(avgs)/3
        }
    averaged['experimento1'] = avg_exp1
    
    # Experimento 2
    exp2_list = [d['experimento2'] for d in all_data]
    os_values = sorted(exp2_list[0].keys())
    avg_exp2 = {}
    for o in os_values:
        totals = [file[o]['total'] for file in exp2_list]
        avgs = [file[o]['avg'] for file in exp2_list]
        ins = [file[o]['inserciones'] for file in exp2_list]
        cons = [file[o]['consulta'] for file in exp2_list]
        obt = [file[o]['obtener'] for file in exp2_list]
        avg_exp2[o] = {
            'total': sum(totals)/3,
            'avg': sum(avgs)/3,
            'inserciones': sum(ins)/3,
            'consulta': sum(cons)/3,
            'obtener': sum(obt)/3
        }
    averaged['experimento2'] = avg_exp2
    
    # Experimento 3
    exp3_list = [d['experimento3'] for d in all_data]
    ns = sorted(exp3_list[0].keys())
    avg_exp3 = {}
    for n in ns:
        ops_data = {}
        for op in ['insercion', 'consulta', 'extraccion']:
            mins = [file[n][op]['min'] for file in exp3_list if op in file[n]]
            maxs = [file[n][op]['max'] for file in exp3_list if op in file[n]]
            proms = [file[n][op]['promedio'] for file in exp3_list if op in file[n]]
            ops_data[op] = {
                'min': sum(mins)/3,
                'max': sum(maxs)/3,
                'promedio': sum(proms)/3
            }
        avg_exp3[n] = ops_data
    averaged['experimento3'] = avg_exp3
    # Guardar datos procesados en archivo de texto
    output_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "procesado.txt")
    with open(output_path, "w", encoding="utf-8") as f:
        f.write("=== Experimento 1 ===\n")
        for m in sorted(averaged['experimento1'].keys()):
            vals = averaged['experimento1'][m]
            f.write(f"M={m}: Tiempo total promedio={vals['total']:.2f}, Tiempo promedio por inserción={vals['avg']:.4f}\n")
        f.write("\n=== Experimento 2 ===\n")
        for o in sorted(averaged['experimento2'].keys()):
            vals = averaged['experimento2'][o]
            f.write(
                f"O={o}: Tiempo total promedio={vals['total']:.2f}, "
                f"Tiempo promedio por operación={vals['avg']:.4f}, "
                f"inserciones={vals['inserciones']:.2f}, "
                f"consulta tope={vals['consulta']:.2f}, "
                f"obtener tope={vals['obtener']:.2f}\n"
            )
        f.write("\n=== Experimento 3 ===\n")
        for n in sorted(averaged['experimento3'].keys()):
            f.write(f"N={n}:\n")
            for op in ['insercion', 'consulta', 'extraccion', 'union']:
                if op in averaged['experimento3'][n]:
                    vals = averaged['experimento3'][n][op]
                    f.write(
                        f"  {op.capitalize()}: min={vals['min']:.2f}µs, "
                        f"max={vals['max']:.2f}µs, promedio={vals['promedio']:.4f}µs\n"
                    )
            f.write("\n")
    # Plotting
    # Experimento 1
    exp1 = averaged['experimento1']
    ms = sorted(exp1.keys())
    avg_times = [exp1[m]['avg'] for m in ms]
    plt.figure()
    plt.plot(ms, avg_times, 'o-')
    plt.xscale('log')
    plt.xlabel('M (log scale)')
    plt.ylabel('Tiempo promedio por inserción (µs)')
    plt.title('Experimento 1: Tiempo promedio de inserción\n Heap Binario')
    plt.grid(True)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    plt.savefig(os.path.join(script_dir, 'experimento1.png'))
    plt.close()
    
    # Experimento 2
    exp2 = averaged['experimento2']
    os_vals = sorted(exp2.keys())
    avg_ops = [exp2[o]['avg'] for o in os_vals]
    plt.figure()
    plt.plot(os_vals, avg_ops, 'o-')
    plt.xscale('log')
    plt.xlabel('O (log scale)')
    plt.ylabel('Tiempo promedio por operación (µs)')
    plt.title('Experimento 2: Tiempo promedio por operación\n Heap Binario')
    plt.grid(True)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    plt.savefig(os.path.join(script_dir, 'experimento2.png'))
    plt.close()
    
    # Experimento 3
    exp3 = averaged['experimento3']
    ns = sorted(exp3.keys())
    ops = ['insercion', 'consulta', 'extraccion']
    op_labels = ['Inserción', 'Consulta', 'Extracción']
    plt.figure(figsize=(10, 6))
    for op, label in zip(ops, op_labels):
        proms = [exp3[n][op]['promedio'] for n in ns]
        plt.plot(ns, proms, 'o-', label=label)
    plt.xscale('log')
    plt.xlabel('N (log scale)')
    plt.ylabel('Tiempo promedio (µs)')
    plt.title('Experimento 3: Tiempo promedio por operación en secuencia\n Heap Binario')
    plt.legend()
    plt.grid(True)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    plt.savefig(os.path.join(script_dir, 'experimento3.png'))
    plt.close()

if __name__ == '__main__':
    main()