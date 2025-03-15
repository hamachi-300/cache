# ordered dictionary have functions to order
from collections import OrderedDict 
import time

# LRU (Least Recently used) : it will move key which used to front and when data exceed it will terminate last item and add new

class LRU:
    # contructor function set size of cache
    def __init__(self, size: int, ttl: int):
        self.cache = OrderedDict()
        self.size = size
        self.ttl = ttl

    # check expire item
    def is_expired(self, key):
        if key not in self.cache:
            return True
        _, expire_time = self.cache[key]
        return time.time() > expire_time
    
    # get function for get data from cache by key
    def get(self, key: str):
        # check expired data
        if key not in self.cache or self.is_expired(key):
            if key in self.cache:
                del self.cache[key]
            return None

        # move accessed item to start (most recent used)
        self.cache.move_to_end(key, last=False)
        return self.cache[key][0]
    
    def getKeys(self):
        keys = [k for k, d in self.cache.items()]
        return keys

    # put function for put data to cache
    def put(self, key: str, data: str):

        # check expired data
        expired_keys = [k for k in self.cache.keys() if self.is_expired(k)]
        for k in expired_keys:
            del self.cache[k]

        # move to most recent used zone if it in cache
        if key in self.cache:
            self.cache.move_to_end(key, last=False)

        # put data (if not key it will automate insert)
        expire_time = time.time() + self.ttl
        self.cache[key] = (data, expire_time)
        self.cache.move_to_end(key, last=False)
        
        # if cache out of size remove last item (least recently used item)
        if (len(self.cache) > self.size):
            self.cache.popitem(last=True)