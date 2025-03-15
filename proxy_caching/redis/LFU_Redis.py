# default dictionary can set inital value of each key
from collections import defaultdict
import time

# LFU (Least Frequency Used) : it will store frequency of key and when size exceed will terminate least frequency

class LFU:
    def __init__(self, size: int, ttl: int):
        self.cache = {}
        # default dictionary of int = 0
        self.freq = defaultdict(int)
        self.size = size
        self.ttl = ttl

    # check expire item
    def is_expired(self, key):
        if key not in self.cache:
            return True
        _, expire_time = self.cache[key]
        return time.time() > expire_time

    def get(self, key: str):

        # check expired data
        if key not in self.cache or self.is_expired(key):
            if key in self.cache:
                del self.cache[key]
                del self.freq[key]
            return None

        self.freq[key] += 1
        return self.cache[key][0]
    
    def getKeys(self):
        keys = [k for k, d in self.cache.items()]
        return keys
    
    def put(self, key: str, data: str):
        # check expired data
        expired_keys = [k for k in self.cache.keys() if self.is_expired(k)]
        for k in expired_keys:
            del self.cache[k]
            del self.freq[k]

        # check cache size
        if len(self.cache) >= self.size:
            # get LRU keys
            least_freq = min(self.freq.values(), default=None)
            keys_to_remove = [k for k, v in self.freq.items() if v == least_freq]
            # delete all items that have LFU
            for k in keys_to_remove:
                del self.cache[k]
                del self.freq[k]
        # insert new key and data
        expire_time = time.time() + self.ttl
        self.cache[key] = (data, expire_time)
        self.freq[key] = 1