from flask import Flask, request, jsonify # type: ignore
from LRU import LRU
from LFU import LFU
import requests # type: ignore

app = Flask(__name__)

# cache memory for store web content
algorithm = input("Enter caching algorithm      "
                "\n============================="
                "\n1) LRU (Least Recently Used) "
                "\n2) LFU (Least Frequency Used)"
                "\nEnter algorithm number : ")
size = int(input("\nEnter cache size : "))

if (algorithm == "1"):
    cache = LRU(size)
else: 
    cache = LFU(size)

# create route for proxy
@app.route('/proxy', methods=['GET'])
def proxy():
    # get url from server
    url = request.args.get("url")
    
    # check url is in cache memory
    cached_data = cache.get(url)
    if cached_data:
        # return data and hit status
        return jsonify({"data" : cached_data, "cache_status" : "hit"})
    else:
        # get data from server at request url
        try:
            response = requests.get(url)
            if response.status_code == 200:
                # store url to cache
                cache.put(url, response.text)
                return jsonify({"data" : response.text, "cache_status" : "miss"})
            else:
                return jsonify({"error": "Failed to fetch data", "status": response.status_code})

        except requests.exceptions.RequestException as e:
            return jsonify({"error" : str(e)}) 
    
# start proxy
if __name__ == "__main__":
    app.run(debug=True)