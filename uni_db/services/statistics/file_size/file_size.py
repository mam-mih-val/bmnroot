import argparse
import matplotlib.pyplot as plt
import numpy as np
import os
import psycopg2
import re
import uproot

import file_size.config as config

from exceptions import NoDataException


class SizeStatComputer:
    def __init__(self, config_dict):

        self.EXTENSIONS = config_dict.get('extensions')
        self.DB_USER = config_dict.get('db_user')
        self.DB_PASS = config_dict.get('db_pass')
        self.DB_NAME = config_dict.get('db_name')
        self.DB_HOST = config_dict.get('db_host')

        self.DPI = config_dict.get('dpi', config.DPI)
        self.FOLDERS_IGNORE = config.FOLDERS_IGNORE
        self.FOLDERS_IGNORE.extend(config_dict.get('folders_ignore', config.FOLDERS_IGNORE))

        self.FILE_SIZE_LIMIT_LOW, self.FILE_SIZE_LIMIT_HIGH = self.extract_size_limits(config_dict.get('file_size_limit'))
        self.EVENT_SIZE_LIMIT_LOW, self.EVENT_SIZE_LIMIT_HIGH = self.extract_size_limits(config_dict.get('event_size_limit'))

        self.SOURCE = (config_dict.get('source') or "exp").lower()  # exp or sim
        

    def extract_size_limits(self, limit_str):
        if limit_str is None or limit_str == "":
            return None, None
        units = {"": 1, "B": 1, "KB": 1024, "MB": 1024*1024, "GB": 1024*1024*1024, "TB": 1024*1024*1024*1024}
        try:
            res = re.search("(\d+)(\w*)\s*:\s*(\d+)(\w*)", limit_str.strip())
            gr = res.groups()
            if len(gr) != 4:
                raise Exception("Wrong string specifying file size limits")
            limit_min = int(gr[0])
            units_min = gr[1].upper()
            limit_max = int(gr[2])
            units_max = gr[3].upper()
            if units_min not in units or units_max not in units:
                raise Exception("Wrong string specifying file size limits")
            return limit_min * units[units_min], limit_max * units[units_max]
        except:
            print(f"\n\nWarning: Could not extract size limits from {limit_str}")
            return None, None        


    def compute(self, _dir, recursive):
        arr, arr_per_event = self.parse_dir(_dir, recursive)

        arr, unit = self.convert_units(arr)
        title = f"File size, {unit}. Mean = {np.mean(arr):.3f} {unit}. Overall {len(arr)} files."

        print()
        print("Obtained characteristics:")
        print(f"  File statistics: min = {np.min(arr):.3f} {unit}, avg = {np.mean(arr):.3f} {unit}, " \
                f"max={np.max(arr):.3f} {unit}, summary={np.sum(arr):.3f} {unit}")

        arr_per_event, unit_per_event = self.convert_units(arr_per_event)
        title_per_event = f"File size per event, {unit_per_event}. Mean = {np.mean(arr_per_event):.3f} {unit_per_event}. " \
                f"Overall {len(arr_per_event)} files."

        print(f"  File statistics per event: min = {np.min(arr_per_event):.3f} {unit_per_event}, " +
            f"avg = {np.mean(arr_per_event):.3f} {unit_per_event}, max={np.max(arr_per_event):.3f} {unit_per_event}")

        return (arr, unit, title, arr_per_event, unit_per_event, title_per_event)


    def parse_dir(self, _dir, recursive):
        filesize_arr = []
        filesize_per_event = []

        if recursive:
            files_to_walk = os.walk(_dir)
        else:
            files_to_walk = [next(os.walk(_dir))]

        files_parsed_successful = 0
        files_parsed_overall = 0
        unsuccessful_list = []
        for root, dirs, files in files_to_walk:
            for file in files:
                if self.is_file_to_parse(root, file):
                    files_parsed_overall += 1
                    file_path = os.path.join(root, file)
                    filesize_bytes = os.stat(file_path).st_size
                    if self.FILE_SIZE_LIMIT_LOW is not None and self.FILE_SIZE_LIMIT_HIGH is not None:
                        if filesize_bytes < self.FILE_SIZE_LIMIT_LOW or filesize_bytes > self.FILE_SIZE_LIMIT_HIGH:
                            filesize_conv, filesize_units = self.convert_units_scalar(filesize_bytes)
                            print(f"\nFile {file_path} is {filesize_conv:.1f} {filesize_units} "\
                                    f"which does not meet file size limit - skipping.")
                            unsuccessful_list.append(file_path)
                            continue
                    run_num = re.search(config.RUN_NUM_REGEX, file)
                    if run_num is None:
                        print(f"\nNo run number found in filename for file {file_path}")
                        unsuccessful_list.append(file_path)
                        continue
                    run_num = run_num.group()
                                        
                    if self.SOURCE == "exp":
                        run_count = self.get_events_count_by_run(run_num)
                    else:  # "sim"
                        run_count = self.get_events_count_sim(file_path)
                        # print(f"Got {run_count} events in simulation file {file_path}")
                                        
                    if run_count is None:
                        print(f"\nNo run number {run_num} found in database for file {file_path}")
                        unsuccessful_list.append(file_path)
                        continue
                    
                    if self.SOURCE == "exp":
                        uproot_count = self.uproot_event_count(file_path)
                        # if None, file is probably not a root file at all
                        if uproot_count != None and uproot_count != run_count:
                            print(f"\nFile {file_path} has {uproot_count} events but DB reports {run_count} events - skipping.")
                            unsuccessful_list.append(file_path)
                            continue
                    
                    filesize_bytes_per_event = filesize_bytes / run_count
                    if self.EVENT_SIZE_LIMIT_LOW is not None and self.EVENT_SIZE_LIMIT_HIGH is not None:
                        if filesize_bytes_per_event < self.EVENT_SIZE_LIMIT_LOW or filesize_bytes_per_event > self.EVENT_SIZE_LIMIT_HIGH:
                            eventsize_conv, eventsize_units = self.convert_units_scalar(filesize_bytes_per_event)
                            print(f"\nFile {file_path} has {eventsize_conv:.1f} {eventsize_units} per event "\
                                    f"which does not meet event size limit - skipping.")
                            unsuccessful_list.append(file_path)
                            continue
                    files_parsed_successful += 1
                    print("+", end="", flush=True)
                    filesize_arr.append(filesize_bytes)
                    filesize_per_event.append(filesize_bytes_per_event)
        print()
        print(f"Total files parsed: {files_parsed_successful}")
        if filesize_arr == []:
            raise NoDataException
        unsuccessful_list.sort()
        if len(unsuccessful_list) == 0:
            print("\nAll files processed successfully.\n")
        else:
            print("\nUnsuccessfully processed files:")
            for elem in unsuccessful_list:
                print(elem)
            if config.UNSUCCESSFUL_LOG_FILE is not None:
                print()
                with open(config.UNSUCCESSFUL_LOG_FILE, "wt") as f:
                    for elem in unsuccessful_list:
                        f.write(elem + "\n")
                print(f"Unsuccessfully processed files list ({len(unsuccessful_list)}/{files_parsed_overall}, {(100*len(unsuccessful_list)/files_parsed_overall):.1f}%)"\
                    f" was saved to {config.UNSUCCESSFUL_LOG_FILE}\n")
        return np.array(filesize_arr), np.array(filesize_per_event)


    def is_file_to_parse(self, root, file):
        # filepath = os.path.join(root, file)
        correct_ext = any([file.endswith(ext) for ext in self.EXTENSIONS]) or ("*" in self.EXTENSIONS)
        correct_folder = all([elem not in os.path.join(root, file) for elem in self.FOLDERS_IGNORE])
        return correct_ext and correct_folder


    def convert_units(self, arr):
        mean = np.mean(arr)
        for i, unit in enumerate(config.UNITS):
            if mean / config.SIZE**i < config.SIZE:
                break
        arr = arr / (config.SIZE**i)
        return arr, unit


    def convert_units_scalar(self, num):
        for i, unit in enumerate(config.UNITS):
            if num / config.SIZE**i < config.SIZE:
                break
        res = num / (config.SIZE**i)
        return res, unit


    def get_events_count_by_run(self, run_num):
        conn = psycopg2.connect(dbname=self.DB_NAME, user=self.DB_USER, 
                                password=self.DB_PASS, host=self.DB_HOST)
        cursor = conn.cursor()
        cursor.execute(f'SELECT event_count FROM run_ WHERE run_number = {run_num}')
        count = cursor.fetchone()
        if count is None:
            return None
        count = count[0]
        cursor.close()
        conn.close()
        return count


    def get_events_count_sim(self, file_path):
        conn = psycopg2.connect(dbname=self.DB_NAME, user=self.DB_USER, 
                                password=self.DB_PASS, host=self.DB_HOST)
        cursor = conn.cursor()
        cursor.execute(f"SELECT event_count FROM simulation_file WHERE file_path='{file_path}'")
        count = cursor.fetchone()
        if count is None:
            return None
        count = count[0]
        cursor.close()
        conn.close()
        return count


    def uproot_event_count(self, file_path):
        try:
            r = uproot.open(file_path)
        except ValueError:
            return None
        bmndata = list(filter(lambda x: x.startswith("bmndata;"), r.keys()))
        if len(bmndata) not in [1, 2]:
            return None
        second_num = sorted(list(map(lambda s: int(s[8:]), bmndata)))[-1]
        return r['bmndata;' + str(second_num)].member('fEntries') - 1
