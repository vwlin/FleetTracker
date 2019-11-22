from map import add_point
from uart import uart_recv
from payload import extract_payload

# get (decrypted) data from uart
#packet = uart_recv()

# deconstruct packet to get all data
#point,timestamp = extract_payload(packet)

# add points to Map
add_point((38.078013, -78.591940),'time')
add_point((38.081811, -78.593103),'time')
add_point((38.057178, -78.597632),'time')
add_point((38.055148, -78.569812),'time')
add_point((38.042553, -78.511540),'time')
add_point((38.032519, -78.514622),'time')
add_point((38.031613, -78.511322),'time')

