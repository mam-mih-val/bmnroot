## Database Service Monitoring

### Requirements

- Linux OS (tested on Ubuntu 16.04 and CentOS 7)
- Python 3.5 or later
- Install PyPI packages `influxdb` (5.2.3 or later) and `psycopg2-binary` (2.8.4 or later), e.g. \
`pip3 install "influxdb>=5.2.3"` \
`pip3 install "psycopg2-binary>=2.8.4"`

### Starting 

To start in foreground:  \
`./mon-service.py [--config <configfile>]`
  
To start in background:  \
`nohup ./mon-service.py [--config <configfile>] &`

The default config file name is `mon-service.json`.


### To run as a service (CentOS 7)

```
[root@centos7 ~]# cd /etc/systemd/system
[root@centos7 system]# cat mon-service.service 
[Unit]
Description = Run monitor service script

[Service]
ExecStart = /usr/bin/python3 /home/lab/mon-service/mon-service.py --config /home/lab/mon-service/mon-service.json
Restart=always 
RestartSec=30

[Install]
WantedBy = multi-user.target

[root@centos7 system]# systemctl enable mon-service.service
[root@centos7 system]# systemctl start mon-service.service
```

### To build installable distribution

Build wheel and tgz:  \
```python3 setup.py sdist bdist_wheel```

Install using pip:  \
`sudo pip install -U dist/mon_service-0.0.1-py3-none-any.whl`

