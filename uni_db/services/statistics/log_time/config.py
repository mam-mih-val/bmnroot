SIZE = [1, 60, 60, 24, 30, 12]
UNITS = ['seconds', 'minutes', 'hours', 'days', 'months', 'years']
START = 'Start date:'
END = 'End date:'
SUCCESS = 'Macro finished successfully'
START_REGEX = r'Start date: \w{2,3} \w{3}\s{1,2}\d{1,2} \d{1,2}:\d{1,2}:\d{1,2} \w+ \d{4}'
END_REGEX = r'End date: \w{2,3} \w{3}\s{1,2}\d{1,2} \d{1,2}:\d{1,2}:\d{1,2} \w+ \d{4}'
PROCESSING_LINE_START = "Processing "
PROCESSING_REGEX = r'Processing .*\.[cC]\(\"([^\"]*)\"\,.*\)\.\.\.'
MONTH_ARR = {
    'Jan': 1, 'Feb': 2, 'Mar': 3, 'Apr': 4, 'May': 5, 'Jun': 6, 'Jul': 7, 'Aug': 8, 'Sep': 9, 'Oct': 10, 'Nov': 11, 'Dec': 12,
    'янв': 1, 'фев': 2, 'мар': 3, 'апр': 4, 'май': 5, 'июн': 6, 'июл': 7, 'авг': 8, 'сен': 9, 'окт': 10, 'ноя': 11, 'дек': 12
}
RUN_REGEX = r'bmn_run\d+'
RUN_EXTENSION = '.root'
DPI = 300
FOLDERS_IGNORE = ['__pycache__', 'myenv', '.git']
BINS = 40
BINS_PER_EVENT = 40
UNSUCCESSFUL_LOG_FILE = "unsuccessful_list.txt"
