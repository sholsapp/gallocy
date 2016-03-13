#!/usr/bin/env python

"""Tests that a leader is elected.

After several nodes are started, ensure that a leader is elected.

"""

import requests


def main():
    responses = []
    # TODO(sholsapp): How can we provide this information as an input so that
    # we don't make assumptions about the cthulhu fixture?
    for node in ['localhost:30000', 'localhost:30001', 'localhost:30002', 'localhost:30003', 'localhost:30004']:
        responses.append(requests.get('http://{node}/admin'.format(node=node)))

    leaders = []
    for rsp in responses:
        assert rsp.ok
        assert rsp.headers['Content-Type'] == 'application/json'
        if rsp.json()['state'] == 'LEADER':
            leaders.append(True)
    assert len(leaders) == 1


if __name__ == '__main__':
    main()
