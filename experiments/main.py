#!/usr/bin/env python3

import boto3
from botocore import UNSIGNED
from botocore.config import Config
import sqlite3

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print("Creating configuration file with sqlite")

    con = sqlite3.connect("wx_configuration.sql3")

    cur = con.cursor()

    cur.execute("""
        CREATE TABLE config (
            key TEXT PRIMARY KEY,
            value TEXT
        );
    """)

    cur.execute("""
        INSERT INTO config VALUES ("version", "1");
    """)

    res = cur.execute("SELECT * FROM config")
    print(res.fetchall())

# ----------------------------------------------------------------------------------------------

# List of stations and locations
# https://www.ncei.noaa.gov/access/homr/file/nexrad-stations.txt
# NCDCID   ICAO WBAN  NAME                           COUNTRY              ST COUNTY                         LAT       LON        ELEV   UTC   STNTYPE

# ----------------------------------------------------------------------------------------------


# 30 second rate limit on this api
# https://www.weather.gov/documentation/services-web-api
# User-Agent: (myweatherapp.com, contact@myweatherapp.com)
#    GeoJSON: application/geo+json
#    JSON-LD: application/ld+json
#    DWML: application/vnd.noaa.dwml+xml
#    OXML: application/vnd.noaa.obs+xml
#    CAP: application/cap+xml
#    ATOM: application/atom+xml
# Accept: application/cap+xml
# Feature-Flag: forecast_temperature_qv

# Get all alerts
# https://api.weather.gov/alerts/active


# Alert locations
# https://www.weather.gov/gis/AWIPSShapefiles

# ----------------------------------------------------------------------------------------------

#    s3 = boto3.client('s3', region_name='us-east-1', config=Config(signature_version=UNSIGNED))

#    response = s3.list_objects_v2(
#        Bucket='noaa-nexrad-level2',
#        Delimiter='/',
#        Prefix='2022/01/01/KGLD/'
#    )

#    print(response)


# https://graphical.weather.gov/xml/
# /Year/Month/Day/NEXRAD Station/filename
# aws s3 ls --recursive s3://noaa-nexrad-level2/2010/01/01/`

# https://gis.ncdc.noaa.gov/kml/nexrad.kmz

CREATE TABLE radar_stations (
    pk INTEGER PRIMARY KEY,
    ncdcid INTEGER NOT NULL, -- Unique identifier used by NCEI
    icaq TEXT NOT NULL,      -- International Civil Aeronautics Organization ID
    wban INTEGER NOT NULL,   -- Used at NCEI for digital data storage and general station identification purposes
    name TEXT NOT NULL,      -- Name of station, uppercase, may contain characters, numbers or symbols
    country TEXT NOT NULL,   -- Name of country, uppercase
);

ST       2        XX            073-074         USPS two character alphabetic abbreviation for each state, uppercase.

COUNTY   30       X(30)         076-105         Name of county, uppercase.

LAT      9        x99.99999     107-115         Latitude in decimal degrees, where "x" is blank or "-".

LON      10       x999.99999    117-126         Longitude in decimal degrees, where "x" is blank or "-".

ELEV     6        999999        128-133         Station elevation in feet, -99999 if missing.  Elevation is combined elevation of
                                                ground elevation + tower height + feed horn.

UTC      3        x99           135-139         Time zone, positive or negative offset from UTC, where "x" is "+" or "-".

STNTYPE  50       X(50)         141-190         Type of observing programs associated with the station.
