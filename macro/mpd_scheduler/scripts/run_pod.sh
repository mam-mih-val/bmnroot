gedit ~/PoD/3.12/pod_ssh.cfg

. ~/mpdroot/build/config.sh
. /opt/fairsoft/env.fairsoft
. ~/PoD/3.12/PoD_env.sh
pod-server start
pod-ssh -c ~/PoD/3.12/pod_ssh.cfg submit
pod-ssh -c ~/PoD/3.12/pod_ssh.cfg status
pod-info -l
pod-info -n

