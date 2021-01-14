"""
    disloc_exec.py
        -- execute disloc.c with an input file
"""

import os
import json
import random
import datetime
import subprocess

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
    outputdir = setoutputlocation()

    inputfile = outputdir + os.path.sep + "input.txt"

    # write input file
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
    
    radarwavelength = 299792458.0/paralist['radarfrequency'] * 100.0 # Convert to cm

    imageURL = ""
    dislocOutput = outputfile
    lineofsight(paralist['elevation'], paralist['azimuth'],radarwavelength,dislocOutput, imageURL)

    return disloc_status

def main():

    #test case 1
    input_1 = "test/simple.input"
    output_1 = "test/simple.output.csv"
    status = exec_disloc(input_1,output_1)
    print(status)
    #test case 2
    input_2 = "test/simple.input"
    output_2 = ""
    status = exec_disloc(input_2,output_2)
    print(status)
    #test case 3
    input_3 = "KO1.input"
    output_3 = "KO1.input.csv"
    status = exec_disloc(input_3,output_3,workdir="test")
    print(status)


if __name__ == '__main__':
    main()