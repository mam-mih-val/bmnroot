import argparse
import json
import warnings

from file_size.file_size import SizeStatComputer
from log_time.log_time import TimeStatComputer
from utils import plot_all_stats


def main():
	parser = argparse.ArgumentParser(
		description='Script for size and time statistics.\
		For more info see https://github.com/loooj58/BMAN'
	)
	parser.add_argument(
		'--dir', '-d',
		nargs=1,
		type=str,
		help='Name of directory to explore',
		required=True
	)
	parser.add_argument(
		'--size', '-s',
		action='store_true',
		help='Compute size statistics'
	)
	parser.add_argument(
		'--time', '-t',
		action='store_true',
		help='Compute time statistics'
	)
	parser.add_argument(
		'--recursive', '-r',
		action='store_true',
		help='Recursive data search'
	)
	parser.add_argument(
		'--config', '-c',
		nargs=1,
		type=str,
		help='Path to config file, default is ./config.txt',
		required=True
	)
	parser.add_argument(
		'--output', '-o',
		nargs='?',
		type=str,
		help='Path to output file, default is ./output.png',
		default='./output.png'
		)

	args = parser.parse_args()
	_dir, size, time, config, output, recursive = \
	args.dir[0], args.size, args.time,\
	args.config[0], args.output, args.recursive

	config_dict = json.load(open(config, 'r'))

	plot_all_stats(size, time, config_dict, _dir, output, recursive)


if __name__ == '__main__':
	main()