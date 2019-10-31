import matplotlib.pyplot as plt
import matplotlib.cm
 
from mpl_toolkits.basemap import Basemap
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
from matplotlib.colors import Normalize
from matplotlib.cbook import get_sample_data
import numpy as np

from Map import Map

map = Map()
map.init_map()
map.add_point(38.078013, -78.591940)
map.add_point(38.081811, -78.593103)
map.add_point(38.057178, -78.597632)
map.add_point(38.055148, -78.569812)
map.add_point(38.042553, -78.511540)
map.add_point(38.032519, -78.514622)
map.add_point(38.031613, -78.511322)
map.draw_map()
