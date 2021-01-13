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
    info_str = info["python"] + " debug mode: "+str(info["runningmode"]) + "<p>"
    info_str += disloc.__doc__.replace('\n','<br>')
    return info_str

"""disloc service"""
@app.route("/dislocservice/disloc")
def disloc():
    """ disloc service 
          /dislocservice/disloc?
        
        Parameters: 
        --------------
        input: emebed input file
        inputurl: url of input file
        elevation: in degrees
        azimuth: in degrees
        radarfrequency: in GHz
        
        Returns:
        --------------
        json:
            
    """

    return

if __name__ == "__main__":
    pass
    # old method
    #app.run(host='0.0.0.0', debug=False)
