# BM@N Dashboard

Данный скрипт нужно запускать, используя python 3.6+, с помощью команды `python stats.py` с различными аргументами:

```
usage: stats.py [-h] [--dir [dir]] [--size] [--time]
                [--config [CONFIG]] [--output [OUTPUT]]

Script for size and time statistics. For more info see
https://github.com/loooj58/BMAN

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

Другие параметры запуска задаются в конфиг-файле в формате json:

обязательные:
* `extensions_size` - расширения файлов с данными
* `extensions_time` - расширения файлов с логами
* `db_user`, `db_pass`, `db_name`, `db_host` - креденшиалы базы данных

опциональные:
* `dpi` - dpi для сохранения/показа картинки
* `folders_ignore` - папки, которые не нужно рассматирвать при обработке


Установка необходимых библиотек происходит с помощью `pip install requirements.txt` или другим любым известным Вам способом.

Сейчас сохраняется картинка с отмеченным линией средним и тайтлом, где написаны единицы измерения и среднее. Список некорректно завершившихся задач записывается выводится в консоль.

Примеры выполнения я специально добавила в репозиторий, чтобы можно было посмотреть результат, не скачивая и не запуская.

Если будут предложения, открывайте issue тут или пишите на почту (loooj58@gmail.com), в youtrack (https://npm.mipt.ru/youtrack/issue/NICA-27), в телеграм (ta_nyan).
