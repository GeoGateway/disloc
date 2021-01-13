"""
    disloc_exec.py
        -- execute disloc.c with an input file
"""

import os
import subprocess

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
        outs, errs = proc.communicate(timeout=30)
    except TimeoutExpired:
        proc.kill()
        outs, errs = proc.communicate()
        exec_status = {"status":"failed","error":"timeout"}
    if proc.returncode == 0:
        exec_status = {"status":"success","error":""}
    else:
        exec_status = {"status":"failed","error":errs}

    return exec_status

def dislocworkflow(args):
    """disloc workflow
        args: dict object from restAPI call
    """
    
    return "dislocworkflow"

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