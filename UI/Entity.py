# Entity.py
#
#   Entity fields:
#       - 
#
#   Functions:
#       - 
#
##############################################
from Payload import Payload

class Entity:
    def __init__(self,payload): 
        self.id = payload.devID
        self.points = []
        self.points.append(payload.coord)
        self.timestamps = []
        self.timestamps.append(payload.getTimeStr())

    def addPoint(self,payload):
        self.points.append(payload.coord)
        self.timestamps.append(payload.getTimeStr())
