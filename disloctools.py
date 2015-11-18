
"""disloc tools
    -- generate disloc inputFile
    -- run disloc binary
"""

import subprocess

def createDislocInputFile(dislocParams, faults, inputFile):
    """create disloc inputfile
        inputFile: username + projectname + ".input" (suggestion)
    """

    """disloc input file format
    Use this form to upload one or more faults that are already in Disloc input file format. The following example shows formatting:
    Line 1: 32.904255 -115.526449 1 (this is the lat, lon of the origin; and "1" signifies use of a grid).
    Line 2: -75 1 151 -40 1 41 (the grid: x0, x_delta, x_number, y0, y_delta, y_number)
    Line 3: 20.489759271 -80.624111128 355.0 (first fault patch: x, y (km) from origin and strike (degrees)
    Line 4: 0 1.21 45.0 1.0 1.0 -0.0 -0.0 0.0 3.0 3.0 (fault_type 0 for point dislocation, depth, dip (degrees), lambda, mu,u1,u2,u3, length, width)
    Repeat the formats for Lines 3 and 4 for each additional fault.

    Sample:
    Afgan1.input
    36.5 70.7 1
    -100.0 10.0 21 -100.0 10.0 21
    0.0 0.0 106.0
    1 213.5 70.0 1.0 1.0 0.0 5000.0 0.0 20.0 19.0
    -19.2252027 5.512789 106.009903
    1 213.5 70.0 1.0 1.0 0.0 5000.0 0.0 19.988104 19.0
    """

    # with block will auto close file
    with open(inputFile, "w") as f:
        # print header line
        if dislocParams.getObservationPointStyle() == 1:
            linestr = " ".join(map(str, [dislocParams.getOriginLat(), dislocParams.getOriginLon(), dislocParams.getObservationPointStyle()]))
            f.write(linestr + "\n")
        elif dislocParams.getObservationPointStyle() == 0:
            pass

        # print grids
        if dislocParams.getObservationPointStyle() == 1:
            linestr = " ".join(map(str, [dislocParams.getGridMinXValue(), dislocParams.getGridXSpacing(), dislocParams.getGridXIterations(), dislocParams.getGridMinYValue(),dislocParams.getGridYSpacing(), dislocParams.getGridYIterations()]))
            f.write(linestr + "\n")
        if dislocParams.getObservationPointStyle() == 0:
            pass

        # print faults
        FAULT_LINE_PREFIX = 1
        for singlefault in faults:
            linestr = " ".join(map(str, [singlefault.faultLocationX, singlefault.faultLocationY, singlefault.faultStrikeAngle]))
            f.write(linestr + "\n")

            linestr = " ".join(map(str, [FAULT_LINE_PREFIX, singlefault.faultDepth, singlefault.faultDipAngle, singlefault.faultLameLambda, singlefault.faultLameMu, singlefault.faultStrikeSlip, singlefault.faultDipSlip, singlefault.faultTensileSlip, singlefault.faultLength, singlefault.faultWidth]))
            f.write(linestr + "\n")

def createMomentTensorInputFile(eqevent):
    """create disloc input file from moment tesnor solution
        eqevent: dict object with moment tensor solution
    """

    pass


def executeDisloc(inputfile, outfile):
    """executeDisloc binary"""

    # "gcc disloc.c -o disloc -lm
    # shall call a configuration for binary location
    dislocbinary = "./disloc_binary"
    rectcode = subprocess.call([dislocbinary, inputfile, outfile])

    if rectcode == 0:
        return True
    else:
        return False


def test_createDislocInputFile():
    """testing createDislocInputFile"""

    # test createDislocInputFile functions
    # sample.input
    # 36.5 70.7 1
    # -100.0 10.0 21 -100.0 10.0 21
    # 0.0 0.0 106.0
    # 1 213.5 70.0 1.0 1.0 0.0 5000.0 0.0 20.0 19.0
    # -19.2252027 5.512789 106.009903
    # 1 213.5 70.0 1.0 1.0 0.0 5000.0 0.0 19.988104 19.0

    from dislocclasses import Fault, DislocParamsBean
    dislocparams = DislocParamsBean()
    dislocparams.setOriginLat(36.5)
    dislocparams.setOriginLon(70.7)
    dislocparams.setObservationPointStyle(1)
    dislocparams.setGridMinXValue(-100.0)
    dislocparams.setGridMinYValue(-100.0)
    dislocparams.setGridXSpacing(10.0)
    dislocparams.setGridYSpacing(10.0)
    dislocparams.setGridXIterations(21)
    dislocparams.setGridYIterations(21)

    testfault = Fault()
    # lon, lat are ignored here
    testfault.setFaultLocationX(0.0)
    testfault.setFaultLocationY(0.0)
    testfault.setFaultStrikeAngle(106)

    testfault.setFaultDepth(213.5)
    testfault.setFaultDipAngle(70.0)
    testfault.setFaultLameLambda(1.0)
    testfault.setFaultLameMu(1.0)
    testfault.setFaultStrikeSlip(0.0)
    testfault.setFaultDipSlip(5000.0)
    testfault.setFaultTensileSlip(0.0)
    testfault.setFaultLength(20.0)
    testfault.setFaultWidth(19.0)

    # testfault.setFaultLatEnd()
    # testfault.setFaultLatStart()
    # testfault.setFaultLocationZ(0)
    # testfault.setFaultLonEnd()
    # testfault.setFaultLonStart()
    # testfault.setFaultName("")
    # testfault.setFaultRakeAngle(0)

    secondfault = Fault()

    secondfault.setFaultLocationX(-19.2252027)
    secondfault.setFaultLocationY(5.512789)
    secondfault.setFaultStrikeAngle(106.009903)

    secondfault.setFaultDepth(213.5)
    secondfault.setFaultDipAngle(70.0)
    secondfault.setFaultLameLambda(1.0)
    secondfault.setFaultLameMu(1.0)
    secondfault.setFaultStrikeSlip(0.0)
    secondfault.setFaultDipSlip(5000.0)
    secondfault.setFaultTensileSlip(0.0)
    secondfault.setFaultLength(19.988104)
    secondfault.setFaultWidth(19.0)

    testfile = "test.input"

    # create input file
    createDislocInputFile(dislocparams, [testfault, secondfault], testfile)


def test_executeDisloc():
    """testing executeDisloc"""

    # run disloc binary
    testfile = "test.input"
    testoutput = "test.output"
    executeDisloc(testfile, testoutput)


def test_createMomentTensorInputFile():
    """testing createMomentTensorInputFile"""

    from earthquakefeed import check_event_geojson

    eventurl = "http://earthquake.usgs.gov/earthquakes/feed/v1.0/detail/us200041ty.geojson"
    print(check_event_geojson(eventurl))


def debug():
    """testing functions in disloctools"""

    # test_createDislocInputFile()
    # test_executeDisloc()

    test_createMomentTensorInputFile()

if __name__ == "__main__":
    debug()
