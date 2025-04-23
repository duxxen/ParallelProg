import re
import random
import subprocess
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from tqdm import tqdm

EXEPATH = 'ParallelProgramming.exe'
INPATH = 'inputs\\'
OUTPATH = 'outputs\\'

def generate_matrix(m: int, n: int, rand_range = (-100, 100)):
    return [[random.randint(*rand_range) for _ in range(m)] for _ in range(n)]

def generate_and_save(n: int, filename: str):
    A = generate_matrix(n, n)
    b = generate_matrix(n, 1)
    with open(filename, 'w') as f:
        f.write(' '.join(f"{val}" for val in b) + '\n')
        for row in A:
            f.write(' '.join(f"{val}" for val in row) + '\n')

def draw_data(threads: list, results: dict, size: int, filename: str):
    plt.figure()
    plt.bar(threads, [int(results[size][t]) for t in threads])
    plt.xlabel('Количество потоков')
    plt.ylabel('Время выполнения, мс')
    plt.title(f'Размер матрицы {size}')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(filename)

if __name__ == '__main__':
    sizes = [2000, 3000, 4000, 5000]
    threads = [0, 1, 2, 4, 6, 8, 10, 12]
    
    inputs, images, results = {}, {}, {}
    for size in sizes:
        inputs[size] = f'{INPATH}input_{size}.txt'
        images[size] = f'{OUTPATH}image_{size}.png'
        results[size] = {}
        for thread in threads:
            results[size][thread] = 1.0
    
    for size in sizes:
        print(f'Generating {size}x{size}...')
        generate_and_save(size, inputs[size])

    time_pattern = re.compile(r'Time:\s(\d+)')
    for size in sizes:
        print(f'Solving {size}x{size}...')
        for thread in tqdm(threads):
            res = subprocess.run(f'{EXEPATH} {inputs[size]} {thread}', capture_output=True).stdout.decode()
            results[size][thread] = int(time_pattern.findall(res)[0])

    for size in sizes:
        print(f'Drawing {size}x{size}...')
        draw_data(threads, results, size, images[size])
    pd.DataFrame(results).to_string(f'{OUTPATH}results.txt')
    plt.show()
    
    

                