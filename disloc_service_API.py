"""
    disloc_service_API.py
        -- a minimum flask api wrapper

        $export FLASK_APP=disloc_service_API.py
        $export FLASK_DEBUG=1
        $python3 -m flask run

        http://localhost:5000/gpsservice/test

"""

import sys
import socket
import json
from flask import Flask
from flask import request
from flask import Response

app = Flask(__name__)


"""test service is running"""
@app.route("/dislocservice/test")
def test():
    info={}
    info["python"] = sys.version
    info["runningmode"] = app.debug
    return info["python"] + str(info["runningmode"])

if __name__ == "__main__":
    pass
    # old method
    #app.run(host='0.0.0.0', debug=False)
