import serial

def uart_recv():
    ser = serial.Serial()
    ser.baudrate = 9600
    ser.port = 'COM12'
    ser.open()
    s = ser.readline()
    s = s.decode("utf-8") 
    print(s)
    s = ser.readline()
    s = s.decode("utf-8") 
    print(s)
    s = ser.readline()
    s = s.decode("utf-8") 
    print(s)
    