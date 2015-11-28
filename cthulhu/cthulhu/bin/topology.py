#!/usr/bin/env python

import click
import logging
import os
import sys

from netaddr import IPAddress, IPNetwork


logging.basicConfig(level=logging.INFO)
log = logging.getLogger(__name__)


def write_file(path, contents, perm=0o644):
  with open(path, 'w') as fh:
    fh.write(contents)
  os.chmod(path, perm)


@click.command()
@click.option('--network', default='10.0.0.0/32',
              help='Network in CIDR format from which to create topology.')
@click.option('--fs-root', default='/tmp',
              help='Path to file system root directory at which to create topology skeleton.')
def main(network, fs_root):

  fixture_root = os.path.join(fs_root, 'cthulu-fixture')

  os.makedirs(fixture_root)

  network = IPNetwork(network)

  for n in network:
    node_root = os.path.join(fixture_root, str(n))
    node_logs = os.path.join(node_root, 'logs')
    os.makedirs(node_root)
    os.makedirs(node_logs)

    node_config = os.path.join(node_root, 'config')
    node_control = os.path.join(node_root, 'control')
    node_log = os.path.join(node_logs, 'log.txt')

    write_file(node_config, 'config')
    write_file(node_control, 'control', perm=0o755)
    write_file(node_log, 'log')
