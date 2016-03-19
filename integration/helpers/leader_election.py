#!/usr/bin/env python

"""Tests that a leader is elected.

After several nodes are started, ensure that a leader is elected.

"""

import requests
import subprocess
import sys
import time


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


def get_leader(state):
    responses = {}
    for name, host in state.iteritems():
        responses[name] = requests.get('http://{host}/admin'.format(host=host))

    leaders = []
    for name, rsp in responses.iteritems():
        if rsp.json()['state'] == 'LEADER':
            leaders.append(name)
    assert len(leaders) == 1, "There should be one and only one leader, but found {0} leaders: {1}.".format(len(leaders, ', '.join(leaders)))

    return leaders[0]


def main():
    state = load_cthulhu_state()

    leader0 = get_leader(state)

    time.sleep(10)
    rsp = requests.get('http://{host}/admin'.format(host=state[leader0]))
    assert rsp.json()['state'] == 'LEADER', "The leader {0} is not stable after 10 seconds.".format(leader0)

    subprocess.check_call(['cthulhu-fixture/{0}/control'.format(leader0), 'stop'])
    time.sleep(5)
    try:
        rsp = requests.get('http://{host}/admin'.format(host=state[leader0]))
        assert False, "The leader {0} did not stop!".format(leader0)
    except requests.ConnectionError:
        del state[leader0]

    leader1 = get_leader(state)

    time.sleep(10)
    rsp = requests.get('http://{host}/admin'.format(host=state[leader1]))
    assert rsp.json()['state'] == 'LEADER', "The leader {0} is not stable after 10 seconds.".format(leader1)
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except Exception as e:
        print(str(e))
        sys.exit(1)
