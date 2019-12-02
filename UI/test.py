from Map import Map
from uart import uart_recv
from Entity import Entity
from Payload import Payload

# get (decrypted) data from uart
#packet = uart_recv()

# deconstruct packet to get all data
# payload = Payload(packet)

#entity = Entity(38.078013, -78.591940)
map = Map()

def dec2bin(num,whol,dec):
    i, d = divmod(abs(float(num)), 1)
    bini = ''
    bind = ''
    while (whol > 0):
        whol = whol - 1
        exp = 2**whol
        if (exp <= i):
            bini = bini + '1'
            i = i - exp
        else:
            bini = bini + '0'
    for i in range(dec):
        d = d * 2
        if (d >= 1):
            d = d - 1
            bind = bind + '1'
        else:
            bind = bind + '0'
    return bini,bind

def coordinate(num):
    num = float(num)
    sign = '0'
    if (num < 0):
        sign = '1'
    num = abs(num)
    whol,dec = dec2bin(num,8,17)
    return sign+whol+dec

def make_payload(point,time,id):
    data = '0'                          # 0:1   seqnum
    devID,_ = dec2bin(int(id),13,0)     # 1:14  devID
    data = data + devID
    for i in range(14,38):              # 14:38 ADC readings
        data = data + '0'
    lat = coordinate(point[0])
    lon = coordinate(point[1])
    data = data + lat + lon
    for i in range(22):
        data = data + '0'
    return Payload(data)
#make_payload((38.07801, -78.59194),'time')

# add points to Map
map.add_point(make_payload((38.07801, -78.59194),'time',1))
map.add_point(make_payload((37.986938, -78.589207),'time',2))
map.add_point(make_payload((38.018806, -78.442065),'time',3))
map.add_point(make_payload((38.08181, -78.59310),'time',1))
map.add_point(make_payload((38.013119, -78.469684),'time',3))
map.add_point(make_payload((38.05718, -78.59763),'time',1))
map.add_point(make_payload((38.008365, -78.506728),'time',3))
map.add_point(make_payload((37.999792, -78.581367),'time',2))
map.add_point(make_payload((38.022968, -78.547592),'time',3))
map.add_point(make_payload((38.014118, -78.548766),'time',2))
map.add_point(make_payload((38.05518, -78.56981),'time',1))
map.add_point(make_payload((38.028170, -78.528508),'time',2))
map.add_point(make_payload((38.027997, -78.588785),'time',3))
map.add_point(make_payload((38.04255, -78.51154),'time',1))
map.add_point(make_payload((38.046555, -78.526092),'time',2))
map.add_point(make_payload((38.057625, -78.498955),'time',2))
map.add_point(make_payload((38.034777, -78.622322),'time',3))
map.add_point(make_payload((38.03252, -78.51462),'time',1))
map.add_point(make_payload((38.037180, -78.702496),'time',3))
map.add_point(make_payload((38.047371, -78.742352),'time',3))
# map.add_point(make_payload((38.041155, -78.831714),'time',3)) # test going out of frame (bus does go out of window frame)
map.add_point(make_payload((38.077617, -78.477289),'time',2))
map.add_point(make_payload((38.03161, -78.51132),'time',1))


''' 
ROUTE 1
map.add_point(make_payload((38.07801, -78.59194),'time',1))
map.add_point(make_payload((38.08181, -78.59310),'time',1))
map.add_point(make_payload((38.05718, -78.59763),'time',1))
map.add_point(make_payload((38.05518, -78.56981),'time',1))
map.add_point(make_payload((38.04255, -78.51154),'time',1))
map.add_point(make_payload((38.03252, -78.51462),'time',1))
map.add_point(make_payload((38.03161, -78.51132),'time',1))

ROUTE 2
map.add_point(make_payload((37.986938, -78.589207),'time',2))
map.add_point(make_payload((37.999792, -78.581367),'time',2))
map.add_point(make_payload((38.014118, -78.548766),'time',2))
map.add_point(make_payload((38.028170, -78.528508),'time',2))
map.add_point(make_payload((38.046555, -78.526092),'time',2))
map.add_point(make_payload((38.057625, -78.498955),'time',2))
map.add_point(make_payload((38.077617, -78.477289),'time',2))

ROUTE 3
map.add_point(make_payload((38.018806, -78.442065),'time',3))
map.add_point(make_payload((38.013119, -78.469684),'time',3))
map.add_point(make_payload((38.008365, -78.506728),'time',3))
map.add_point(make_payload((38.022968, -78.547592),'time',3))
map.add_point(make_payload((38.027997, -78.588785),'time',3))
map.add_point(make_payload((38.034777, -78.622322),'time',3))
map.add_point(make_payload((38.037180, -78.702496),'time',3))
map.add_point(make_payload((38.037180, -78.702496),'time',3))
map.add_point(make_payload((38.041155, -78.831714),'time',3))


'''









