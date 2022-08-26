#!/usr/bin/env python3

import boto3
from botocore import UNSIGNED
from botocore.config import Config
import sqlite3

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    print("Creating configuration file with sqlite")

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
