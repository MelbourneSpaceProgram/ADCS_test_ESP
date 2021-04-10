import requests
import numpy as np
import sseclient
import json

ESP_IP='http://192.168.0.181/events'
#'192.168.0.1'

x = requests.get(ESP_IP, stream=True)
print(x.status_code)

client = sseclient.SSEClient(x)
for event in client.events():
    print(event.data)