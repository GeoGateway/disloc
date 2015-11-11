
"""fault.py
    define fault class
"""


class Fault():
    """ fault class
           double faultDepth,
           double faultDipAngle,
           double faultDipSlip,
           double faultLameLambda,
           double faultLameMu,
           double faultLatEnd,
           double faultLatStart,
           double faultLength,
           double faultLocationX,
           double faultLocationY,
           double faultLocationZ,
           double faultLonEnd,
           double faultLonStart,
           java.lang.String faultName,
           double faultRakeAngle,
           double faultStrikeAngle,
           double faultStrikeSlip,
           double faultTensileSlip,
           double faultWidth)
    """

    def __int__(self):
        #  python cannot define multiple constructors as in Java
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
