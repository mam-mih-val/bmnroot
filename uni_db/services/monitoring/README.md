## Database Health Check

To start in foreground:  \
`./nica-db-mon.py [--config <configfile>]`
  
To start in background:  \
`nohup ./nica-db-mon.py [--config <configfile>] &`

The default config file name is `nica-db-mon.json`.


### To run as a service (CentOS 7)

```
[root@centos7 ~]# cd /etc/systemd/system
[root@centos7 system]# cat nica-db-mon.service 
[Unit]
Description = Run NICA db test script

[Service]
ExecStart = /usr/bin/python3 /home/lab/nica-health-check/nica-db-mon.py --config /home/lab/nica-health-check/nica-db-mon.json
Restart=always 
RestartSec=30

[Install]
WantedBy = multi-user.target

[root@centos7 system]# systemctl enable nica-db-mon.service
[root@centos7 system]# systemctl start nica-db-mon.service
```

### To build installable distribution

Build wheel and tgz:  \
```python3 setup.py sdist bdist_wheel```

Install using pip:  \
`sudo pip install -U dist/nica_db_monitor_pklimai-0.0.1-py3-none-any.whl`

