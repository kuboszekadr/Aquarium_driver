import requests
import random

from time import sleep

from datetime import datetime as dt

url = 'http://192.168.0.179/aquarium_post.php'

user = 'aquarium_arduino'
pwd = 'test'

temp = lambda: random.uniform(20, 30)
ph = lambda: random.uniform(5, 7.1)
water_level = lambda: random.uniform(0, 10)

i = 0
while i < 1000:
    dt = dt.now()
    post_data = {'temp': temp(), 'ph': ph(), 'water_level_sump': water_level(), 'date': dt, 'user': user, 'pwd': pwd}
    r = requests.post(url, post_data)
    
    print (r.status_code)
    print(i)
    
    i += 1
    sleep(1)