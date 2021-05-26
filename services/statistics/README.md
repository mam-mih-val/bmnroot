# BM@N Statistics

Pyton script to calculate file size and processing time statistics. Original version from: https://github.com/loooj58/BMAN

This script must be run with python 3.6+, providing arguments in the command line:

```
python3 stats.py [-h] [--size | --time] --dir [dir] --config [CONFIG] 
                 [--output [OUTPUT]] [--recursive]

optional arguments:
  -h, --help              Show this help message and exit
  --size, -s              Compute file size statistics
  --time, -t              Compute time statistics
  --dir [dir], -d [dir]   Name of directory to explore
  --config [CONFIG], -c [CONFIG]
                          Path to JSON config file
  --output [OUTPUT], -o [OUTPUT]
                          Path to output file, default is ./output.png
  --recursive             Recursive data search
```

Other parmeters are specified in JSON config file.

required JSON file parameters:
* `extensions` - data or log file extensions (use ["*"] to include all files)
* `db_user`, `db_pass`, `db_name`, `db_host` - database credentials

optional JSON file parameters:
* `dpi` - dpi for the generated diagram
* `folders_ignore` - directories that must be ignored while processing
* `file_size_limit` - constrain min/max size of files to be processed (for example, `50kb:500gb`)
* `event_size_limit` - constrain min/max file size per event (for example, `1kb:1mb`)
* `source` - `exp` (default, for experimental files) or `sim` (for simulated files). For `exp`, event count is taken from `run_` table based on run number extracted from the file name. For `sim`, event count is taken from `simulation_file` table based on full file path string.


### Installation of the required libraries

Can be done with `pip install -r requirements.txt`.


### Examples running the script

Time:
```
python3 stats.py --time --dir /eos/nica/bmn/users/gertsen/logs/batch_raw_run5/ --config config-time.json
```
Size - exp:
```
python3 stats.py --size --dir /eos/nica/bmn/exp/dst/run7/prerelease/4720-5186_BMN_Krypton/ --config config-size.json --recursive
```
Size - sim:
```
python3 stats.py --size --dir /eos/nica/bmn/sim/gen/DCMQGSM/CC_3.5_mb_20k/ --config config-size-sim.json --recursive
```

See also the `examples` sub-directory.


### Example running from virtual environment

```
git clone -b dev --recursive https://git.jinr.ru/nica/bmnroot.git
cd bmnroot/uni_db/services/statistics/
python3.6 -m venv myenv
source myenv/bin/activate
pip install -r requirements.txt

(myenv) [user@host statistics]$ python3 stats.py --size --dir /eos/nica/bmn/exp/raw/run7/ --config config-size.json --recursive
```
