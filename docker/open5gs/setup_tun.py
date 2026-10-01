#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

import click
import ipaddress
import subprocess
from pyroute2 import IPRoute
from pyroute2.netlink import NetlinkError


def handle_ip_string(ctx, param, value):
    try:
        ret = ipaddress.ip_network(value)
        return ret
    except ValueError:
        raise click.BadParameter(f'{value} is not a valid IP range.')


def nft_add_masquerade(if_name, ip_range):
    subprocess.run(["nft", "add", "table", "ip", "nat"], check=True)
    subprocess.run(["nft", "add", "chain", "ip", "nat", "POSTROUTING",
                     "{", "type", "nat", "hook", "postrouting", "priority", "100", ";", "}"], check=True)
    subprocess.run(["nft", "add", "rule", "ip", "nat", "POSTROUTING",
                     "ip", "saddr", ip_range, "oifname", if_name, "masquerade"], check=True)


def nft_allow_all(if_name):
    subprocess.run(["nft", "add", "table", "inet", "filter"], check=True)
    subprocess.run(["nft", "add", "chain", "inet", "filter", "INPUT",
                     "{", "type", "filter", "hook", "input", "priority", "0", ";", "}"], check=True)
    subprocess.run(["nft", "add", "rule", "inet", "filter", "INPUT",
                     "iifname", if_name, "accept"], check=True)


@click.command()
@click.option("--if_name", default="ogstun", help="TUN interface name.")
@click.option("--ip_range", default='10.45.0.0/24', callback=handle_ip_string,
              help="IP range of the TUN interface.")
def main(if_name, ip_range):

    for subnet in range(0,256):
        # Get the first IP address in the IP range and netmask prefix length
        first_ip_addr = next(ip_range.hosts(), None) + (subnet * 256)
        if not first_ip_addr:
            raise ValueError('Invalid IP range.')
        else:
            first_ip_addr = first_ip_addr.exploded

        ip_netmask = ip_range.prefixlen

        ipr = IPRoute()
        # create the tun interface
        ipr.link('add', ifname=if_name, kind='tuntap', mode='tun')
        # lookup the index
        dev = ipr.link_lookup(ifname=if_name)[0]
        # bring it down
        ipr.link('set', index=dev, state='down')
        # add primary IP address
        ipr.addr('add', index=dev, address=first_ip_addr, mask=ip_netmask)
        # bring it up
        ipr.link('set', index=dev, state='up')

        try:
            ipr.route('add', dst=ip_range.with_prefixlen, gateway=first_ip_addr)
        except NetlinkError:
            pass

        # setup nftables
        nft_add_masquerade(if_name, ip_range.with_prefixlen)
        nft_allow_all(if_name)


if __name__ == "__main__":
    main()