# BM@N Statistics

Original version from: https://github.com/loooj58/BMAN

This script must be run with python 3.6+, providing arguments in the command line:

```
python3 stats.py [-h] [--dir [dir]] [--size] [--time]
                [--config [CONFIG]] [--output [OUTPUT]]


optional arguments:
  -h, --help            show this help message and exit
  --dir [dir], -d [dir]
                        Name of directory to explore
  --size, -s            Compute size statistics
  --time, -t            Compute time statistics
  --config [CONFIG], -c [CONFIG]
                        Path to config file, default is ./config.txt
  --output [OUTPUT], -o [OUTPUT]
                        Path to output file, default is ./output.png
```

Other parmeters are specified in JSON config file.

required parameters:
* `extensions_size` - data file extensions
* `extensions_time` - log file extensions
* `db_user`, `db_pass`, `db_name`, `db_host` - database credentials

optional parameters:
* `dpi` - dpi for the generated diagram
* `folders_ignore` - directories that must be ignored while processing

### Installation of the required libraries

Can be done with `pip install -r requirements.txt`.

### Example running from virtual environment

```
git clone -b dev --recursive https://git.jinr.ru/nica/bmnroot.git
cd bmnroot/uni_db/services/statistics/
python3.6 -m venv MYENV
source MYENV/bin/activate
pip install -r requirements.txt

(MYENV) [user@host statistics]$ python3 stats.py --size --dir /eos/nica/bmn/exp/raw/run7/ --config config.json --recursive
```
