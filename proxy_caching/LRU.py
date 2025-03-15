# ordered dictionary have functions to order
from collections import OrderedDict 

# LRU (Least Recently used) : it will move key which used to front and when data exceed it will terminate last item and add new

class LRU:
    # contructor function set size of cache
    def __init__(self, size: int):
        self.cache = OrderedDict()
        self.size = size
    
    # get function for get data from cache by key
    def get(self, key: str):
        if key not in self.cache:
            return None
        else:
            # move accessed item to start (most recent used)
            self.cache.move_to_end(key, last=False)
            return self.cache[key]

    # put function for put data to cache
    def put(self, key: str, data: str):
        # move to most recent used zone if it in cache
        if key in self.cache:
            self.cache.move_to_end(key, last=False)

        # put data (if not key it will automate insert)
        self.cache[key] = data
        
        # if cache out of size remove last item (least recently used item)
        if (len(self.cache) > self.size):
            self.cache.popitem(last=True)