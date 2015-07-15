#!/usr/bin/env python

"""Generate a .tar.gz that can be deployed."""


import contextlib
import os
import platform
import tarfile


@contextlib.contextmanager
def chdir(dirname=None):
  curdir = os.getcwd()
  try:
    if dirname is not None:
      os.chdir(dirname)
    yield
  finally:
    os.chdir(curdir)


def package_name(version):
  return 'gallocy-{0}-{1}-{2}.tar.gz'.format(
    version,
    platform.system().lower(),
    platform.machine().lower(),
  )

def main():

  version = '0.0.0'
  with open('VERSION') as fh:
    version = fh.read().strip()

  tar = tarfile.open(package_name(version), 'w:gz')

  with chdir('install'):
    tar.add('bin')
    tar.add('lib')
    tar.close()

if __name__ == '__main__':
  main()
