
"""GeoGateway Disloc service
    -- execute workflows
        -- moment-tensor solution
        -- auto-disloc
        -- single input file
        -- multiple input files
"""

import sys
import argparse
from earthquakefeed import check_event_geojson


def moment_tensor_workflow(geojsonurl):
    """execute moment tensor workflow"""

    # parse event json to get dict object
    event = check_event_geojson(geojsonurl)

    if not "moment-tensor" in event:
        print("no moment tensor is presented in ", geojsonurl)
        sys.exit()

    return


def main():
    """parameters parsing"""

    parser = argparse.ArgumentParser(prog="disloc_service.py")
    # main workflows
    parser.add_argument("workflow", choices=["moment-tensor", "auto-disloc", "input-file", "multiple-files"], help="choose an workflow")

    # optional paramters
    # monment-tensor
    # url to the event GeoJSON
    parser.add_argument("--geojson", help="URL to event GeoJSON")

    args = parser.parse_args()
    if args.workflow == "moment-tensor":
        if not args.geojson:
            print("!notice: for moment-tensor workflow, an GeoJSON URL is required.")
        else:
            moment_tensor_workflow(args.geojson)

if __name__ == "__main__":
    main()
