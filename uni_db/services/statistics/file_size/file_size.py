import argparse
import matplotlib.pyplot as plt
import numpy as np
import os
import psycopg2
import re
import warnings

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

	def compute(self, _dir, recursive):
		arr, arr_per_event = self.parse_dir(_dir, recursive)

		arr, unit = self.convert_units(arr)
		title = f'File size, {unit}. Mean = {np.mean(arr):.3f} {unit}.'

		arr_per_event, unit_per_event = self.convert_units(arr_per_event)
		title_per_event = f'File size per event, {unit_per_event}. Mean = {np.mean(arr_per_event):.3f} {unit_per_event}.'

		return (arr, unit, title, arr_per_event, unit_per_event, title_per_event)

	def parse_dir(self, _dir, recursive):
		filesize_arr = []
		filesize_per_event = []

		if recursive:
			files_to_walk = os.walk(_dir)
		else:
			files_to_walk = [next(os.walk(_dir))]

		for root, dirs, files in files_to_walk:
			for file in files:
				if self.is_file_to_parse(root, file):
					filesize_bytes = os.stat(os.path.join(root, file)).st_size
					run_num = re.search(config.RUN_NUM_REGEX, file)
					if run_num is None:
						warnings.warn(f'No run number found in filename for file {os.path.join(root, file)}')
					else:
						run_num = run_num.group()
						run_count = self.get_events_count(run_num)
						if run_count is None:
							warnings.warn(f'No run number found in database for file {os.path.join(root, file)}')
						else:
							filesize_arr.append(filesize_bytes)
							filesize_per_event.append(filesize_bytes / run_count)
		if filesize_arr == []:
			raise Exception("No data")
		return np.array(filesize_arr), np.array(filesize_per_event)


	def is_file_to_parse(self, root, file):
		filepath = os.path.join(root, file)
		correct_ext = any([ext in file for ext in self.EXTENSIONS])
		correct_folder = all([elem not in os.path.join(root, file) for elem in self.FOLDERS_IGNORE])
		return correct_ext and correct_folder

	def convert_units(self, arr):
		mean = np.mean(arr)
		for i, unit in enumerate(config.UNITS):
			if mean / config.SIZE**i < config.SIZE:
				break
				arr = arr / (config.SIZE**i)
		return arr, unit


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
