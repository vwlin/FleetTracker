# Map.py
#
#   Map class fields:
#        - driver:          webdriver       opens Chrome window
#        - dirpath:         string          path of directory containing index.html file
#        - center:          (lat,lon)       coordinate at which map is centered
#        - last_pt:         (lat,lon)       coordinate of last location of map
#        - icon:            CustomIcon      bus icon for last coordinate
#        - fleet:           Fleet object
#
#   Functions:
#       - addPoint(Payload)
#
##########################################################




############### ADJUSTABLE ATTRIBUTES ####################
BUFFER = 1          # maximum allowed difference btwn previous & current lat and lon coordinates
ZOOM_FACTOR = 30    # level of zoom for map
RELOAD = True       # select whether program should 'press F5' every time map is updated to reload Chrome index.html page
LOAD_DRIVER = False # select whether to automatically open index.html file
##########################################################




import os 
import folium
from folium import plugins
from folium.features import CustomIcon
import matplotlib.pyplot as plt
from selenium import webdriver
from Fleet import Fleet
from Payload import Payload
import pyautogui

################### MAP CLASS ########################
class Map:
    '''
    init Map: 
        - input     -   (lat,lon) coord specifying where to center map
        - purpose   -   open serial connection at COM##
    '''
    def __init__(self,coord):
        # Create a map using the Map() function and the coordinates for Charlottesville
        #self.map = folium.Map(location=[38.055148, -78.569812], zoom_start=12)
        if LOAD_DRIVER: self.driver = webdriver.Chrome()
        self.dirpath = os.path.dirname(os.path.abspath(__file__))
        
        # draw initial map
        self.center = coord
        #self.centers.append([coord[0],coord[1]])
        my_map = folium.Map(location=[coord[0],coord[1]], zoom_start=ZOOM_FACTOR)
        self.last_pt = coord
        my_map.save('index.html')
        if LOAD_DRIVER: self.driver.get("file:///"+self.dirpath+"/index.html")
        
        icon_image = 'bus.png'
        self.icon = CustomIcon('bus.png',icon_size=(50, 50),popup_anchor=(0,-10))
        self.fleet = Fleet()
    '''
    function add_point: 
        - input     -   Payload received from Uart
        - purpose   -   add point to map if it is within BUFFER of previous coordinate
                    -   create popup from Payload 
                    -   draw route of each entity and add bus icon marker and popup at coordinate in payload
                    -   save map to index.html, 'press F5' to reload index.html in Chrome (if option selected at top)
        - return    -   nothing
    '''        
    def add_point(self,payload):
        if ((abs(payload.lat-self.last_pt[0]) < BUFFER) and (abs(payload.lon-self.last_pt[1]) < BUFFER)):
            point = (payload.lat,payload.lon)
            self.last_pt = point
            callout = payload.getCallout()
            self.fleet.processPayload(payload)
            #redraw map
            my_map = folium.Map(location=[self.center[0],self.center[1]], zoom_start=ZOOM_FACTOR)
            #add markers
            for entity in self.fleet.getEntities():
                # add all markers but the last one
                for i in range(len(entity.points)):
                    pt = entity.points[i]
                    ts = entity.timestamps[i]
                    if (i >= len(entity.points)-1):
                        marker = folium.Marker(
                            location=[pt[0], pt[1]],
                            icon=CustomIcon('bus.png',icon_size=(50, 50),popup_anchor=(0,-10)),
                            popup=folium.Popup(ts,max_width = 80, min_width=80, min_height=200, max_height=200)
                        )#.add_to(my_map)
                        my_map.add_child(marker)
                #add lines
                folium.PolyLine(entity.points).add_to(my_map)
            # display map
            my_map.save('index.html')
            if RELOAD: pyautogui.press('f5')     # press the F1 key