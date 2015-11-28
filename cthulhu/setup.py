#!/usr/bin/env python

import os
import subprocess

from setuptools import setup, find_packages, Command


entry_points = {
  'console_scripts': [
    'hello = cthulhu.bin.hello:main',
    'topology = cthulhu.bin.topology:main',
  ],
}


class Pex(Command):

  user_options = []

  def initialize_options(self):
    """Abstract method that is required to be overwritten"""

  def finalize_options(self):
    """Abstract method that is required to be overwritten"""

  def run(self):
    if not os.path.exists('dist/wheel-cache'):
      print('You need to create dist/wheel-cache first! You\'ll need to run the following.')
      print('  mkdir dist/wheel-cache')
      print('  pip wheel -w dist/wheel-cache')
      return
    for entry in entry_points['console_scripts']:
      name, call = tuple([_.strip() for _ in entry.split('=')])
      print('Creating {0} as {1}'.format(name, call))
      pex_cmd = [
        'pex',
        '--no-pypi',
        '--repo=dist/wheel-cache',
        '-o', 'build/bin/{0}'.format(name),
        '-e', call,
        '.',
      ]
      print('Running {0}'.format(' '.join(pex_cmd)))
      subprocess.check_call(pex_cmd)


setup(
  name='cthulhu',
  version='0.0.1',
  description="A distributed systems testing framework.",
  author='Stephen Holsapple',
  author_email='sholsapp@gmail.com',
  url='https://github.com/sholsapp/gallocy',
  license='Apache License, Version 2.0',
  packages=find_packages(),
  install_requires=[
    # For compatibility with py2.6
    'argparse',
    'click',
    'jinja2',
    'netaddr',
    'netifaces',
    'pex',
    'requests',
    'tabulate',
    'wheel',
  ],
  tests_require=[
    'flake8',
    'pytest',
    'pytest-cov',
  ],
  entry_points=entry_points,
  cmdclass={'pexify': Pex},
  classifiers=[
    'License :: OSI Approved :: Apache Software License',
    'Programming Language :: Python :: 2',
    'Programming Language :: Python :: 2.6',
    'Programming Language :: Python :: 2.7',
    'Programming Language :: Python :: 3',
    'Programming Language :: Python :: 3.3',
    'Programming Language :: Python :: Implementation :: CPython',
    'Programming Language :: Python :: Implementation :: PyPy',
    'Topic :: Security :: Cryptography',
    'Topic :: Software Development :: Libraries :: Python Modules',
  ],
)
