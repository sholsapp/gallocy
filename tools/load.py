#!/usr/bin/env python

import requests


for i in range(0, 1000000):
  try:
    rsp = requests.get('http://localhost:8080/admin')
    rsp.raise_for_status()
    print('... {0}'.format(i))
  except:
    raise RuntimeError('Fails at iteration {0}'.format(i))
