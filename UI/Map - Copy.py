import matplotlib.pyplot as plt
import matplotlib.cm
 
from mpl_toolkits.basemap import Basemap
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
from matplotlib.colors import Normalize
from matplotlib.cbook import get_sample_data
import numpy as np

class Map():
    def __init__(self):
        self.x = []
        self.y = []
        self.fig = []
    
    def add_point(self,lat,lon):
        self.draw_map(lon,lat)
        
    def draw_map(self,xp,yp):
        plt.ion()
        plt.close()
        fig, ax = plt.subplots(figsize=(40,30))

        # c, l, i, h, f or None

        ''' Virginia
        m = Basemap(resolution='l', projection='merc', 
                    lat_0=54.5, lon_0=-4.36, 
                    llcrnrlon=-83.71, llcrnrlat=36.51, 
                    urcrnrlon=-75.88, urcrnrlat=39.5)
                    #westlimit=-83.71; southlimit=36.51; eastlimit=-75.88; northlimit=39.5
        '''
        # Albemarle county
        m = Basemap(resolution='l', projection='merc', 
                    lat_0=54.5, lon_0=-4.36, 
                    llcrnrlon=-78.850380, llcrnrlat=37.721760, 
                    urcrnrlon=-78.206402, urcrnrlat=38.273587)
                    #37.721760, -78.850380          38.273587, -78.206402
        #m.drawmapboundary(fill_color='#46bcec')
        #m.fillcontinents(color='#f2f2f2',lake_color='#46bcec')
        #m.drawcoastlines()

        #m.readshapefile('boundaries/states', 'states')
        #m.readshapefile('boundaries/roads/tl_2015_51_prisecroads','roads')
        m.readshapefile('boundaries/albemarle_roads/tl_2019_51003_roads','alb_roads',color='r')
        m.readshapefile('boundaries/cville_roads/tl_2015_51540_roads','cvl_roads',color='r')
        m.readshapefile('boundaries/tl_2016_51_cousub', 'counties')

        xp, yp = m(xp, yp)
        self.x.append(xp)
        self.y.append(yp)

        plt.plot(self.x, self.y, 'go', markersize=3)
        plt.plot(self.x, self.y, 'g')
        fig.savefig('plot.png')
        plt.show()
