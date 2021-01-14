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

from disloc_exec import dislocworkflow

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
        
        optional:
        output: name of output file 
        elevation: in degrees, default: 60
        azimuth: in degrees, default: 0
        radarfrequency: in GHz, default: 1.26
        
        Returns:
        --------------
        json:
            
    """

    args = request.args

    # assume everything is right
    if not("input" in args or 'inputurl' in args):
        return Response("bad request: input or inputurl required!",status=400)

    result = dislocworkflow(args)
    if result['status'] != 'success':
        return Response(json.dumps(result),status=500)
    
    return json.dumps(result)

if __name__ == "__main__":
    pass
    # old method
    #app.run(host='0.0.0.0', debug=False)
