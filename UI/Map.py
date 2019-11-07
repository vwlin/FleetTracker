import matplotlib.pyplot as plt
import matplotlib.cm
 
from mpl_toolkits.basemap import Basemap
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
from matplotlib.colors import Normalize
from matplotlib.cbook import get_sample_data
import matplotlib.image as mpimg
import numpy as np
import pickle

class Map():
    def __init__(self):
        self.x = []
        self.y = []
        
    def init_map(self):
        plt.ion()
        fig, ax = plt.subplots(figsize=(40,30))
        #westlimit=-83.71; southlimit=36.51; eastlimit=-75.88; northlimit=39.5
        # Albemarle county
        m = Basemap(resolution='l', projection='merc', 
                    lat_0=54.5, lon_0=-4.36, 
                    llcrnrlon=-78.850380, llcrnrlat=37.721760, 
                    urcrnrlon=-78.206402, urcrnrlat=38.273587)
        m.readshapefile('boundaries/cville_roads/tl_2015_51540_roads','cvl_roads',color='r')
        m.readshapefile('boundaries/albemarle_roads/tl_2019_51003_roads','alb_roads',color='r')
        m.readshapefile('boundaries/tl_2016_51_cousub', 'counties')
        self.m = m
        plt.show()
        #save figure
        pickle.dump(ax, open("myplot.pickle", "wb"))
        plt.close()
    
    def add_point(self,lat,lon):
        xp, yp = self.m(lon,lat)
        self.x.append(xp)
        self.y.append(yp)

    def draw_map(self):
        #plt.ioff()
        plt.close()
        ax = pickle.load(open('myplot.pickle','rb'))
        plt.plot(self.x, self.y, 'go', markersize=2)
        plt.plot(self.x, self.y, 'g')
        plt.show()
        plt.pause(5)
    def draw_map_final(self):
        plt.close()
        plt.ioff()
        ax = pickle.load(open('myplot.pickle','rb'))
        plt.plot(self.x, self.y, 'go', markersize=2)
        plt.plot(self.x, self.y, 'g')
        plt.show()
        






