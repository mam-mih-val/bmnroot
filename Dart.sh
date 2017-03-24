#!/bin/bash


function print_example(){
echo "##################################################################"
echo "# To set the required parameters as source and the build         #"
echo "# directory for ctest, the linux flavour and the SIMPATH         #"
echo "# put the export commands below to a separate file which is read #"
echo "# during execution and which is defined on the command line.     #"
echo "# Set all parameters according to your needs.                    #"
echo "# LINUX_FLAVOUR should be set to the distribution you are using  #"
echo "# eg Debian, SuSe etc.                                           #"
echo "# For example                                                    #"
echo "#!/bin/bash                                                      #"
echo "#export LINUX_FLAVOUR=Etch32                                     #"
echo "#export FAIRSOFT_VERSION=mar08                                   #"
echo "#export SIMPATH=<path_to_installation_of_external_packages>      #"
echo "#export BUILDDIR=/tmp/fairroot/build_cbm_\${FAIRSOFT_VERSION}     #"
echo "#export SOURCEDIR=/misc/uhlig/SVN/ctest/cbmroot                  #"
echo "##################################################################"
}

#if test  "x$SIMPATH" = "x" ; then
#  echo ""                                                             
#  echo "-- Error -- You don't set the needed variables in this shell script."
#  echo "-- Error -- Please edit the script and do so."
#  echo ""
#  exit 1
#fi

if [ "$#" -lt "1" ]; then
  echo ""
  echo "-- Error -- Please start script with one,two,three, four or five parameters:"
  echo "-- Error -- 1st parameter - Nightly or Experimental "
  echo "-- Error -- 2nd parameter - path to your *.cmake files (trunk)"
  echo "-- Error -- 3rd parameter - CDash_Env_BmnRoot.sh     (default)"
  echo "-- Error -- 4th parameter - path to your config.sh     (build)"
  echo "-- Error -- 5th parameter - fairsoft: jul09(default), jan10, or jun11"
  echo ""
  print_example
  exit 1
fi

# test if a ctest model is either Experimantal or Nightly
if [ "$1" == "Experimental" -o "$1" == "Nightly" -o "$1" == "Continuous" ]; then
  echo ""
else
  echo "-- Error -- This ctest model is not supported."
  echo "-- Error -- Possible arguments are Nightly, Experimental or Continuous."
  exit 1
fi 

# test if the input file exists and execute it
# if [ "$#" -gt "2" ]; then
if [ -e "$2" ]; then
  cd $2
  if [ "$#" -gt "4" ]; then
      sleep 2
      source $3 $2 $4 $5
  else
      if [ -e "$4" ]; then
          sleep 2
	  source $3 $2 $4
      else
	  if [ -e "$3" ]; then
              sleep 2
	      source $3 $2
	  else
              sleep 2
	      source CDash_Env_BmnRoot.sh  $2 
	  fi
      fi
  fi
else
  if [ "$#" -gt "1" ]; then
    cd $2
    sleep 2
    source CDash_Env_BmnRoot.sh $2
  else
    sleep 2
    source CDash_Env_BmnRoot.sh $(pwd) 
  fi
fi

# set the ctest model to command line parameter
export ctest_model=$1
# test for architecture
arch=$(uname -s | tr '[A-Z]' '[a-z]')
chip=$(uname -m | tr '[A-Z]' '[a-z]')

# extract information about the system and the machine and set
# environment variables used by ctest
SYSTEM=$arch-$chip
if test -z $CXX ; then
  COMPILER=gcc;
  GCC_VERSION=$(gcc -dumpversion)
else
  COMPILER=$CXX;
  GCC_VERSION=$($CXX -dumpversion)
fi

export LABEL1=${LINUX_FLAVOUR}-$SYSTEM-$COMPILER$GCC_VERSION-fairsoft_$FAIRSOFT_VERSION
export LABEL=$(echo $LABEL1 | sed -e 's#/#_#g')

# get the number of processors
# and information about the host
if [ "$arch" = "linux" ];
then
  if [ "$NCPU" != "" ];
  then
    export number_of_processors=$NCPU
  else 
    export number_of_processors=$(cat /proc/cpuinfo | grep processor | wc -l)
  fi
  export SITE=$(hostname -f)
elif [ "$arch" = "darwin" ];
then
  if [ "$NCPU" != "" ];
  then
    export number_of_processors=$NCPU
  else 
    export number_of_processors=$(sysctl -n hw.ncpu)
  fi
  export SITE=$(hostname -s)
fi

echo "************************"
date
echo "LABEL: " $LABEL
echo "SITE: " $SITE
echo "Model: " ${ctest_model}
echo "Nr. of processes: " $number_of_processors
echo "************************"

cd $SOURCEDIR
# cd $BUILDDIR

ctest -S $SOURCEDIR/BmnRoot_test.cmake -V --VV


echo "******************* end of Dart.sh  *****"
