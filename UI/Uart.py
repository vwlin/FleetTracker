# Uart.py
#
#   Uart class fields:
#        - serial object (one serial connection per Uart object)
#        - baudrate
#        - port number
#
#   Functions:
#       - getTimeStr()
#       - getLatLonStr()
#
##########################################################

import serial


################### UART CLASS ########################
class Uart:
    '''
    init Uart: 
        - input     -   COM port number
        - purpose   -   open serial connection at COM##
    '''
    def __init__(self, portnum): 
        self.ser = serial.Serial()
        self.ser.baudrate = 9600
        self.ser.port = 'COM'+str(portnum)
        self.ser.open()
    '''
    function uart_recv: 
        - input     -   nothing
        - purpose   -   read line from Uart until a long hex string detected
                    -   convert hex bytes into binary string
        - return    -   string of 128-bit payload in binary
    '''
    def uart_recv(self):
        line = ''
        while ((len(line) < 30) or (line is None)):
            line = self.ser.readline()        # read line from serial
            line = line.decode("utf-8") 
            if (not ((len(line) < 30) or (line is None))):
                if (line[0].isnumeric()):
                    bytes = line.split()                 # split bytes by space
                    data = ''
                    for b in bytes:
                        temp = str(bin(int(b,16)))
                        temp = temp[2:len(temp)]
                        length = len(temp)
                        for i in range(8-length):
                            temp = '0' + temp
                        data = data + temp
                    return data