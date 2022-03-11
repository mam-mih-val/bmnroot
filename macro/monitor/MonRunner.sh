BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true; 

do
cd "${BMNROOT}/macro/monitor/"
/opt/fairsoft/install/bin/root.exe -b -q 'monitor.C( 8, "~/monref/", "~/monref/", "localhost", 9001)'
sleep 5
/opt/fairsoft/install/bin/root.exe -b -q 'monitor.C( 8, "~/monref/", "~/monref/", "localhost", 9000)'
sleep 5

done
