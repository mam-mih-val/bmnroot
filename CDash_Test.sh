#!/bin/bash
#
# CDASH check script
#

HOST="$(hostname -s)"
BRANCH="$(git branch | grep \* | awk '{print $2}')"
SOURCE_DIR="$HOME/CDASH/bmnroot_$HOST"
LOG="$HOME/CDASH/LOGS/bmnroot_cdash_$HOST.log"

mkdir -p $HOME/CDASH/LOGS

date +"START TEST %y-%m-%d at %H:%M:%S" > $LOG

if [[ $1 = "Nightly" ]]; then
	if [ -e "$SOURCE_DIR" ]
	then
        	cd $SOURCE_DIR
		git reset --hard HEAD
	else
        	git clone git@git.jinr.ru:nica/bmnroot.git $SOURCE_DIR
        	cd $SOURCE_DIR
	fi

	./Dart.sh Nightly >> $LOG 2>&1
else
	rm -rf $SOURCE_DIR
	git clone -b $BRANCH git@git.jinr.ru:nica/bmnroot.git $SOURCE_DIR
	cd $SOURCE_DIR
	./Dart.sh Experimental >> $LOG 2>&1
fi

date +"FINISH TEST %y-%m-%d at %H:%M:%S" >> $LOG
