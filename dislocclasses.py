
"""Classes related with disloc service
    -- Fault
    -- DislocParamsBean
"""


class Fault():
    """fault class
           self, faultDepth,
           self, faultDipAngle,
           self, faultDipSlip,
           self, faultLameLambda,
           self, faultLameMu,
           self, faultLatEnd,
           self, faultLatStart,
           self, faultLength,
           self, faultLocationX,
           self, faultLocationY,
           self, faultLocationZ,
           self, faultLonEnd,
           self, faultLonStart,
           java.lang.String faultName,
           self, faultRakeAngle,
           self, faultStrikeAngle,
           self, faultStrikeSlip,
           self, faultTensileSlip,
           self, faultWidth)
    """

    def __init__(self):
        # python cannot define multiple constructors as in Java
        pass

    def setFaultDepth(self, faultDepth):
        self.faultDepth = faultDepth

    def setFaultDipAngle(self, faultDipAngle):
        self.faultDipAngle = faultDipAngle

    def setFaultDipSlip(self, faultDipSlip):
        self.faultDipSlip = faultDipSlip

    def setFaultLameLambda(self, faultLameLambda):
        self.faultLameLambda = faultLameLambda

    def setFaultLameMu(self, faultLameMu):
        self.faultLameMu = faultLameMu

    def setFaultLatEnd(self, faultLatEnd):
        self.faultLatEnd = faultLatEnd

    def setFaultLatStart(self, faultLatStart):
        self.faultLatStart = faultLatStart

    def setFaultLength(self, faultLength):
        self.faultLength = faultLength

    def setFaultLocationX(self, faultLocationX):
        self.faultLocationX = faultLocationX

    def setFaultLocationY(self, faultLocationY):
        self.faultLocationY = faultLocationY

    def setFaultLocationZ(self, faultLocationZ):
        self.faultLocationZ = faultLocationZ

    def setFaultLonEnd(self, faultLonEnd):
        self.faultLonEnd = faultLonEnd

    def setFaultLonStart(self, faultLonStart):
        self.faultLonStart = faultLonStart

    def setFaultName(self, faultName):
        self.faultName = faultName

    def setFaultRakeAngle(self, faultRakeAngle):
        self.faultRakeAngle = faultRakeAngle

    def setFaultStrikeAngle(self, faultStrikeAngle):
        self.faultStrikeAngle = faultStrikeAngle

    def setFaultStrikeSlip(self, faultStrikeSlip):
        self.faultStrikeSlip = faultStrikeSlip

    def setFaultTensileSlip(self, faultTensileSlip):
        self.faultTensileSlip = faultTensileSlip

    def setFaultWidth(self, faultWidth):
        self.faultWidth = faultWidth


class DislocParamsBean():
    """DislocParamsBean class"""

    def __init__(self):
        self.originLat = 0.0
        self.originLon = 0.0
        self.gridMinXValue = 0.0
        self.gridMinYValue = 0.0
        self.gridXSpacing = 0.0
        self.gridYSpacing = 0.0
        self.gridXIterations = 1
        self.gridYIterations = 1
        # GRID_OBSERVATION_STYLE = 1
        # SCATTER_OBSERVATION_STYLE = 0
        self.observationPointStyle = 1

    # Setters
    def setOriginLat(self, originLat):
        self.originLat = originLat

    def setOriginLon(self, originLon):
        self.originLon = originLon

    def setGridMinXValue(self, gridMinXValue):
        self.gridMinXValue = gridMinXValue

    def setGridMinYValue(self, gridMinYValue):
        self.gridMinYValue = gridMinYValue

    def setGridXSpacing(self, gridXSpacing):
        self.gridXSpacing = gridXSpacing

    def setGridYSpacing(self, gridYSpacing):
        self.gridYSpacing = gridYSpacing

    def setGridXIterations(self, gridXIterations):
        self.gridXIterations = gridXIterations

    def setGridYIterations(self, gridYIterations):
        self.gridYIterations = gridYIterations

    def setObservationPointStyle(self, observationPointStyle):
        self.observationPointStyle = observationPointStyle

    # Getters
    def getOriginLon(self):
        return self.originLon

    def getOriginLat(self):
        return self.originLat

    def getGridMinXValue(self):
        return self.gridMinXValue

    def getGridMinYValue(self):
        return self.gridMinYValue

    def getGridXSpacing(self):
        return self.gridXSpacing

    def getGridYSpacing(self):
        return self.gridYSpacing

    def getGridXIterations(self):
        return self.gridXIterations

    def getGridYIterations(self):
        return self.gridYIterations

    def getObservationPointStyle(self):
        return self.observationPointStyle
