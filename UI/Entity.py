# Entity.py
#
#   Entity class fields:
#       - id:           int
#       - points:       vector
#       - timestamps:   vector
#
#   Functions:
#       - addPoint(Payload)
#
##############################################
from Payload import Payload


################### ENTITY CLASS ########################
class Entity:
    '''
    init Entity: 
        - input     -   Payload object
        - purpose   -   initialize points & timestamps vector
    '''
    def __init__(self,payload): 
        self.id = payload.devID
        self.points = []
        self.timestamps = []
        self.addPoint(payload)
    '''
    function addPoint: 
        - input     -   Payload object
        - purpose   -   append point & timestamp from Payload to appropriate vectors
    '''
    def addPoint(self,payload):
        self.points.append(payload.coord)
        self.timestamps.append(payload.getCallout())
