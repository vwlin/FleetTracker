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

def str_to_coord(data_str):
    # convert string to number
    data_str = data_str + '000000'          # bit shift 6 to the left
    coord = 0
    exp = 0
    negative = False
    # if negative perform two's complement
    neg_str = ''
    if data_str[0] == '1':
        negative = True
        for i in range(len(data_str)):
            if data_str[i] == '1': neg_str = neg_str + '0'
            if data_str[i] == '0': neg_str = neg_str + '1'
        data_str = neg_str
    
    for i in reversed(data_str):
        if i == '1':
            coord = coord + 2**exp;
        exp  = exp + 1
    if negative: coord = (-1)*(coord + 1)
    result = float(coord) * 10**-7          # multiply result by 10^-7
    return result

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
        self.lat = str_to_coord(data[38:64])  # 26 bits

        #   Longitude
        #print(data[64:90])
        self.lon = str_to_coord(data[64:90])  # 26 bits
        self.coord = (self.lat,self.lon)
        
        #   Timestamp
        self.day = int(data[90:95],2)         # 5 bits
        #print(self.day)
        self.mon = int(data[95:99],2)         # 4 bits
        #print(self.mon)
        #print(data[99:104])
        self.hrs = int(data[99:104],2)        # 5 bits
        #print(self.hrs)
        self.min = int(data[104:110],2)       # 6 bits
        #print(self.min)
        self.sec = int(data[110:116],2)       # 6 bits
        #print(self.sec)
        
        # self.unused = int(data[116:120],2)    # 4 bits
    
    def getCallout(self):
        return 'Device #'+str(self.devID)+'\nDate: '+str(self.mon)+'/'+str(self.day)+'\nTime: '+str(self.hrs)+':'+str(self.min)+':'+str(self.sec)
        
    def getLatLonStr(self):
        return str(self.lat)+', '+str(self.lon)
        
        
        
        
        
        