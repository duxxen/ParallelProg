import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import subprocess
import re

path = 'C:\\Users\\Boom\\Desktop\\ParallelProgramming\\'
lab_names = ['MPI', 'OMP', 'PTHREADS']
exe_names = ['lab1.exe', 'lab2.exe', 'lab3.exe']

threads_count = [2, 4, 8, 12]

tests = {
    '10': 1073676287,
    '13': 4398042316799,
    '16': 1125899839733759,
    '19': 2305843009213693951,
}
sizes = [size for size in tests]

lab_pathes = { lab_names[i]: path + exe_names[i] for i in range(len(lab_names)) }

exec_results = dict.fromkeys(lab_names, dict)
exec_relative = dict.fromkeys(lab_names, dict)

def execute_programms(results_dict: dict):
    for lname, results in results_dict.items():
        for tcount, exectime in results.items():
            if lname == 'MPI': cmd = f'mpiexec -n {tcount} {lab_pathes[lname]} '
            else:              cmd = f'{lab_pathes[lname]} {tcount} '

            for i in range(len(tests)):
                value = tests[sizes[i]]
                out = subprocess.run(cmd + f'{value}', capture_output=True).stdout
                time = float(re.findall(r'Time: (\d+.\d*)', str(out))[0])
                exectime[i] = time


def plot_results(results_dict: dict):
    fig, axs = plt.subplots(3, 1, figsize=(10, 8))

    for lname, results in results_dict.items():
        index = lab_names.index(lname)
        for tcount, exectime in results.items():
            axs[index].plot(sizes, exectime, marker='o', label=f'Потоков: {tcount}')
        
        axs[index].legend()
        axs[index].set_yscale('log')
        axs[index].set_ylabel('Время выполнения')
        axs[index].set_title(f'Библиотека {lname}')

    plt.xlabel('Порядок числа')
        

def barh_results(results_dict: dict):
    plt.figure(figsize=(10, 8))

    y_pos = np.arange(len(threads_count)) - 0.3
    for lname, results in results_dict.items():
        values = []
        for tcount, time in results.items():
            values.append(time)
        plt.barh(y_pos, values, height=0.2, label=lname)
        y_pos += 0.3

    plt.legend()
    plt.xlabel('Относительное время выполнения, %')

    plt.yticks(y_pos - 0.6, threads_count)
    plt.ylabel('Количество потоков')

if __name__ == '__main__':
    for lname in lab_names:
        results = dict.fromkeys(threads_count)
        rel_results = results.copy()

        for tcount in threads_count:
            results[tcount] = [None] * len(tests)
            rel_results[tcount] = None

        exec_results[lname] = results
        exec_relative[lname] = rel_results
    
    execute_programms(exec_results)

    itest = len(tests) - 1
    maxtime = exec_results['MPI'][12][itest]
    for lname in lab_names:
        for tcount in threads_count:
            exec_relative[lname][tcount] = exec_results[lname][tcount][itest] / maxtime * 100

    plot_results(exec_results)
    barh_results(exec_relative)
    
    print(pd.DataFrame(exec_results))
    print(pd.DataFrame(exec_relative))

    plt.show()
    