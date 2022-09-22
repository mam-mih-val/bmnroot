#! /usr/bin/python3

import os
import requests
import sys
import argparse
import smtplib
import json
from timeit import default_timer as timer
from time import sleep, time
# Install: "pip install psycopg2-binary"
import psycopg2
# Install: "pip install influxdb"
from influxdb import InfluxDBClient
from requests.exceptions import ConnectionError

# InfluxDB measurement name
MEASUREMENT_NAME = "resp_time"


def test_ping(ip):
    # Number of pings is hardcoded, consider turning it into parameter if needed
    resp = os.system("ping -c 5 {} > /dev/null".format(ip))
    return {"success": resp == 0}


def test_pgsql_connection(params):
    t0 = timer()
    connection = None
    cursor = None

    try:
        # Create connection
        # Note: timeout is hardcoded, consider turning it into parameter if needed
        connection = psycopg2.connect(host=params["SERVER"], port=params["PORT"],
                                      user=params["USER"], password=params["PASS"],
                                      database=params["DBNAME"], connect_timeout=5)

        cursor = connection.cursor()
        # Print PostgreSQL Connection properties
        # print ( connection.get_dsn_parameters(),"\n")

        # Print PostgreSQL version
        cursor.execute("SELECT version();")
        record = cursor.fetchone()
        # print("You are connected to - ", record,"\n")

    except (Exception, psycopg2.Error) as error:
        # print ("Error while connecting to PostgreSQL", error)
        return {"success": False}

    finally:
        # closing database connection.
        if cursor:
            cursor.close()
        if connection:
            connection.close()

    time_elapsed = timer() - t0
    # print("PUTVAL pgsql_resp_time {}".format(time_elapsed))
    return {"success": True, "latency": time_elapsed}

def test_http(http_addr):
    try:
        result_request = requests.get(http_addr, timeout=5)  # Getting status code from the web site
        # if status code is less than 200 or more than 299 then show error message
        if result_request.status_code < 200 and result_request.status_code > 299:
            return {"success": False}
    except requests.exceptions.RequestException as err:    # ConnectionError, HTTPError, Timeout, TooManyRedirects
        return {"success": False}
    return {"success": True}

def send_email(to, short_msg, long_msg=""):
    try:
        subj = email_subj_prefix + ": " + short_msg
        msg = "From: %s\nTo: %s\nSubject: %s\n\n%s\n" % (config["MAIL"]["USER"], to, subj, long_msg)

        mailserver = smtplib.SMTP(config["MAIL"]["SERVER"], config["MAIL"]["PORT"])
        mailserver.starttls()
        mailserver.login(config["MAIL"]["USER"], config["MAIL"]["PASS"])
        # For multiple recipients, 'to' has to be a list here
        problems = mailserver.sendmail(config["MAIL"]["USER"], to.split(","), msg)
        mailserver.quit()
    except Exception as e:
        print("Error sending email - ignoring. Details: " + str(e))


def get_notify(params_dict):
    global config
    if "NOTIFY" in params_dict:
        return params_dict["NOTIFY"]
    else:
        return config["NOTIFY"]


def main():
    # Config loaded from JSON file
    global config
    # Line to use in email Subject
    global email_subj_prefix

    argparser = argparse.ArgumentParser()
    argparser.add_argument("--config", help="Configuration file")
    args = argparser.parse_args()
    if args.config is None:
        print("You must provide configuration file using --config")
        sys.exit(1)
    print("Reading --config file: " + args.config)

    try:
        with open(args.config) as conf_file:
            config = json.loads(conf_file.read())
    except Exception as e:
        print("Error reading config file: " + str(e))
        sys.exit(1)

    # Use either configured service name or JSON file name (without extension) for emails
    email_subj_prefix = config["NAME"] if "NAME" in config else os.path.splitext(os.path.basename(args.config))[0]

    print("Starting " + email_subj_prefix)
    send_email(config["LOG"], "Monitoring script started")

    # Variables storing previous success/fail values
    ping_prev_success = {}
    pgsql_prev_success = {}
    http_prev_success = {}
    ping_failed = 0
    pgsql_failed = 0
    http_failed = 0
    influxdb_prev_success = True
    influxdb_initialized = False

    for server in config["PING"]:
        ping_prev_success[server] = True
    for server, server_params in config["DATABASE"].items():
        if server_params["DBMS"] == "PGSQL":
            pgsql_prev_success[server] = True
        else:
            print("Unsupported DBMS type: " + server_params["DBMS"])
    for server in config["WEB"]:
        http_prev_success[server] = True

    try:
        if "OUTPUT" in config and config["OUTPUT"]["DBMS"].upper() == "INFLUXDB":
            if "USER" in config["OUTPUT"] and "PASS" in config["OUTPUT"]:
                iclient = InfluxDBClient(host=config["OUTPUT"]["SERVER"], port=config["OUTPUT"]["PORT"],
                                         username=config["OUTPUT"]["USER"], password=config["OUTPUT"]["PASS"])
            else:
                iclient = InfluxDBClient(host=config["OUTPUT"]["SERVER"], port=config["OUTPUT"]["PORT"])
        else:
            print("No configuration for InfluxDB output is found.")

        while True:
            # Do all PING tests
            for test_name, test_params in config["PING"].items():
                result_ping = test_ping(test_params["HOST"])
                print("PING " + test_name + ": " + str(result_ping))
                success = result_ping["success"]
                if ping_prev_success[test_name] != success or ping_failed > 0:
                    if success:
                        send_email(get_notify(test_params), test_name + " - PING state changed to UP")
                        ping_failed = 0
                    else:
                        if ping_failed == 1
                            send_email(get_notify(test_params), test_name + " - PING state changed to *** DOWN ***")
                        else
                            ping_failed += 1
                    ping_prev_success[test_name] = success

            # Do all PGSQL tests
            for test_name, test_params in config["DATABASE"].items():
                if test_params["DBMS"] != "PGSQL":
                    print("Unsupported DBMS type " + test_params["DBMS"])
                    continue
                result_pgsql = test_pgsql_connection(test_params)
                print("PGSQL " + test_name + ": " + str(result_pgsql))
                success = result_pgsql["success"]
                if pgsql_prev_success[test_name] != success or pgsql_failed > 0:
                    if success:
                        send_email(get_notify(test_params), test_name + " - PGSQL state changed to UP")
                        pgsql_failed = 0
                    else:
                        if pgsql_failed == 1
                            send_email(get_notify(test_params), test_name + " - PGSQL state changed to *** DOWN ***")
                        else
                            pgsql_failed += 1
                    pgsql_prev_success[test_name] = success

                if "OUTPUT" in config and config["OUTPUT"]["DBMS"].upper() == "INFLUXDB":
                    # measurement,tag_set field_set timestamp
                    data = ["{measurement},test_name={test_name} resptime={resptime} {timestamp}"
                            .format(measurement=MEASUREMENT_NAME,
                                    test_name=test_name,
                                    resptime=result_pgsql.get("latency") or 0,
                                    timestamp=int(time() * 1000))]
                    try:
                        if not influxdb_initialized:
                            # If database exists, this does nothing
                            iclient.create_database(config["OUTPUT"]["DBNAME"])
                            influxdb_initialized = True
                        iclient.write_points(data, database=config["OUTPUT"]["DBNAME"], time_precision='ms',
                                             batch_size=10000, protocol='line')
                        if influxdb_prev_success == False:
                            influxdb_prev_success = True
                            send_email(get_notify(config["OUTPUT"]), "InfluxDB is reachable again.")
                    except (ConnectionRefusedError, ConnectionError) as err:
                        print("Error writing to InfluxDB - ignoring...")
                        if influxdb_prev_success == True:
                            influxdb_prev_success = False
                            send_email(get_notify(config["OUTPUT"]), "InfluxDB is unreachable!", str(err))
                else:
                    print("Skip writing to InfluxDB due to no configuration.")

            # Do all HTTP tests
            for test_name, test_params in config["WEB"].items():
                result_http = test_http(test_params["HTTP"])
                print("Checking website for " + test_name + " (" + test_params["HTTP"] + "): " + str(result_http))
                success = result_http["success"]
                if http_prev_success[test_name] != success or http_failed > 0:
                    if success:
                        send_email(get_notify(config["OUTPUT"]), test_name + " - Web Interface state changed to UP")
                        http_failed = 0
                    else:
                        if http_failed == 1
                            send_email(get_notify(config["OUTPUT"]), test_name + " - Web Interface state changed to *** DOWN ***")
                        else
                            http_failed += 1
                    http_prev_success[test_name] = success

            sleep(config["INTERVAL_SEC"])

    except Exception as e:
        print("Error - terminating! " + str(e))
        send_email(config["LOG"], "Monitoring script terminated!", str(e))


if __name__ == "__main__":
    main()
