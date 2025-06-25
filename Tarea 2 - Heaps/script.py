import subprocess
import os
# Lista de archivos fuente .cpp
cpp_files = ["binario.cpp", "binomial.cpp", "fibonacci.cpp"] #"binomial.cpp", "fibonnachi.cpp"
seeds = [123456789, 9876, 555]
numeros = [100, 500, 1000, 5000, 10000, -1] #

# Compila cada archivo .cpp en un ejecutable con el mismo nombre base
for cpp_file in cpp_files:
    exe_name = os.path.splitext(cpp_file)[0] + ".exe"
    compile_cmd = ["g++", cpp_file, "-o", exe_name]
    result = subprocess.run(compile_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        print(f"Error compilando {cpp_file}:\n{result.stderr.decode()}")
        
cpp_executables = ["binario.exe", "binomial.exe", "fibonacci.exe"] #, "binomial.exe", "fibonnachi.exe"

entradas = [f"{seed} {' '.join(map(str, numeros))}\n" for seed in seeds]

# Ejecuta los 3 programas y redirige sus salidas, repitiendo 3 veces para cada uno
for cpp_executable in cpp_executables:
    base_name = os.path.splitext(cpp_executable)[0]
    # Crea una carpeta con el nombre del archivo .cpp (sin extensión) si no existe
    output_dir = base_name
    os.makedirs(output_dir, exist_ok=True)
    for i in range(1, 4):
        salida_txt = os.path.join(output_dir, f"{base_name}{i}.txt")
        with open(salida_txt, "w") as outfile:
            entrada = entradas[i - 1]
            proceso = subprocess.run(
                [cpp_executable],
                input=entrada.encode(),
                stdout=outfile,
                stderr=subprocess.PIPE
            )
        if proceso.returncode != 0:
            print(f"Error al ejecutar el programa {cpp_executable}:", proceso.stderr.decode())
    
    # Ejecuta procesamiento.py en el directorio de salida
    procesamiento_path = os.path.join(output_dir, "procesamiento.py")
    if os.path.exists(procesamiento_path):
        result = subprocess.run(
            ["python", "procesamiento.py"],
            cwd=output_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        if result.returncode != 0:
            print(f"Error al ejecutar procesamiento.py en {output_dir}:\n{result.stderr.decode()}")
    else:
        print(f"No se encontró procesamiento.py en {output_dir}")