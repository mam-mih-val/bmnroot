import matplotlib.pyplot as plt
import numpy as np
import subprocess

from log_time.log_time import TimeStatComputer
from file_size.file_size import SizeStatComputer
import file_size.config as config_size
import log_time.config as config_time

from exceptions import NoDataException

def plot_all_stats(size, time, config_dict, _dir, output, recursive):
    plt.clf()
    if size and time:
        # data = {}
        print("Calculating both size and time statistics...")
        computer = SizeStatComputer(config_dict)
        arr, unit, title, arr_per_event, unit_per_event, title_per_event \
            = computer.compute(_dir, recursive)
        
        computer_time = TimeStatComputer(config_dict)
        arr_time, unit_time, title_time, arr_per_event_time, unit_per_event_time, title_per_event_time \
            = computer_time.compute(_dir, recursive)

        fig, axs = plt.subplots(nrows=2, ncols=2, figsize=(20, 10))

        axs[0][0].set_title(title)
        axs[0][0].hist(arr, bins=config_size.BINS)
        axs[0][0].axvline(np.mean(arr), linestyle='dashed', linewidth=1)

        axs[0][1].set_title(title_per_event)
        axs[0][1].hist(arr_per_event, bins=config_size.BINS_PER_EVENT)
        axs[0][1].axvline(np.mean(arr_per_event), linestyle='dashed', linewidth=1)

        axs[1][0].set_title(title_time)
        axs[1][0].hist(arr_time, bins=config_time.BINS)
        axs[1][0].axvline(np.mean(arr_time), linestyle='dashed', linewidth=1)

        axs[1][1].set_title(title_per_event_time)
        axs[1][1].hist(arr_per_event_time, bins=config_time.BINS_PER_EVENT)
        axs[1][1].axvline(np.mean(arr_per_event_time), linestyle='dashed', linewidth=1)

    elif size:
        print("Calculating file size statistics...")
        computer = SizeStatComputer(config_dict)
        try:
            arr, unit, title, arr_per_event, unit_per_event, title_per_event \
                = computer.compute(_dir, recursive)
        except NoDataException:
            print("\nNo data to display - returning!")
            return        

        fig, axs = plt.subplots(nrows=1, ncols=2, figsize=(20, 10))

        axs[0].set_title(title)
        axs[0].hist(arr, bins=config_size.BINS)
        axs[0].axvline(np.mean(arr), linestyle='dashed', linewidth=1)

        axs[1].set_title(title_per_event)
        axs[1].hist(arr_per_event, bins=config_size.BINS_PER_EVENT)
        axs[1].axvline(np.mean(arr_per_event), linestyle='dashed', linewidth=1)

    elif time:
        print("Calculating time statistics...")
        computer = TimeStatComputer(config_dict)
        try:
            arr, unit, title, arr_per_event, unit_per_event, title_per_event \
                 = computer.compute(_dir, recursive)
        except NoDataException:
            print("\nNo data to display - returning!")
            return

        fig, axs = plt.subplots(nrows=1, ncols=2, figsize=(20, 10))

        axs[0].set_title(title)
        axs[0].hist(arr, bins=config_time.BINS)
        axs[0].axvline(np.mean(arr), linestyle='dashed', linewidth=1)

        axs[1].set_title(title_per_event)
        axs[1].hist(arr_per_event, bins=config_time.BINS_PER_EVENT)
        axs[1].axvline(np.mean(arr_per_event), linestyle='dashed', linewidth=1)

    else:
        raise Exception('No stats to compute')

    if output is None:
        plt.show()
    else:
        print(f"\nWriting output to {output}")
        plt.savefig(output, dpi=computer.DPI)
        try:
            print("Trying to open graphics...", end="", flush=True)
            subprocess.getoutput(f"xdg-open {output}")
            print("   ...ok")
        except KeyboardInterrupt:
            print("   ...finishing after ^C")
        except:
            print("   ...unsuccessful, please open it yourself.")

