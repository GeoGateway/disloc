
"""Functions to parse USGS geojson earthquake feed.

USGS GeoJSON feed: http://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php
Weekly significant feed
    http://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/significant_week.geojson
GeoJSON Detail Format
    http://earthquake.usgs.gov/earthquakes/feed/v1.0/detail/us200041ty.geojson
"""

import urllib.request
import json


def download_as_text(feedurl):
    """download file as text"""

    with urllib.request.urlopen(feedurl) as x:
        rawtext = x.read().decode('utf8')
    x.close

    return rawtext


def check_event_geojson(eventurl):
    """check GeoJSON Detail Format
    http://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson_detail.php
    geojsonurl: url to single event geojson
    """

    rawfeed = download_as_text(eventurl)

    # check if it has monment tensor MWB solution
    eventjson = json.loads(rawfeed)

    momenttensor = {}
    momenttensor_records = ["nodal-plane-1-dip","nodal-plane-1-rake","nodal-plane-1-strike","nodal-plane-2-dip","nodal-plane-2-rake","nodal-plane-2-strike"]

    if "moment-tensor" in eventjson["properties"]["products"]:
        for entry in eventjson["properties"]["products"]["moment-tensor"]:
            if entry["properties"]["derived-magnitude-type"].lower() == "mwb":
                for record in momenttensor_records:
                    momenttensor[record] = entry["properties"][record]
    else:
        print("no moment tensor solution")

    # dict to record earthquake event
    event_dict = {}
    event_record = ["title","place","mag","time","code","ids","url","updated"]
    for record in event_record:
        event_dict[record] = eventjson["properties"][record]
    # get coordinates: longitude, latitude, depth (in km)
    event_dict['coordinates'] = eventjson['geometry']['coordinates']

    # add moment-tensor solution
    if momenttensor == {}:
        event_dict["moment-tensor"] = False
    else:
        event_dict["moment-tensor"] = momenttensor

    return event_dict


def check_summary_geojson(summaryurl):
    """check feed in GeoJSON Summary Format
    http://earthquake.usgs.gov/earthquakes/feed/v1.0/geojson.php
    summaryurl: url to geojson feed
    """

    rawfeed = download_as_text(summaryurl)

    entries = json.loads(rawfeed)

    m5list = []
    rsslist = []

    for entry in entries['features']:

        # get mag first
        mag = entry['properties']['mag']
        # only process with mag >= 5.0
        if mag >= 5.0:

            # get coordinates: longitude, latitude, depth (in km)
            coordinates = entry['geometry']['coordinates']
            geomtype = entry['geometry']['type']
            # get event id through ids
            # http://earthquake.usgs.gov/earthquakes/feed/v1.0/glossary.php#id
            # ",ci15296281,us2013mqbd,at00mji9pf,"
            ids = entry['properties']['ids']
            event_id = ids.split(',')[1]
            # get place
            place = entry['properties']['place']
            # get time
            # Time when the event occurred. Times are reported in milliseconds since the epoch
            eventtime = entry['properties']['time']
            # get updated time
            updated = entry['properties']['updated']
            # get url
            url = entry['properties']['url']
            # get detail
            detail = entry['properties']['detail']

            print("===============================")
            print('id: ', event_id)
            print('mag: ', mag)
            print('time: ', eventtime)
            print('coordinates: ', coordinates)
            print('geomtype: ', geomtype)
            print('place: ', place)
            print('updated: ', updated)
            print('url: ', url)
            print('detail: ', detail)
            print("===============================")


def debug():
    """dry run testing"""

    feedurl = "http://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/significant_week.geojson"
    check_summary_geojson(feedurl)
    eventurl = "http://earthquake.usgs.gov/earthquakes/feed/v1.0/detail/us200041ty.geojson"
    print(check_event_geojson(eventurl))

if __name__ == "__main__":
    debug()
