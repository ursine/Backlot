#!/usr/bin/env python3

import boto3
from botocore import UNSIGNED
from botocore.config import Config


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    s3 = boto3.client('s3', region_name='us-east-1', config=Config(signature_version=UNSIGNED))

    response = s3.list_objects_v2(
        Bucket='noaa-nexrad-level2',
        Delimiter='/',
        Prefix='2022/01/01/KGLD/'
    )

    print(response)


# https://graphical.weather.gov/xml/
# /Year/Month/Day/NEXRAD Station/filename
# aws s3 ls --recursive s3://noaa-nexrad-level2/2010/01/01/`

# https://gis.ncdc.noaa.gov/kml/nexrad.kmz
