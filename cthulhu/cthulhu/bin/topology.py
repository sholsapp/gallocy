#!/usr/bin/env python

"""Create a distributed test fixture for use on a Unix-like system.

This tool creates a file system on disk that can be used to start n-many docker
containers. Each docker container started using the control script will
automatically have a configuration and log bind mount created and linked to the
respective container.

It is said that this tool creates a distributed test *fixture* because no
assertions are codified or made by this tool or its generated files. Instead,
only set up and tear down states are provided by this tool. All assertions
should be codified and made by an external driver like bats.

"""

import click
import json
import logging
import os
import shutil
import sys

from netaddr import IPAddress, IPNetwork, IPRange
from jinja2 import Template
import netifaces


logging.basicConfig(level=logging.INFO)
log = logging.getLogger(__name__)


class FixtureContext(object):
  """A fixture context.

  A fixture context is a distributed control framework codified onto the file
  system and runnable through docker.

  """
  def __init__(self, root):
    self.instances = []
    self.fixture_root = os.path.join(root, 'cthulhu-fixture')
    self.fixture_control = os.path.join(self.fixture_root, 'control')
    os.makedirs(self.fixture_root)

  def write_file(self, path, contents, perm=0o644):
    with open(path, 'w') as fh:
      fh.write(contents)
    os.chmod(path, perm)

  def render(self):
    # Render all instances
    for instance in self.instances:
      instance.render()
    # Render human readable links to each instance
    for instance in self.instances:
      try:
        os.symlink(instance.node_root, os.path.join(self.fixture_root, instance.instance_name))
      except Exception:
        log.exception('Failed to create symlink to %s', instance.instance_name)
    # Render the master control script
    self.write_file(self.fixture_control, self.render_control(), perm=0o755)

  def render_control(self):
    return Template("""#!/bin/bash

# GENERATED CONTROL SCRIPT

sub_help() {
  echo "Usage: control <subcommand> [options]"
  echo "Subcommands:"
  echo "    start"
  echo "    stop"
}

sub_start() {
{% for instance in instances %}
  {{ instance.node_control }}
{% endfor %}
}

sub_stop() {
  RUNNING_CONTAINERS=$(docker ps -q)
  if [ "$RUNNING_CONTAINERS" != "" ]; then
    OUT="$(docker stop -t 1 $RUNNING_CONTAINERS)"
  fi

  OLD_CONTAINERS="$(docker ps -a -q)"
  if [ "$OLD_CONTAINERS" != "" ]; then
    OUT="$(docker rm $OLD_CONTAINERS)"
  fi
}

SUBCOMMAND=$1
case $SUBCOMMAND in
  "" | "-h" | "--help")
    sub_help
    ;;
  *)
    shift
    sub_${SUBCOMMAND} $@
    if [ $? = 127 ]; then
      echo "Error: '$SUBCOMMAND' is not a known subcommand." >&2
      exit 1
    fi
    ;;
esac
""").render(
    instances=self.instances,
  )



class InstanceContext(object):
  """An instance context.

  An instance context is a part larger fixture and captures context specific
  to this instance, e.g., the instance's file system root, configuration
  directory, log file, and more.

  """
  def __init__(self, root, instance, network, container):
    self.instance = instance
    self.network = list(network)
    self.container = container
    self.node_root = os.path.join(root, str(self.network[self.instance]))
    self.node_bindmounts = os.path.join(self.node_root, 'bindmounts')
    self.node_etc = os.path.join(self.node_bindmounts, 'etc')
    self.node_var = os.path.join(self.node_bindmounts, 'var')

    os.makedirs(self.node_root)
    os.makedirs(self.node_bindmounts)
    os.makedirs(self.node_etc)
    os.makedirs(self.node_var)

    self.node_config = os.path.join(self.node_etc, 'config.json')
    self.node_control = os.path.join(self.node_root, 'control')
    self.node_log = os.path.join(self.node_var, '{0}.log'.format(self.instance_name))

  def write_file(self, path, contents, perm=0o644):
    with open(path, 'w') as fh:
      fh.write(contents)
    os.chmod(path, perm)

  @property
  def instance_name(self):
    return 'i{0}'.format(str(self.instance + 1).zfill(3))

  def render(self):
    self.write_file(self.node_config,
                    self.render_config())
    self.write_file(self.node_control,
                    self.render_control(),
                    perm=0o755)
    self.write_file(self.node_log, '')

  def render_config(self):
    # TODO(sholsapp): How can we make this generic but meaningful? We always will
    # need to know a little bit about the application here, so maybe we should
    # ask for a fixture.spec or something?
    ip = self.network[self.instance]
    return json.dumps({
      'self': str(ip),
      'port': 8080,
      'master': True if ip == self.network[0] else False,
      'peers': [str(n) for n in self.network if n != ip]
      ,
    }, indent=2)

  def render_control(self):
    host_port = 30000 + self.instance
    host_name = self.instance_name
    # TODO(sholsapp): How can we make this generic but meaningful? We always will
    # need to know a little bit about the application here, so maybe we should
    # ask for a fixture.spec or something?
    return Template('''#!/bin/bash -x

# GENERATED CONTROL SCRIPT

PID=$(docker run \\
  --privileged=true \\
  --name {{ name }} \\
  --interactive \\
  --tty \\
  --detach \\
  --publish={{ host_port }}:8080 \\
  --hostname={{ host_name }} \\
  --volume={{ local_etc }}:/home/gallocy/etc \\
  --volume={{ local_var }}:/home/gallocy/var \\
  --memory=16M \\
  {{ docker_container }})
echo $PID > {{ local_root }}/pid.txt


''').render(
    docker_container=self.container,
    host_name=host_name,
    host_port=host_port,
    local_etc=self.node_etc,
    local_root=self.node_root,
    local_var=self.node_var,
    name=self.instance_name,
  )


@click.command(help=__doc__, epilog='Currently, only usable within the context of gallocy.')
@click.option('--instances', default=1,
              help='The number of instances to create skeletons for.')
@click.option('--docker-bridge', default='docker0',
              help='The docker bridge network within which to create the distributed test fixture.')
@click.option('--docker-container', default='gallocy-example',
              help='The docker container to wire into the distributed test fixture.')
@click.option('--fixture-root', default=os.getcwd(),
              help='Path to file system root directory at which to create the fixture.')
def main(instances, docker_bridge, docker_container, fixture_root):

  try:
    addrs = netifaces.ifaddresses(docker_bridge)
  except ValueError:
    click.secho('It appears {0} is not a valid newtork interface on this system.'.format(docker_bridge), fg='red')
    sys.exit(1)

  try:
    docker_bridge_addr = IPAddress(addrs[netifaces.AF_INET][0]['addr'])
  except IndexError:
    click.secho('It appears {0} does not have an address at this time.'.format(docker_bridge), fg='red')
    sys.exit(1)

  network = list(IPRange(docker_bridge_addr + 1,
                         docker_bridge_addr + 1 + instances - 1))

  fixture_ctx = FixtureContext(fixture_root)
  for instance in range(0, len(network)):
    # TODO(sholsapp): We might want to specify different containers for
    # different instances one day.
    instance_ctx = InstanceContext(fixture_ctx.fixture_root, instance, network, docker_container)
    click.secho('Creating instance {0} at {1}... '.format(instance_ctx.instance, instance_ctx.node_root), nl=False)
    fixture_ctx.instances.append(instance_ctx)
    click.secho('[GOOD]', fg='green')
  fixture_ctx.render()
