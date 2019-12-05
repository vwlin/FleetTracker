from Map import Map
from Entity import Entity
from Payload import Payload
from Uart import Uart

uart = Uart(13)

map = Map()
while(1):
    binary = uart.uart_recv()
    pld = Payload(binary)
    print(pld.devID)
    map.add_point(pld)

