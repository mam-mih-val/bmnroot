## Monitoring Service for the Information Systems of the experiment

### Features

This is a service to monitor availability of the IT infrastructure of the experiment systems, such as hosts, databases, web interfaces.

Currently implemented mon-service functionality:
- PING and PGSQL request probing for any number of monitored servers
- Configurable via JSON file
- Email notifications
- Response time stored in InfluxDB for visualization and extra notifications with tools like Grafana

An example of the JSON configuration is presented in `mon-service.EXAMPLE.json` file, where all parameters are explained
below.


### Requirements

- Linux OS (tested on Ubuntu 16.04 and CentOS 7)
- Python 3.5 or later
- PyPI packages `influxdb` (5.2.3 or later) and `psycopg2-binary` (2.8.4 or later), e.g. \
`pip3 install "influxdb>=5.2.3"` \
`pip3 install "psycopg2-binary>=2.8.4"`


### JSON schema description

```
{
  "PING": {                <-- Section to specify hosts probed with ICMP ping
    "server1": {                <-- Test name used in notifications 
      "IP": "192.168.65.116",   <-- Host IP address or name resolvable with DNS
      "NOTIFY": "mail1.jinr.ru" <-- Email or list of emails
    },
    "router1": {                <-- Multiple test targets are supported
      "IP": "10.254.0.41"       <-- If you do not specify "NOTIFY" it takes it from global level
    }
  },
  "DATABASE": {      <-- Section to specify hosts probed with database (PGSQL) request
    "server1": {     <-- Test name used in notifications
      "SERVER": "192.168.65.116",   <-- Host IP address or name resolvable with DNS
      "DBMS": "PGSQL",              <-- Databse type (currently we support PGSQL only)
      "PORT": 5432,                 <-- TCP port
      "DBNAME": "testdb",           <-- Database name to query
      "USER": "testuser",           <-- Username
      "PASS": "pass",               <-- Password
      "NOTIFY": "mail3.jinr.ru,mail5.jinr.ru"   <-- Email or list of emails
    },
    "server-centos2": {  <-- Multiple test targets are supported
        ...              <-- Use same parameter names as above
    }
  },

  "OUTPUT": {     <-- This optional section describes where to write data about service availability
       (if this section is omitted, service works in 'standalone' mode and only email notifications are sent)
    "DBMS": "INFLUXDB",        <-- Currently, only InfluxDB is supported
    "SERVER": "192.168.65.52", <-- InfluxDB server host
    "PORT": 8086,             
    "DBNAME": "pgsqltest",     <-- InfluxDB database name
    "USER": "influx",          <-- InfluxDB username (optional), needed if authentication is required 
    "PASS": "123456",          <-- InfluxDB password (optional), needed if authentication is required
    "NOTIFY": "mail1.jinr.ru,mail2.jinr.ru" <-- Where to sent notifications about InfluxDB availability 
  },

  "INTERVAL_SEC": 60,              <-- Interval to wait between probe requests

  "MAIL": {                        <-- Mail server parameters
    "SERVER": "smtp.yandex.ru",
    "PORT": 587,
    "USER": "***",
    "PASS": "***"
  },

  "LOG": "mail1.jinr.ru,mail2.jinr.ru",  <-- Email adress(es) to send global notifications  (service start and termination)
  "NOTIFY": "mail-default@jinr.ru",      <-- This email(s) will be used if NOTIFY is not specified at the test/output level
  "NAME": "Monitoring Service"           <-- Service name used in notifications
}

```

### Starting 

To start in foreground:  \
`./mon-service.py [--config <configfile>]`

To start in background:  \
`nohup ./mon-service.py [--config <configfile>] &`

JSON config file name must be provided. There is no default file name.


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
