# Payload.py
#
#   Payload fields:
#       - seqNum
#       - devID
#       - volt
#       - temp
#       - lat
#       - lon
#       - coord
#       - day
#       - hrs
#       - min
#       - sec
#
#   Functions:
#       - getTimeStr()
#       - getLatLonStr()
#
##############################################

def nums_to_coord(sign, whole, dec):
    return ((-1)**sign)*float(whole+dec)

def bin2frac(bin):
    frac = 0;
    for i in range(len(bin)):
        if (bin[i] == '1'):
            exp = (1/2)**(i+1)
            frac = frac + exp
    return frac
  
class Payload:
    def __init__(self, data): 
        #self.seq_num  = int(data[0:1],2)     # 1 bit
        self.devID   = int(data[1:14],2)      # 13 bits

        # ADC_readings                    24 bits: 
        self.volt = int(data[14:26],2)        # 12 bits
        self.temp = int(data[26:38],2)        # 12 bits

        # GPS data                        74 bits: 
        #   Latitude
        self.lat_sign = int(data[38:39],2)    # 1 bit
        self.lat_whol = int(data[39:47],2)    # 8 bits
        self.lat_decl = bin2frac(data[47:64]) # 17 bits
        self.lat = nums_to_coord(self.lat_sign,self.lat_whol,self.lat_decl)

        #   Longitude
        self.lon_sign = int(data[64:65],2)    # 1 bit
        self.lon_whol = int(data[65:73],2)    # 8 bits
        self.lon_decl = bin2frac(data[73:90]) # 17 bits
        self.lon = nums_to_coord(self.lon_sign,self.lon_whol,self.lon_decl)
        self.coord = (self.lat,self.lon)
        
        #   Timestamp
        self.day = int(data[90:95],2)         # 5 bits
        self.hrs = int(data[95:100],2)        # 5 bits
        self.min = int(data[100:106],2)       # 6 bits
        self.sec = int(data[106:112],2)       # 6 bits
    
    def getTimeStr(self):
        return str(self.hrs)+':'+str(self.min)+':'+str(self.sec)
        
    def getLatLonStr(self):
        return str(self.lat)+', '+str(self.lon)
        
        
        
        
        
        