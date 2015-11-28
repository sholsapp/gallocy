#!/usr/bin/env python

import click
import logging
import sys


logging.basicConfig(level=logging.INFO)
log = logging.getLogger(__name__)


@click.command()
def main():
  click.echo(
    click.style('Hello', fg='blue') +
    click.style(', ', fg='yellow') +
    click.style('World!', fg='green')
  )
