import argparse
import datetime
import matplotlib.pyplot as plt
import numpy as np
import os
import psycopg2
import re

import log_time.config as config

from exceptions import NoDataException


class TimeStatComputer:
    def __init__(self, config_dict):

        self.EXTENSIONS = config_dict.get('extensions')
        self.DB_USER = config_dict.get('db_user')
        self.DB_PASS = config_dict.get('db_pass')
        self.DB_NAME = config_dict.get('db_name')
        self.DB_HOST = config_dict.get('db_host')

        self.DPI = config_dict.get('dpi', config.DPI)
        self.FOLDERS_IGNORE = config.FOLDERS_IGNORE
        self.FOLDERS_IGNORE.extend(config_dict.get('folders_ignore', config.FOLDERS_IGNORE))


    # ### Old implementation ###
    # def get_events_count(self, run_num):
    #     conn = psycopg2.connect(dbname=self.DB_NAME, user=self.DB_USER, 
    #                             password=self.DB_PASS, host=self.DB_HOST)
    #     cursor = conn.cursor()
    #     cursor.execute(f'SELECT event_count FROM run_ WHERE run_number = {run_num}')
    #     count = cursor.fetchone()
    #     if count is None:
    #         return None
    #     count = count[0]
    #     cursor.close()
    #     conn.close()
    #     return count

    def get_events_count(self, run_num):
        """ Returns events_count from DB (or None) and a warning flag (if there where multiple runs with same num) """
        conn = psycopg2.connect(dbname=self.DB_NAME, user=self.DB_USER, 
                                password=self.DB_PASS, host=self.DB_HOST)
        cursor = conn.cursor()
        cursor.execute(f"SELECT period_number, event_count FROM run_ WHERE run_number = {run_num}")
        resp = cursor.fetchall()
        cursor.close()
        conn.close()
        # print(resp)
        if resp is None or len(resp) == 0:
            return (None, False) 
        if len(resp) == 1:
            count = resp[0][1]
            return (count, False)
        else:
            # Look for maximum period number with given run_num
            count = max(resp, key = lambda tup: tup[0])[1]
            return (count, True)
        

    def is_file_to_parse(self, root, file):
        # filepath = os.path.join(root, file)
        correct_ext = any([file.endswith(ext) for ext in self.EXTENSIONS]) or ("*" in self.EXTENSIONS)
        correct_folder = all([elem not in os.path.join(root, file) for elem in self.FOLDERS_IGNORE])
        return correct_ext and correct_folder


    def parse_dir(self, _dir, recursive):
        time_arr = []
        time_per_events_arr = []
        unsuccessful_arr = []
        unsuccessful_arr_files = []
        if recursive:
            files_to_walk = os.walk(_dir)
        else:
            files_to_walk = [next(os.walk(_dir))]

        files_parsed = 0
        for root, dirs, files in files_to_walk:
            for file in files:
                if self.is_file_to_parse(root, file):
                    try:
                        time, is_successful, run_num, file_processing = self.parse_time(os.path.join(root, file))
                    except PermissionError:
                        print(f"Permission denied - skipping file {os.path.join(root, file)}")
                        continue
                    except Exception as ex:
                        print(f"Error {ex} while processing - skipping file {os.path.join(root, file)}")
                        continue
                    files_parsed += 1
                    print("+", end="", flush=True)
                    if time is not None:
                        if run_num is not None:
                            if is_successful == True:
                                time_arr.append(time)
                                events_count, warn_flag = self.get_events_count(run_num)
                                if events_count is None or events_count == 0:
                                    print(f"\nCan't get events count from the database - skipping file {os.path.join(root, file)}")
                                else:
                                    if warn_flag:
                                        print(f"Warning: more than one run records: the latest period number is selected for {os.path.join(root, file)}")
                                    time_per_events_arr.append(time / events_count)
                            else:
                                print(f"\n File has time and run number, but not ended successfully - skipping {os.path.join(root, file)}")
                        elif is_successful == True:
                            print(f"\nCan not parse run number in successfully ended log file {os.path.join(root, file)}")
                    elif is_successful == True:
                        print(f"\nCan not parse time in successfully ended log file {os.path.join(root, file)}")
                    if is_successful == False:
                        unsuccessful_arr.append(os.path.join(root, file))
                        if file_processing != None:
                            unsuccessful_arr_files.append(file_processing)
                        else:
                            print(f"\nUnsuccessfully processed file name was not defined in {os.path.join(root, file)}.")
        if time_arr == []:
            raise NoDataException

        print(f"\n\nTotal files parsed: {files_parsed}")
        return np.array(time_arr), np.array(time_per_events_arr), unsuccessful_arr, unsuccessful_arr_files, files_parsed


    def compute(self, _dir, recursive):
        arr, arr_per_event, unsuccessful_arr, unsuccessful_arr_files, files_parsed = self.parse_dir(_dir, recursive)
        arr, unit = self.convert_units(arr)
        title = f"Time, {unit}. Mean = {np.mean(arr):.3f} {unit}, overall {len(arr)} files."

        arr_per_event, unit_per_event = self.convert_units(arr_per_event)
        title_per_event = f"Time per event, {unit_per_event}. Mean = {np.mean(arr_per_event):.3f} {unit_per_event}, " \
                f"overall {len(arr_per_event)} files."

        print()
        if len(unsuccessful_arr) == 0:
            print("All runs ended successfully.\n")
        else:
            print("Unsuccessfully ended runs:")
            for elem in unsuccessful_arr:
                print(elem)
            if config.UNSUCCESSFUL_LOG_FILE is not None:
                print()
                with open(config.UNSUCCESSFUL_LOG_FILE, "wt") as f:
                    for elem in sorted(unsuccessful_arr_files):
                        f.write(elem + "\n")
                print(f"Unsuccessfully processed files list ({len(unsuccessful_arr_files)}/{files_parsed}, {(100*len(unsuccessful_arr_files)/files_parsed):.1f}%)"\
                    f" was saved to {config.UNSUCCESSFUL_LOG_FILE}")

        print()
        print("Obtained characteristics:")
        print(f"  Mean time = {np.mean(arr):.3f} {unit}.")
        print(f"  Summary time = {np.sum(arr):.3f} {unit}.")
        print(f"  Mean time per event = {np.mean(arr_per_event):.3f} {unit_per_event}.")

        return (arr, unit, title, arr_per_event, unit_per_event, title_per_event)


    def convert_units(self, arr):
        mean = np.mean(arr)
        u = len(config.UNITS) - 1
        for i, unit in enumerate(config.UNITS):
            mean = mean / config.SIZE[i]
            if mean < config.SIZE[i+1]:
                u = i
                break
        for i in range (u + 1):
            arr = arr / config.SIZE[i]
        return arr, config.UNITS[u]


    def convert_month(self, month):
        res = config.MONTH_ARR[month]
        return res


    def get_date(self, result):
        year = int(result[7])
        month = self.convert_month(result[3])
        day = int(result[4])
        time = result[5].split(':')
        hour = int(time[0])
        minute = int(time[1])
        second = int(time[2])
        return year, month, day, hour, minute, second


    def parse_time(self, log_file):
        is_successful = False
        start = None
        end = None
        run_num = None
        file_processing = None
        with open(log_file, 'r') as f:
            for line in f:
                if config.START in line:
                    result = re.search(config.START_REGEX, line).group().split()
                    y, m, d, h, minute, s = self.get_date(result)
                    start = datetime.datetime(y, m, d, hour=h, minute=minute, second=s)
                if config.END in line:
                    result = re.search(config.END_REGEX, line).group().split()
                    y, m, d, h, minute, s = self.get_date(result)
                    end = datetime.datetime(y, m, d, hour=h, minute=minute, second=s)
                if config.SUCCESS in line:
                    is_successful = True
                if config.RUN_EXTENSION in line and run_num == None:
                    result = re.search(config.RUN_REGEX, line)
                    if result != None:
                        run_num = int(result.group()[7:])
                if file_processing == None:
                    if line.startswith(config.PROCESSING_LINE_START):
                        result = re.search(config.PROCESSING_REGEX, line)
                        if result != None:
                            if len(result.groups()) > 0:
                                file_processing = result.groups()[0]
                            # print("file_processing = ", file_processing)
        if start == None or end == None:
            delta = None
        else:
            delta = (end - start).total_seconds()
        # TODO add verbose output option
        #print(f"Log file {log_file}, delta {delta}, is_successful {is_successful}, run_num {run_num}")

        return delta, is_successful, run_num, file_processing
