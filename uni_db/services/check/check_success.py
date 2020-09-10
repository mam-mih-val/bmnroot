import os, re
import sys, getopt
import glob
import mmap

# GLOBAL CONSTANTS
success_marker = "FINISH (event"

# MAIN CODE
try:
  options, remainder = getopt.getopt(sys.argv[1:], "hi:r",["help", "input", "remove"])
except getopt.GetoptError:
  print 'USAGE: python check_success.py -i \"[<input_direcory>,<regular_expression_files>]\" [--remove]\n'
  sys.exit(1)

file_list = ""
remove_option = False
for opt, arg in options:
    if opt in ('-h', '--help'):
        print 'USAGE: python check_success.py -i \"[<input_direcory>,<regular_expression_files>]\" [--remove]\n'
        sys.exit(0)
    elif opt in ('-i', '--input'):
        file_list = arg
    elif opt in ('-r', '--remove'):
        remove_option = True

if file_list == "":
  print 'USAGE: python check_success.py -i \"[<input_direcory>,<regular_expression_files>]\" [--remove]\n'
  sys.exit(2)

if file_list.endswith("/"):
  file_list += "*"
else:
  if os.path.isdir(file_list):
    file_list += "/*"
#print('file_list = ' + file_list)

file_count = 0
# cycle to check all files
for file in glob.glob(file_list):
  if not os.path.isfile(file): continue
  #print('Current file: '+ file)
  file_count += 1
  delete_flag = False
  with open(file) as fp:
    s = mmap.mmap(fp.fileno(), 0, access=mmap.ACCESS_READ)
    if s.find(success_marker) == -1:
        print(file + ': marker not found')
    elif remove_option == True:
        delete_flag = True
  if delete_flag == True:
    os.remove(file)

if file_count == 0:
  print('There are no files in "'+ file_list + '" or directory does not exist')
