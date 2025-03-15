from flask import Flask, request, jsonify
import requests

app = Flask(__name__)

# cache memory for store web content
cache = {}

# create route for proxy
@app.route('/proxy', methods=['GET'])
def proxy():
    # get url from server
    url = request.args.get("url")
    
    # check url is in cache memory
    if url in cache:
        # return data and hit status
        return jsonify({"data" : cache[url], "cache_status" : "hit"})
    else :
        # get data from server at request url
        response = requests.get(url)
        # store url to cache
        cache[url] = response.text
        return jsonify({"data" : response.text, "cache_status" : "miss"})
    
# start proxy
if __name__ == "__main__":
    app.run(debug=True)