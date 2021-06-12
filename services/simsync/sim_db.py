# -*- coding: utf-8 -*-

import os
import sys
import json
import psycopg2
import re
import subprocess
import logging
import hashlib

simulation_directory = "/eos/nica/bmn/sim/gen"
VMCWORKDIR = "$HOME/bmnroot"

name_to_generator = {
    "urqmd": "UrQMD",
    "dqgsm": "DCMQGSM",
    "dcmqgsm": "DCMQGSM",
    "dcm-qgsm": "DCMQGSM",
    "dcmsmm": "DCMSMM",
    "dcm-smm": "DCMSMM"
}

name_to_particle = {
    "d" : "d",
    "pb": "Pb",
    "cu": "Cu",
    "sn": "Sn",
    "al": "Al",
    "ar": "Ar",
    "kr": "Kr",
    "au": "Au",
    "c" : "C",
    "p" : "p",
    "ag": "Ag",
    "xe": "Xe"
}

exclude_extensions = {
    ".out"
}

log_level= logging.INFO #logging.DEBUG

# Print iterations progress
def printProgress(iteration, total, prefix = 'Progress:', suffix = 'Complete', percent_view = 1):
    """
    Call in a loop to create terminal progress bar
    @params:
        iteration   - Required  : current iteration (Int)
        total       - Required  : total iterations (Int)
    """
    decimals = 1 # positive number of decimals in percent complete
    length = 25 # character length of bar
    fill = '█' # bar fill character
    iteration += 1
    total += 1
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    if percent_view == 1:
        percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
        sys.stdout.write('\r%s |%s| %s%% %s' % (prefix, bar, percent, suffix))
    else:
        progress_iter = ("{0} of {1}").format(iteration, total)
        sys.stdout.write('\r%s |%s| %s %s' % (prefix, bar, progress_iter, suffix))
    sys.stdout.flush()

# Recursive processing the simulation directory
def recurse_path(dir_path, generator_type_dir, conn, existing_files, exist_validity):
    dir_list = os.listdir(dir_path)
    logging.debug(dir_list)

    first_file = 1
    total_files = len([name for name in dir_list if os.path.isfile(os.path.join(dir_path, name))]) - 1
    base_dir_path = os.path.basename(dir_path)
    for idx, file_name in enumerate(dir_list):
        filepath = os.path.join(dir_path, file_name)
        
        # whether it is a normal file
        if os.path.isfile(filepath):
            # Check if the file has a correct extension
            isSkip = False
            for excl_extension in exclude_extensions:
                if filepath.endswith(excl_extension):
                    logging.debug('File was skipped because of the extension : {0}'.format(filepath))
                    isSkip = True
                    break
            if isSkip: continue

            if first_file == 1:
                first_file = 0
                sys.stdout.write('\n') # Print New Line
            printProgress(idx, total_files, base_dir_path, "files", 0)

            # find file in the list obtained from the Database: if exists, then skip
            if filepath in existing_files:
                exist_validity[existing_files.index(filepath)] = 1
                continue

            # get file size
            file_size = os.path.getsize(filepath)
            logging.debug('file size: {0} MB'.format(file_size/1024.0/1024.0))
            if file_size <= 0:
                logging.error("File size is wrong: {0}".format(filepath))
                file_size = None
                continue

            logging.debug('{0}'.format(filepath))
            logging.debug('generator_type_dir: {0}'.format(generator_type_dir))
            # remove extension
            file_name = os.path.splitext(file_name)[0]
            # split file name by '_' symbol
            file_tokens = file_name.split("_")

            is_exit = False
            token_num = 0
            # parse generator name if exists
            generator_type_file = ""
            for gen_name in name_to_generator:
                if gen_name in file_tokens[token_num].lower():
		    generator_type_file = name_to_generator[gen_name]
                    if generator_type_dir and generator_type_dir != generator_type_file:
                        logging.error('generator types do not match for the directory and file name: {0}'.format(filepath))
			is_exit = True
                    else: logging.debug('generator type in file: {0}'.format(generator_type_file))
                    token_num += 1
                    if token_num == len(file_tokens):
                        logging.error('unexpected end of the file name after generator definition: {0}'.format(filepath))
                        is_exit = True
                        break
                    break
            if is_exit: continue
            if not generator_type_file: generator_type_file = generator_type_dir
            if not generator_type_file:
                logging.error('generator type was not set for file: {0}'.format(filepath))

            # parse beam and target
            beam_target = ""
            while not beam_target:
                beam_target = re.search(r"(?P<beam>(d|ar|kr|au|c|p|ag|xe).*?)(?P<target>(cu|al|pb|sn|au|c|p|ag).*?)", file_tokens[token_num].lower())
                token_num += 1
                if token_num == len(file_tokens):
                    break
            if not beam_target:
                logging.error("Beam and Target were not found in the file name: {0}".format(filepath))
                continue
            else:
                logging.debug('{0}-{1}'.format(beam_target.group('beam'),beam_target.group('target')))
            if token_num == len(file_tokens):
                logging.error('unexpected end of the file name after beam-target definition: {0}'.format(filepath))
                continue
            beam = name_to_particle[beam_target.group('beam')]
            target = name_to_particle[beam_target.group('target')]

            energy_gr = re.search("\d+\.?\d*", file_tokens[token_num])
            if not energy_gr:
                logging.error("Energy was not found in the file name: {0} with token: {1}".format(filepath, file_tokens[token_num]))
                continue
            else:
                energy = energy_gr.group()
                logging.debug('energy: {0}'.format(energy))
                token_num += 1
                if token_num == len(file_tokens):
                    logging.error('unexpected end of the file name after energy definition: {0}'.format(filepath))
                    continue

            centrality = file_tokens[token_num]
            if not centrality:
                logging.error("Centrality was not found in the file name: {0}".format(filepath))
                continue
            else:
                logging.debug('centrality: {0}'.format(centrality))
                token_num += 1

            # get event count via BmnRoot executable file
            popen = subprocess.Popen(". {0}/build/config.sh > /dev/null; show_event_count {1} \"{2}\"".format(VMCWORKDIR, generator_type_file, filepath), stdout=subprocess.PIPE, shell=True)
            popen.wait()
            event_count = popen.stdout.read()
            logging.debug(event_count)
            if not event_count.isdigit():
                logging.error("Event count was not defined: {0}".format(filepath))
                event_count = None
                continue
            else:
                i_event_count = int(event_count)
                if i_event_count < 1:
                    logging.error("Event count is zero or less: {0}".format(filepath))
                    event_count = None
                    continue 
                else: logging.debug('event count: {0}'.format(event_count))
            
            # get file MD5 checksum
            a_file = open(filepath, "rb")
            file_content = a_file.read()
            file_md5 = hashlib.md5(file_content).hexdigest()
            
            # replace 'eos' by 'eos/eos.jinr.ru' to form file_path_micc
            #filepath_micc = filepath
            #filepath_micc.replace("eos", "eos/eos.jinr.ru", 1)
            

            logging.info("\nINSERT INTO simulation_file(file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_size, file_md5) \
            \nVALUES ({0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8})".format(filepath, generator_type_file, beam, target, energy, centrality, event_count, file_size, file_md5))
            # insert new file into the Database
            cursor = conn.cursor()
            cursor.execute("INSERT INTO simulation_file(file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_size, file_md5) \
            VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)", (filepath, generator_type_file, beam, target, energy, centrality, event_count, file_size, file_md5))
            conn.commit()
            cursor.close()
        
        # whether it is a directory        
        elif os.path.isdir(filepath):
            for gen_name in name_to_generator:
                if gen_name in file_name.lower():
                    generator_type_dir = name_to_generator[gen_name]
                    break
            recurse_path(filepath, generator_type_dir, conn, existing_files, exist_validity)

    return 0


# MAIN FUNCTION
simulation_directory = os.path.abspath(simulation_directory)

# create log file
logging.basicConfig(format='%(asctime)s %(message)s', datefmt='%Y-%m-%d %H:%M:%S', filename='sim_db.log', filemode='w', level=log_level)

# load JSON configuration
config = json.load(open("sim_db.json"))

# connect to the Unified Database
try:
  conn = psycopg2.connect(("dbname=%s user=%s host=%s password=%s") % (config["db_name"], config["db_user"], config["db_host"], config["db_pass"]))
except Exception as e:
  logging.error("Connection Error: invalid connection parameters")
  logging.error(e)

# select all simulation files from the Database
cursor = conn.cursor()
cursor.execute("SELECT file_path from simulation_file")
conn.commit()
existing_files_full = cursor.fetchall()
cursor.close()
existing_files = [x[0] for x in existing_files_full]

exist_validity = [0] * len(existing_files)

#for row in existing_files:
#print(existing_files[0])

# process all files in simulation_directory
recurse_path(simulation_directory, "", conn, existing_files, exist_validity)
print()

# delete files from the Database which have not been found
val = len(exist_validity)
for idx, val in enumerate(exist_validity):
    if val != 1:
        logging.error("\nDELETE FROM simulation_file WHERE file_path = {0}".format(existing_files[idx]))
        cursor = conn.cursor()
        cursor.execute("DELETE FROM simulation_file WHERE file_path = %s", (existing_files[idx], ))
        conn.commit()
        cursor.close()

# close connection to the Database
conn.close()
