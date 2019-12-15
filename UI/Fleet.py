# Fleet.py
#
#   Fleet class fields:
#       - fleet:    array of Entities
#
#   Functions:
#       - processPayload(Payload)
#       - getEntities()
#       - printFleet()
#
##############################################

from Payload import Payload
from Entity import Entity


################### FLEET CLASS ########################
class Fleet:
    '''
    init Fleet: 
        - input     -   Nothing
        - purpose   -   initialize fleet array
    '''
    def __init__(self): 
        self.fleet = []
    '''
    function processPayload: 
        - input     -   nothing
        - purpose   -   find Entity with matching device ID
                    -   if found add point, else create new Entity in fleet and add  point
        - return    -   nothing
    '''
    def processPayload(self,payload):
        found = False
        for entity in self.fleet:
            if (entity.id == payload.devID):
                found = True
                entity.addPoint(payload)
        if not found:
            entity = Entity(payload)
            self.fleet.append(entity)
    '''
    function getEntities: 
        - input     -   nothing
        - return    -   array of Entities in fleet
    '''
    def getEntities(self):
        return self.fleet
    '''
    function printFleet: 
        - input     -   nothing
        - purpose   -   print entity id's and points, for debugging
        - return    -   nothing
    '''  
    def printFleet(self):
        for entity in self.fleet:
            print(str(entity.id)+': ')
            for point in entity.points:
                print(point)