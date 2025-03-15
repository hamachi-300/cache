# default dictionary can set inital value of each key
from collections import defaultdict

# LFU (Least Frequency Used) : it will store frequency of key and when size exceed will terminate least frequency

class LFU:
    def __init__(self, size: int):
        self.cache = {}
        # default dictionary of int = 0
        self.freq = defaultdict(int)
        self.size = size

    def get(self, key: str):
        if key not in self.cache:
            return None
        self.freq[key] += 1
        return self.cache[key]
    
    def getKeys(self):
        keys = [k for k, d in self.cache.items()]
        return keys
    
    def put(self, key: str, value: str):
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
        self.cache[key] = value
        self.freq[key] = 1 