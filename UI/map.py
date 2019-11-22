import os 
import folium
from folium import plugins
from folium.features import CustomIcon
import matplotlib.pyplot as plt
from selenium import webdriver

# set up web driver for Firefox
driver = webdriver.Chrome()

# Create a map using the Map() function and the coordinates for Charlottesville
my_map = folium.Map(location=[38.055148, -78.569812], zoom_start=12)

points = []
timestamps = []

def add_point(point, time):
    dirpath = os.path.dirname(os.path.abspath(__file__))
    points.append(point)
    timestamps.append(time)
    #redraw map
    my_map = folium.Map(location=[38.055148, -78.569812], zoom_start=12)
    #add markers
    for i in range(len(points)-1):
        point = points[i]
        timestamp = timestamps[i]
        folium.Marker(
            location=[point[0], point[1]],
            popup= timestamp
        ).add_to(my_map)
    #add lines
    folium.PolyLine(points).add_to(my_map)
    # add bus icon for last collected point
    icon_image = 'bus.png'
    icon = CustomIcon(
        icon_image,
        icon_size=(50, 50),
        #icon_anchor=(20,30),
        popup_anchor=(0,-10)
    )
    last_point = points[-1]
    last_timestamp = timestamps[-1]
    marker = folium.Marker(
        location=[last_point[0], last_point[1]],
        icon=icon,
        popup=last_timestamp
    )
    my_map.add_child(marker)

    # display map
    my_map.save('index.html')
    driver.get("file:///"+dirpath+"/index.html")
    plt.pause(5)

