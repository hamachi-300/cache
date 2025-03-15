import time
import redis

# LRU (Least Recently used) : it will move key which used to front and when data exceed it will terminate last item and add new

class LRU:
    # contructor function set size of cache
    def __init__(self, size: int, ttl: int):
        # create connection with redis and set auto decode to UTF-8
        self.cache = redis.StrictRedis(host="localhost", port=6379, db=0, decode_responses=True)
        self.size = size
        self.ttl = ttl

    # check expire item
    def is_expired(self, key):
        # check ttl mechanism in redis
        ttl = self.cache.ttl(key)
        return ttl <= 0
    
    # get function for get data from cache by key
    def get(self, key: str):
        # check expired data
        if self.is_expired(key):
            self.cache.delete(key)
            return None
        
        # get key if not exist redis will return None
        data = self.cache.get(key)

        # if got data extend ttl
        if data:
            self.cache.setex(key, self.ttl, data)
            # move to front
            self.cache.lrem("lru_order", 0, key) 
            self.cache.lpush("lru_order", key)
        
        return data
    
    def getKeys(self):
        return self.cache.lrange("lru_order", 0, -1)

    # put function for put data to cache
    def put(self, key: str, data: str):

        # check expired data
        for k in self.cache.keys("*"):
            if self.is_expired(k):
                self.cache.delete(k)
                self.cache.lrem("lru_order", 0, k)

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