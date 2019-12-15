# Payload.py
#
#   Payload class fields:
#       - seqNum:   int
#       - devID:    int
#       - volt:     float
#       - temp:     float
#       - lat:      float
#       - lon:      float
#       - coord:    (float lat, float lon)
#       - mon:      int
#       - day:      int
#       - hrs:      int
#       - min:      int
#       - sec:      int
#
#   Functions:
#       - getCallout()
#       - getLatLonStr()
#
##########################################################


#################### HELPER FUNCTIONS ####################
'''
function str_to_coord: 
    - input     -   data_str: (string) 26 binary bits of lat or long data
    - purpose   -   convert from bits to float
    - return    -   result: (float) value of lat/lon coordinate
'''
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

'''
function format_time: 
    - input     -   num, either string or int
    - purpose   -   add leading zero if input < 10
    - return    -   formatted string
'''
def format_time(num):
    formatted = str(num)
    if (num < 10):
        formatted = '0'+formatted
    return formatted
##########################################################





################### PAYLOAD CLASS ########################
class Payload:
    '''
    init Payload: 
        - input     -   string of 128-bit binary payload
        - purpose   -   parse binary payload into appropriate data fields
    '''
    def __init__(self, data):
        self.data = data[0:len(data)-1]
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
        self.mon = int(data[95:99],2)         # 4 bits
        self.day = int(data[90:95],2)         # 5 bits
        self.hrs = int(data[99:104],2)        # 5 bits
        self.min = int(data[104:110],2)       # 6 bits
        self.sec = int(data[110:116],2)       # 6 bits
        # unused = int(data[116:120],2)    # 4 bits
    '''
    function getCallout: 
        - input     -   nothing
        - purpose   -   format popup for map
                    -   display device ID, date, time
        - return    -   string of map popup
    '''
    def getCallout(self):
        return 'Device #'+str(self.devID)+'\nDate: '+str(self.mon)+'/'+format_time(self.day)+'\nTime: '+format_time(self.hrs)+':'+format_time(self.min)+':'+format_time(self.sec)
    '''
    function getLatLonStr: 
        - input     -   nothing
        - return    -   string of latitude,longintude 
    '''  
    def getLatLonStr(self):
        return str(self.lat)+', '+str(self.lon)
        
        
        
        
        
        