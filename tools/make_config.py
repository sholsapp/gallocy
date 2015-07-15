#!/usr/bin/env python

"""Generate a few configs for local testing."""

import json


def main():
  nodes = 3
  for n in range(0, nodes):
    config = {
      'host': '0.0.0.0',
      'port': '8080',
      'me': '0.0.0.0:{0}'.format(8080 + n),
      'peer': [],
    }
    for peer in range(0, nodes):
      if n == peer:
        continue
      config['peer'].append('0.0.0.0:{0}'.format(8080 + peer))
    with open('gallocy-config-{0}.json'.format(n), 'w') as fh:
      json.dump(config, fh, indent=2)


if __name__ == '__main__':
  main()
