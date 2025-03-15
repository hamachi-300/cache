import redis
import time
import json

class LRU:
    def __init__(self, size: int, ttl: int, host='localhost', port=6379, db=0):
        # connection to redis server
        self.redis = redis.Redis(host=host, port=port, db=db)
        self.size = size
        self.ttl = ttl
        # redis list for store cache and list for store recently order
        self.cache_key = "lru_cache"
        self.order_key = "lru_order"
    
    def get(self, key: str):
        
        # check if key exists in cache
        if not self.redis.hexists(self.cache_key, key):
            return None
        
        # get the cached value
        value = self.redis.hget(self.cache_key, key)
        item = json.loads(value)
        
        # check if item is expired
        if time.time() > item['expire_time']:
            # remove expired item
            self.redis.hdel(self.cache_key, key)
            self.redis.lrem(self.order_key, 1, key)
            return None
        
        # update the LRU order (move to front)
        self.redis.lrem(self.order_key, 1, key) # remove 
        self.redis.lpush(self.order_key, key) # push left
        
        return item['data']
    
    def getKeys(self):
        # get keys in LRU order
        return self.redis.lrange(self.order_key, 0, -1)
    
    def put(self, key: str, data: str):
        # check if key already exists
        if self.redis.hexists(self.cache_key, key):
            # update LRU order
            self.redis.lrem(self.order_key, 1, key)
        
        # calculate expiration time
        expire_time = time.time() + self.ttl
        
        # store the data with expiration time
        item = {
            'data': data,
            'expire_time': expire_time
        }
        self.redis.hset(self.cache_key, key, json.dumps(item))
        self.redis.lpush(self.order_key, key)
        
        # Remove expired items
        self.remove_expired_items()
        
        # If cache exceeds size, remove least recently used items
        self.enforce_size_limit()
    
    # remove all expired items from the cache
    def remove_expired_items(self):
        current_time = time.time()
        
        # get all keys and their values
        all_items = self.redis.hgetall(self.cache_key)
        
        # check each item for expiration
        for key, value in all_items.items():
            item = json.loads(value)
            if current_time > item['expire_time']:
                # delete from cache list and order list
                self.redis.hdel(self.cache_key, key)
                self.redis.lrem(self.order_key, 1, key)
    
    # check size is exceed
    def enforce_size_limit(self):

        # get current cache size
        cache_size = self.redis.hlen(self.cache_key)
        
        # remove least recently used items if cache exceeds size
        if cache_size > self.size:
            # get items to remove (from the end of the list)
            items_to_remove = cache_size - self.size
            for _ in range(items_to_remove):
                # get the least recently used key (right zone)
                oldest_key = self.redis.rpop(self.order_key)
                if oldest_key:
                    # delete from cache
                    self.redis.hdel(self.cache_key, oldest_key)
    
    def clear(self):
        # clear entire cache
        self.redis.delete(self.cache_key)
        self.redis.delete(self.order_key)