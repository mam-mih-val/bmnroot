import os, re
import sys, getopt
import glob
import mmap

# GLOBAL CONSTANTS
success_marker = "FINISH (event"

# MAIN CODE
fullCmdArguments = sys.argv
argumentList = fullCmdArguments[1:]
try:
  opts, args = getopt.getopt(argumentList,"h:i",["help","input"])
except getopt.GetoptError:
  print 'USAGE: check_success.py -i \"[<input_direcory>,<regular_expression_files>]\"\n'
  sys.exit(1)

if len(args) != 1:
  print 'USAGE: check_success.py -i \"[<input_direcory>,<regular_expression_files>]\"\n'
  sys.exit(2)

#pattern = re.compile(success_marker)
file_list = args[0]
if file_list.endswith("/"):
  file_list += "*"
else:
  if os.path.isdir(file_list):
    file_list += "/*"
#print('file_list = ' + file_list)

file_count = 0
for file in glob.glob(file_list):
  if not os.path.isfile(file): continue
  #print('Current file: '+ file)
  file_count += 1
  with open(file) as fp:
    s = mmap.mmap(fp.fileno(), 0, access=mmap.ACCESS_READ)
    if s.find(success_marker) == -1:
        print(file + ': marker not found')
if file_count == 0:
  print('There are no files in "'+ file_list + '" or directory does not exist')
