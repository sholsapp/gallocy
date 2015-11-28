# py509

[![Latest Version](https://img.shields.io/pypi/v/py509.svg)](https://pypi.python.org/pypi/py509/)
[![Documentation Status](https://readthedocs.org/projects/py509/badge/?version=latest)](http://py509.readthedocs.org/en/latest/)
[![Travis](https://secure.travis-ci.org/sholsapp/py509.png?branch=master)](https://travis-ci.org/sholsapp/py509)

Framework and scripts written with pyOpenSSL and cryptography for running
public key infrastructure.

## packaging

Consider using [pex](https://pex.readthedocs.org/en/latest/index.html) to
package the scripts provided by this library to make them relocatable and
installable on machines.

```bash
mkdir ~/wheel-cache
pip wheel -w ~/wheel-cache .
pex -r py509 --no-pypi --repo=~/wheel-cache -o pyssl-get -e py509.bin.get:main
```

This process is automated using a custom `setuptools.Command` in this project
called `pexify`. To run the above code for each entry point specified in
`setup.py` just run the following.

```bash
python setup.py pexify
```

## docs

```bash
sphinx-build -W -b html docs docs/_build/html
```
