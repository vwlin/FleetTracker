# Fleet.py
#
#   Fleet fields:
#       - 
#
#   Functions:
#       - 
#
##############################################
from Payload import Payload
from Entity import Entity

class Fleet:
    def __init__(self): 
        self.fleet = []
    
    def processPayload(self,payload):
        found = False
        for entity in self.fleet:
            if (entity.id == payload.devID):
                found = True
                entity.addPoint(payload)
        if not found:
            entity = Entity(payload)
            self.fleet.append(entity)
    def getEntities(self):
        return self.fleet
            
    def printFleet(self):
        for entity in self.fleet:
            print(str(entity.id)+': ')
            for point in entity.points:
                print(point)