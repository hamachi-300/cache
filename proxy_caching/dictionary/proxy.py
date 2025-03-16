from flask import Flask, request, jsonify, render_template
from LRU import LRU
from LFU import LFU
import requests
import time
import os

app = Flask(__name__)

# cache status
cache_stats = {
    "hits": 0,
    "misses": 0,
    "requests": 0,
    "hit_ratio": 0,
    "history": [],  # List of (timestamp, hit/miss, url)
}

# update stats function
def update_stats(hit, url):
    cache_stats["requests"] += 1
    if hit:
        cache_stats["hits"] += 1
    else:
        cache_stats["misses"] += 1
    
    cache_stats["hit_ratio"] = (cache_stats["hits"] / cache_stats["requests"]) * 100 if cache_stats["requests"] > 0 else 0
    
    # add to history (keep last 100 entries)
    timestamp = time.strftime("%H:%M:%S", time.localtime())
    status = "hit" if hit else "miss"
    cache_stats["history"].append((timestamp, status, url))
    if len(cache_stats["history"]) > 100:
        cache_stats["history"].pop(0)
    

# create HTML template directory
os.makedirs("templates", exist_ok=True)

# create dashboard template
with open("templates/dashboard.html", "w") as f:
    f.write("""
<!DOCTYPE html>
<html>
<head>
    <title>Cache Proxy Dashboard</title>
    <meta http-equiv="refresh" content="5">
    <style>
        body { font-family: monospace, sans-serif; margin: 20px; background-color: #2C3333; color: #DDD}
        .stats-container { display: flex; flex-wrap: wrap; }
        .stat-box { 
            border: 1px solid #ccc; 
            border-radius: 5px; 
            padding: 15px; 
            margin: 10px; 
            text-align: center; 
            min-width: 150px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        .hit { background-color: #395B64; }
        .miss { background-color: #992D2D; }
        .neutral { background-color: #2C3333; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
        th { background-color: #232828; }
        tr.hit-row { background-color: #395B64; }
        tr.miss-row { background-color: #992D2D; }
        .cache-content { margin-top: 20px }
        h2 { color: #DDD; border-bottom: 1px solid #eee; padding-bottom: 5px; }
        .keys-box {
            border: 1px solid #ccc;
            border-radius: 5px;
            padding: 15px;
            margin: 20px 0;
            background-color: #232828;
            max-height: 200px;
            overflow-y: auto;
        }
        .key-item {
            display: inline-block;
            background-color: #3c4242;
            border-radius: 3px;
            padding: 5px 10px;
            margin: 3px;
            font-size: 14px;
            box-shadow: 0 1px 2px rgba(0,0,0,0.1);
        }
    </style>
</head>
<body>
    <h1>Cache Proxy Dashboard</h1>
    
    <h2>Cache Statistics</h2>
    <div class="stats-container">
        <div class="stat-box neutral">
            <h3>Total Requests</h3>
            <p>{{ stats.requests }}</p>
        </div>
        <div class="stat-box hit">
            <h3>Cache Hits</h3>
            <p>{{ stats.hits }}</p>
        </div>
        <div class="stat-box miss">
            <h3>Cache Misses</h3>
            <p>{{ stats.misses }}</p>
        </div>
        <div class="stat-box neutral">
            <h3>Hit Ratio</h3>
            <p>{{ "%.2f"|format(stats.hit_ratio) }}%</p>
        </div>
    </div>
    
    <h2>Current Cache Keys ({{ keys|length }})</h2>
    <div class="keys-box">
        {% if keys %}
            {% for key in keys %}
                <div class="key-item">{{ key }}</div>
            {% endfor %}
        {% else %}
            <p>No keys in cache</p>
        {% endif %}
    </div>
    
    <h2>Cache Content Details ({{ cached_items|length }} items)</h2>
    <table>
        <tr>
            <th>Key</th>
            <th>Size (bytes)</th>
            <th>Expires In</th>
        </tr>
        {% for key, size, expiry in cached_items %}
        <tr>
            <td>{{ key }}</td>
            <td>{{ size }}</td>
            <td>{{ expiry }}</td>
        </tr>
        {% endfor %}
    </table>
    
    <h2>Recent Requests</h2>
    <table>
        <tr>
            <th>Time</th>
            <th>Status</th>
            <th>URL</th>
        </tr>
        {% for time, status, url in stats.history|reverse %}
        <tr class="{{ status }}-row ">
            <td>{{ time }}</td>
            <td>{{ status.upper() }}</td>
            <td>{{ url }}</td>
        </tr>
        {% endfor %}
    </table>
</body>
</html>
    """)

# get information about cached items
def get_cache_items():
    cached_items = []
    now = time.time()
    
    # check cache is class
    if isinstance(cache, LRU) or isinstance(cache, LFU):
        for key in cache.getKeys():
            if key in cache.cache:
                if isinstance(cache, LRU):
                    data, expire_time = cache.cache[key]
                else:  # LFU
                    data, expire_time = cache.cache[key]
                
                size = len(data) if data else 0
                expires_in = int(expire_time - now)
                if expires_in < 0:
                    expires_in = "Expired"
                else:
                    expires_in = f"{expires_in} sec"
                
                cached_items.append((key, size, expires_in))
    
    return cached_items

# route for dashboard
@app.route('/dashboard')
def dashboard():
    cached_items = get_cache_items()
    cache_keys = cache.getKeys()
    # return html page
    return render_template('dashboard.html', stats=cache_stats, cached_items=cached_items, keys=cache_keys)

# get cache algorithm from user
algorithm = input("Enter caching algorithm      "
                "\n============================="
                "\n1) LRU (Least Recently Used) "
                "\n2) LFU (Least Frequency Used)"
                "\nEnter algorithm number : ")
size = int(input("\nEnter cache size : "))
expire_ttl = int(input("Enter Expire Time (second) : "))

if (algorithm == "1"):
    cache = LRU(size, expire_ttl)
    cache_type = "LRU"
else: 
    cache = LFU(size, expire_ttl)
    cache_type = "LFU"

# route for proxy
@app.route('/proxy', methods=['GET'])
def proxy():
    url = request.args.get("url")
    if not url:
        return jsonify({"error": "URL parameter is required"}), 400
    
    print(f"Checking cache for URL: {url}")
    print(f"Current cache keys: {cache.getKeys()}")
    
    # check if URL is in cache
    cached_data = cache.get(url)
    if cached_data:
        update_stats(True, url)
        return jsonify({"data": cached_data, "cache_status": "hit"})
    else:
        # Get data from server
        try:
            response = requests.get(url)
            if response.status_code == 200:
                cache.put(url, response.text)
                update_stats(False, url)
                return jsonify({"data": response.text, "cache_status": "miss"})
            else:
                return jsonify({"error": "Failed to fetch data", "status": response.status_code})
        except requests.exceptions.RequestException as e:
            return jsonify({"error": str(e)})

# start server
if __name__ == "__main__":
    print(f"\nProxy server started with {cache_type} caching")
    print(f"Access the dashboard at: http://localhost:5000/dashboard")
    print(f"Use the proxy at: http://localhost:5000/proxy?url=YOUR_URL")
    app.run(debug=False)