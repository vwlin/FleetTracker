import os 
import folium
from folium import plugins
from folium.features import CustomIcon
import matplotlib.pyplot as plt
from selenium import webdriver
from Fleet import Fleet
from Payload import Payload


class Map:
    def __init__(self):
        # Create a map using the Map() function and the coordinates for Charlottesville
        #self.map = folium.Map(location=[38.055148, -78.569812], zoom_start=12)
        self.driver = webdriver.Chrome()
        self.dirpath = os.path.dirname(os.path.abspath(__file__))
        icon_image = 'bus.png'
        self.icon = CustomIcon('bus.png',icon_size=(50, 50),popup_anchor=(0,-10))
        self.fleet = Fleet()
        
    def add_point(self,payload):
        #self.fleet.printFleet()
        point = (payload.lat,payload.lon)
        callout = payload.getCallout()
        self.fleet.processPayload(payload)
        #redraw map
        my_map = folium.Map(location=[38.033129, -78.509628], zoom_start=30)
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
                        popup=ts
                    )#.add_to(my_map)
                    my_map.add_child(marker)
            #add lines
            folium.PolyLine(entity.points).add_to(my_map)
        # mark last point as bus icon
        '''
        last_point = entity.points[-1]
        marker = folium.Marker(
            location=[last_point[0], last_point[1]],
            icon=CustomIcon('bus.png',icon_size=(50, 50),popup_anchor=(0,-10)),
            popup=timestamp
        )#.add_to(my_map)
        my_map.add_child(marker)
        '''

        # display map
        my_map.save('index.html')
        self.driver.get("file:///"+self.dirpath+"/index.html")
        #plt.pause(0.5)

