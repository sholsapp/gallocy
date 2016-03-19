#!/usr/bin/env python

"""Tests that a leader is elected.

After several nodes are started, ensure that a leader is elected.

"""

import requests
import subprocess
import sys


def load_cthulhu_state():
    """Load cthulhu state from fixture.

    This method reads in the cthulhu state (i.e., how many nodes, node short
    names, etc.) from the fixture on disk. This state is useful for inspecting
    or manipulating the docker containers.

    """
    # TODO(sholsapp): How can we provide this information as an input so that
    # we don't make assumptions about the cthulhu fixture?
    return {
        'i001': 'localhost:30000',
        'i002': 'localhost:30001',
        'i003': 'localhost:30002',
        'i004': 'localhost:30003',
        'i005': 'localhost:30004',
    }


def main():
    state = load_cthulhu_state()
    responses = {}
    for name, host in state.iteritems():
        responses[name] = requests.get('http://{host}/admin'.format(host=host))

    leaders = []
    for name, rsp in responses.iteritems():
        assert rsp.ok, "Responses should always return successfully."
        assert rsp.headers['Content-Type'] == 'application/json', "Responses should always be application/json."
        if rsp.json()['state'] == 'LEADER':
            leaders.append(name)
    assert len(leaders) == 1, "There should be one and only one leader."
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except Exception as e:
        print(str(e))
        sys.exit(1)
