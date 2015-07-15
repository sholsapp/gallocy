#!/usr/bin/env python

import contextlib
import json
import os
import platform
import sys
import tarfile
import tempfile
import shutil


@contextlib.contextmanager
def chdir(dirname=None):
  curdir = os.getcwd()
  try:
    if dirname is not None:
      os.chdir(dirname)
    yield
  finally:
    os.chdir(curdir)


@contextlib.contextmanager
def mkdtmp(suffix='', prefix='', dir=None):
  tmp = None
  try:
    tmp = tempfile.mkdtemp(suffix=suffix, prefix=prefix, dir=dir)
    yield tmp
  finally:
    if tmp:
      shutil.rmtree(tmp)


def main():
  nodes = 3
  deployment_dir = '/tmp/gallocy'
  distribution = os.path.abspath(sys.argv[1])

  try:
    shutil.rmtree(deployment_dir)
  except:
    pass

  try:
    os.makedirs(deployment_dir)
  except:
    pass

  with mkdtmp() as tmp:

    os.chdir(tmp)

    configs = os.path.join(tmp, 'configs')
    os.makedirs(configs)
    os.chdir(configs)
    for n in range(1, nodes + 1):
      config = {
        'host': '0.0.0.0',
        'port': str(8080 + n),
        'me': '0.0.0.0:{0}'.format(8080 + n),
        'peer': [],
      }
      for peer in range(0, nodes):
        if n == peer:
          continue
        config['peer'].append('0.0.0.0:{0}'.format(8080 + peer))
      with open('gallocy-config-{0}.json'.format(n), 'w') as fh:
        json.dump(config, fh, indent=2)


    for i in range(1, nodes + 1):

      idir = os.path.join(deployment_dir, 'i{0}'.format(i))
      os.makedirs(idir)

      os.chdir(idir)

      tar = tarfile.open(distribution)
      tar.extractall()
      tar.close()

      shutil.copy(os.path.join(configs, 'gallocy-config-{0}.json'.format(i)), os.path.join(idir, 'config.json'))



if __name__ == '__main__':
  main()
