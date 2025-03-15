import redis
import time
import json

class LFU:

    def __init__(self, size: int, ttl: int, host='localhost', port=6379, db=0):
        # connection to redis server
        self.redis = redis.Redis(host=host, port=port, db=db)
        self.size = size
        self.ttl = ttl
        # redis list for store cache and list for store frequency used order
        self.cache_key = "lfu_cache"
        self.freq_key = "lfu_freq"
    
    def is_expired(self, key: str) -> bool:

        if not self.redis.hexists(self.cache_key, key):
            return True
            
        value = self.redis.hget(self.cache_key, key)
        item = json.loads(value)
        return time.time() > item['expire_time']
    
    def get(self, key: str):
        # check if key exists and is not expired
        if not self.redis.hexists(self.cache_key, key) or self.is_expired(key):
            if self.redis.hexists(self.cache_key, key):
                # delete expired item
                self.redis.hdel(self.cache_key, key)
                self.redis.hdel(self.freq_key, key)
            return None
        
        # get the cached value
        value = self.redis.hget(self.cache_key, key)
        item = json.loads(value)
        
        # increment frequency counter
        self.redis.hincrby(self.freq_key, key, 1)
        
        return item['data']
    
    def getKeys(self):
        # get keys from cache and decode as UTF-8
        return [key.decode('utf-8') for key in self.redis.hkeys(self.cache_key)]
    
    def put(self, key: str, data: str):
        # remove expired items first
        self.remove_expired_items()
        
        # calculate expiration time
        expire_time = time.time() + self.ttl
        
        # store the data with expiration time
        item = {
            'data': data,
            'expire_time': expire_time
        }
        
        # check if cache is at capacity and needs eviction
        if (self.redis.hlen(self.cache_key) >= self.size and 
            not self.redis.hexists(self.cache_key, key)):
            self.evict_least_frequent()
        
        # store the item
        self.redis.hset(self.cache_key, key, json.dumps(item))
        
        # initialize or reset frequency counter
        self.redis.hset(self.freq_key, key, 1)

    # remove all expired items from the cache
    def remove_expired_items(self):
        current_time = time.time()
        
        # Get all keys and their values
        all_items = self.redis.hgetall(self.cache_key)
        
        # Check each item for expiration
        for key, value in all_items.items():
            item = json.loads(value)
            if current_time > item['expire_time']:
                self.redis.hdel(self.cache_key, key)
                self.redis.hdel(self.freq_key, key)
    
    # remove all least frequency data
    def evict_least_frequent(self):
        # get all frequency counts
        all_freqs = self.redis.hgetall(self.freq_key)
        if not all_freqs:
            return
        
        # convert to Python dictionary and find minimum frequency
        freq_dict = {key.decode('utf-8'): int(val) for key, val in all_freqs.items()}
        
        # find minimum frequency
        min_freq = min(freq_dict.values())
        
        # find all keys with minimum frequency
        min_freq_keys = [key for key, freq in freq_dict.items() if freq == min_freq]

        if min_freq_keys:
            # delete the first key with minimum frequency
            key_to_remove = min_freq_keys[0]
            self.redis.hdel(self.cache_key, key_to_remove)
            self.redis.hdel(self.freq_key, key_to_remove)
    
    def clear(self):
        """Clear the entire cache"""
        self.redis.delete(self.cache_key)
        self.redis.delete(self.freq_key)