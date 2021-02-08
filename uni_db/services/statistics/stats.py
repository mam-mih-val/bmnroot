import sys
if sys.version_info[0] < 3:
    print("Must be using Python 3, exiting!")
    sys.exit(1)

import argparse
import json
import warnings
import os

from file_size.file_size import SizeStatComputer
from log_time.log_time import TimeStatComputer
from utils import plot_all_stats

def main():
    print("Statistics calculation script started.")
    parser = argparse.ArgumentParser(
        description='Script for file size and time statistics.\
        For more info see README.md file'
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
        help='Compute file size statistics'
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
        help='Path to JSON config file',
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
        args.dir[0], args.size, args.time, args.config[0], args.output, args.recursive

    if not os.path.isdir(_dir):
        print("Error: Directory {} not found!".format(_dir))
        sys.exit(1)

    if not os.path.isfile(config):
        print("Error: Config file {} not found!".format(config))
        sys.exit(1)

    config_dict = json.load(open(config, 'r'))

    plot_all_stats(size, time, config_dict, _dir, output, recursive)

    print("Script execution finished.")


if __name__ == '__main__':
    main()
