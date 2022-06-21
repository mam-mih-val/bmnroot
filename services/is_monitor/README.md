## Monitoring Service for the Information Systems of the experiment

### Features

This is a service to monitor availability of the IT infrastructure of the experiment systems, such as hosts, databases, web interfaces.

Currently implemented mon-service functionality:
- PING, PGSQL, and HTTP requests probing for any number of monitored servers and Web interfaces
- Configurable via JSON file
- Email notifications (sent when a check fails twice)
- Response time stored in InfluxDB for visualization and extra notifications with tools like Grafana

An example of the JSON configuration is presented in `mon-service.EXAMPLE.json` file, where all parameters are explained below.


### Requirements

- Linux OS (tested on Ubuntu 16.04 and CentOS 7)
- Python 3.5 or later
- PyPI packages `influxdb` (5.2.3 or later) and `psycopg2-binary` (2.8.4 or later), e.g. \
`pip3 install "influxdb>=5.2.3"` \
`pip3 install "psycopg2-binary>=2.8.4"`


### JSON schema description

```
{
  "PING": {          <-- Section to specify hosts probed with ICMP ping
    "server1": {     <-- Test name used in notifications
      "HOST": "192.168.65.116",   <-- Host IP address or name resolvable with DNS
      "NOTIFY": "mail1.jinr.ru"   <-- Email or list of emails
    },
    "server2": {                  <-- Multiple test targets are supported
      "HOST": "myserver.jinr.ru"  <-- If you do not specify "NOTIFY" it takes it from the global level
    }
  },

  "DATABASE": {      <-- Section to specify hosts probed with database (PGSQL) request
    "server1": {     <-- Test name used in notifications
      "SERVER": "192.168.65.116",   <-- Host IP address or name resolvable with DNS
      "DBMS": "PGSQL",              <-- Databse type (currently, only PGSQL is supported)
      "PORT": 5432,                 <-- TCP port
      "DBNAME": "mydb1",            <-- Database name to query
      "USER": "user_reader1",       <-- Username
      "PASS": "user_pass1",         <-- Password
      "NOTIFY": "mail3.jinr.ru,mail5.jinr.ru"   <-- Email or list of emails
    },
    "server2": {     <-- Multiple test targets are supported
        ...          <-- Use same parameter names as above
    }
  },

  "WEB": {           <-- Section to specify Web interfaces (sites) probed with HTTP (Get) request
    "mywebsite": {   <-- Test name used in notifications
      "HTTP": "http://mywebsite.jinr.ru"       <-- URL address with the protocol prefix (HTTP or HTTPS)
    },
    "mywebinter": {  <-- Multiple test targets are supported
        ...          <-- Use same parameter names as above
    }
  },

  "OUTPUT": {        <-- This optional section describes where to write data about service availability
       (if this section is omitted, service works in 'standalone' mode and only email notifications are sent)
    "DBMS": "INFLUXDB",             <-- Currently, only InfluxDB is supported
    "SERVER": "192.168.65.52",      <-- InfluxDB server host
    "PORT": 8086,                   <-- InfluxDB database port
    "DBNAME": "myinfluxdbname",     <-- InfluxDB database name
    "USER": "influxdbuser",         <-- InfluxDB username (optional), needed if authentication is required
    "PASS": "influxdbpass",         <-- InfluxDB password (optional), needed if authentication is required
    "NOTIFY": "mail1.jinr.ru,mail2.jinr.ru"     <-- Where to sent notifications about InfluxDB availability
  },

  "INTERVAL_SEC": 450,              <-- Interval to wait between probe requests

  "MAIL": {                         <-- Mail server parameters
    "SERVER": "smtp.jinr.ru",
    "PORT": 587,
    "USER": "***",
    "PASS": "***"
  },

  "LOG": "mail1.jinr.ru,mail2.jinr.ru",  <-- Email adress(es) to send global notifications  (service start and termination)
  "NOTIFY": "mail-default@jinr.ru",      <-- This email(s) will be used if NOTIFY is not specified at the test/output level
  "NAME": "My Monitoring Service"           <-- Service name used in notifications
}

```

### Starting

To start in foreground:  \
`python3 mon-service.py --config <configfile>`

To start in background:  \
`nohup python3 mon-service.py --config <configfile> &`

JSON config file name must be provided. There is no default file name.


### To run as a service (CentOS 7)

```
[root@centos7 ~]# cd /etc/systemd/system
[root@centos7 system]# cat mon-service.service
[Unit]
Description = Run monitor service script

[Service]
ExecStart = /usr/bin/python3 /home/user/system_monitor/mon-service.py --config /home/user/system_monitor/mon-service.json
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
