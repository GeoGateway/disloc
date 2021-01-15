#!/usr/bin/env python
"""
    disloc_exec.py
        -- execute disloc.c with an input file
"""

import os
import sys
import json
import random
import datetime
import subprocess
import argparse


from SARImage import lineofsight 

def setoutputlocation():
	""" return a folder for output """

	# make a random folder for each job
	# add time stamp
	wkdir = "disloc" + str(random.randint(1,9999)).zfill(4) + datetime.datetime.now().strftime("%Y%M%d%H%M%S")
	# put output into static folder
	# sample url: http://192.168.59.130:8000/static/kml1867/postseismic.txt

	wkpath = os.path.dirname(os.path.realpath(__file__)) + "/static"

	outputdir = wkpath + os.path.sep + wkdir

	if not os.path.exists(outputdir):
		os.mkdir(outputdir)

	return outputdir

def getbinary():
    """ get the location of disloc """
    
    # assume the same location as the script
    script_path = os.path.dirname(os.path.realpath(__file__))
    #disloc_binary = script_path + os.path.sep + "disloc_table"
    disloc_binary = script_path + os.path.sep + "disloc"
    
    return disloc_binary

def getURLprefix():
    """return url prefix for output"""
    
    urlprefix = "http://localhost/disloc/" 
    
    return urlprefix

def exec_disloc(input, output, workdir=False):
    """ execute disloc with input and output """

    disloc_binary = getbinary()

    if workdir:
        os.chdir(workdir)
    
    proc = subprocess.Popen([disloc_binary,input,output], stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    try:
        outs, errs = proc.communicate(timeout=15)
    except subprocess.TimeoutExpired:
        proc.kill()
        outs, errs = proc.communicate()
        # remove output file, save disk space
        if os.path.exists(output):
            os.remove(output)
        exec_status = {"status":"failed","error":"timeout"}
        return exec_status
    
    if proc.returncode == 0:
        exec_status = {"status":"success","error":""}
    else:
        exec_status = {"status":"failed","error":errs}

    return exec_status

def dislocworkflow(args):
    """disloc workflow
        args: dict object from restAPI call

        step 0: working dir and input file
        step 1: disloc
        step 2: SARImage
        step N: result json
    """
    
    # step 0: working dir and input file
    # create a working dir
    outputdir = False
    # create a working dir for api call
    if 'api' in args:
        if 'workdir' in args:
            outputdir = args['workdir']
    else:
        outputdir = setoutputlocation()

    # readfile directly
    if "inputfile" in args:
        with open(args['inputfile'],'r') as f:
            inputdata = f.read()
            args['input'] = inputdata

    if outputdir: 
        inputfile = outputdir + os.path.sep + "input.txt"
    else:
        inputfile = "input.txt"
        
    # write input file attached input
    if 'input' in args:
        with open(inputfile,"w") as f:
            f.write(args['input'])

    # step 1: disloc
    # exec disloc.c
    if 'output' in args:
        outputfile = args['output'] + ".csv"
    else:
        outputfile = "output.csv"

    disloc_status = exec_disloc(inputfile,outputfile, workdir = outputdir)    

    # dislo failed
    if disloc_status['status'] != 'success':
        err = disloc_status['error']
        return {"status":"failed","error":"disloc failed"}
    
    # step 2: SARImage
    # three paras:
    # elevation: 60
    # azimuth: 0
    # radarfrequency: 1.26
    # step N: result JSON
    
    paralist = {"elevation":60,"azimuth":0,"radarfrequency":1.26}
    for key in paralist:
        if key in args:
            paralist[key] = float(args[key])
    
    radarfrequency = paralist['radarfrequency']*10**9 # Ghz to hz
    radarwavelength = 299792458.0/radarfrequency * 100.0 # Convert to cm

    imageURL = ""
    dislocOutput = outputfile
    # west = str(extent[0])
    # east = str(extent[1])
    # south = str(extent[2])
    # north = str(extent[3])
    # # [west,east,south,north]
    imageextent = lineofsight(paralist['elevation'], paralist['azimuth'],radarwavelength,dislocOutput, imageURL)
    west = imageextent[0]
    east = imageextent[1]
    south =imageextent[2]
    north =imageextent[3]
    disloc_status["latlonbox"] = {"north":north,"south":south,"east":east,"west":west}    
    disloc_status["parameters"] = paralist
    # list of output file
    if outputdir:
        filelist = os.listdir(outputdir)
    else:
        filelist = os.listdir()

    filelist.append('summary.json')
        
    if 'api' in args:
        disloc_status['output'] = filelist
    else:
        urlprefix = getURLprefix()
        foldername = os.path.basename(outputdir)
        urlslist = [urlprefix + foldername + "/" + x for x in filelist]
        disloc_status['output'] = urlslist
    
    if outputdir: 
        jobsummary = outputdir + os.path.sep + "summary.json"
    else:
        jobsummary = "summary.json"
    
    # write summary.json
    with open(jobsummary,"w") as f:
        f.write(json.dumps(disloc_status,indent = 4))

    return disloc_status

def _getParser():
    
    # input: emebed input file
    # optional:
    # output: name of output file 
    # elevation: in degrees, default: 60
    # azimuth: in degrees, default: 0
    # radarfrequency: in GHz, default: 1.26
    # workdir: working directory

    parser = argparse.ArgumentParser(description="Execute disloc workflow.")
    parser.add_argument('-i','--input', action='store', dest='inputfile',required=True,help='disloc input file')
    parser.add_argument('-o','--output', action='store', dest='output',required=False,help='output file name prefix')
    parser.add_argument('-el','--elevation', action='store', dest='elevation',required=False,help='elevation in degrees, default: 60')
    parser.add_argument('-az','--azimuth', action='store', dest='azimuth',required=False,help='azimuth in degrees, default: 0')
    parser.add_argument('-rf','--radarfrequency', action='store',dest='radarfrequency',required=False,help='radarfrequency: in GHz, default: 1.26')
    parser.add_argument('-wd','--workdir', action='store',dest='workdir',required=False,help='working directory')
    return parser

def main():

    # Read command line arguments
    parser = _getParser()
    paras = vars(parser.parse_args())
    # remove none paras
    noneparas = {k: v for k, v in paras.items() if v is not None}
    noneparas['api'] = False
    results = dislocworkflow(noneparas)
    print(results)
    # test exec_disloc
    #test case 1
    # input_1 = "test/simple.input"
    # output_1 = "test/simple.output.csv"
    # status = exec_disloc(input_1,output_1)
    # print(status)
    # #test case 2
    # input_2 = "test/simple.input"
    # output_2 = ""
    # status = exec_disloc(input_2,output_2)
    # print(status)
    # #test case 3
    # input_3 = "KO1.input"
    # output_3 = "KO1.input.csv"
    # status = exec_disloc(input_3,output_3,workdir="test")
    # print(status)


if __name__ == '__main__':
    main()