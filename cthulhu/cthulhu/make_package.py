#!/usr/bin/env python

"""Generate a .tar.gz that can be deployed."""


import contextlib
import os
import platform
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

  gallocy_root = os.getcwd()

  with mkdtmp() as tmp:

    shutil.copytree(os.path.join(gallocy_root, 'install', 'bin'), os.path.join(tmp, 'bin'))
    shutil.copytree(os.path.join(gallocy_root, 'install', 'lib'), os.path.join(tmp, 'lib'))

    with open(os.path.join(tmp, 'run'), 'w') as fh:
      fh.writelines([
        '#!/bin/bash\n',
        'LD_LIBRARY_PATH=lib   \\ \n',
        'DYLD_LIBRARY_PATH=lib \\ \n',
        'GALLOCY_CONFIG=config.json ./bin/httpd\n'
      ])
      os.fchmod(fh.fileno(), 0755)

    with chdir(tmp):
      tar.add('bin')
      tar.add('lib')
      tar.add('run')

  tar.close()

if __name__ == '__main__':
  main()
