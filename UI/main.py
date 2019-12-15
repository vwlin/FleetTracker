'''
main.py
    purpose: run map user interface
    to run, enter into terminal:
        >> python main.py <COM#>
'''

import sys

if len(sys.argv) != 2:
    print('*ERROR*\n Usage: python {} <COM#>'.format(sys.argv[0]))
    sys.exit(1)
    
from Map import Map
from Entity import Entity
from Payload import Payload
from Uart import Uart

COM_PORT = sys.argv[1]
uart = Uart(COM_PORT)

# starting center coordinate 
coord = (38.0331584, -78.5098304)
map = Map(coord)
while(1):
    binary = uart.uart_recv()
    pld = Payload(binary)
    map.add_point(pld)
    print('Received Data: '+str(pld.data))
    print(pld.getCallout())
    print(pld.coord)
    print('\n')
