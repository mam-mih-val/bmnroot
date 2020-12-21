import argparse
import matplotlib.pyplot as plt
import numpy as np
import os
import psycopg2
import re

import file_size.config as config


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

        files_parsed = 0
        for root, dirs, files in files_to_walk:
            for file in files:
                if self.is_file_to_parse(root, file):
                    filesize_bytes = os.stat(os.path.join(root, file)).st_size
                    if self.FILE_SIZE_LIMIT_LOW is not None and self.FILE_SIZE_LIMIT_HIGH is not None:
                        if filesize_bytes < self.FILE_SIZE_LIMIT_LOW or filesize_bytes > self.FILE_SIZE_LIMIT_HIGH:
                            filesize_conv, filesize_units = self.convert_units_scalar(filesize_bytes)
                            print(f"\nFile {os.path.join(root, file)} is {filesize_conv:.1f} {filesize_units} "\
                                    f"which does not meet file size limit - skipping.")
                            continue
                    run_num = re.search(config.RUN_NUM_REGEX, file)
                    if run_num is None:
                        print(f"\nNo run number found in filename for file {os.path.join(root, file)}")
                        continue
                    run_num = run_num.group()
                    run_count = self.get_events_count(run_num)
                    if run_count is None:
                        print(f"\nNo run number {run_num} found in database for file {os.path.join(root, file)}")
                        continue
                    filesize_bytes_per_event = filesize_bytes / run_count
                    if self.EVENT_SIZE_LIMIT_LOW is not None and self.EVENT_SIZE_LIMIT_HIGH is not None:
                        if filesize_bytes_per_event < self.EVENT_SIZE_LIMIT_LOW or filesize_bytes_per_event > self.EVENT_SIZE_LIMIT_HIGH:
                            eventsize_conv, eventsize_units = self.convert_units_scalar(filesize_bytes_per_event)
                            print(f"\nFile {os.path.join(root, file)} has {eventsize_conv:.1f} {eventsize_units} per event "\
                                    f"which does not meet event size limit - skipping.")
                            continue
                    files_parsed += 1
                    print("+", end="", flush=True)
                    filesize_arr.append(filesize_bytes)
                    filesize_per_event.append(filesize_bytes_per_event)
        print()
        print(f"Totally parsed {files_parsed} files")
        if filesize_arr == []:
            raise Exception("No data")
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


    def get_events_count(self, run_num):
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
