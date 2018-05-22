#!/usr/bin/python

import os

args = "A " * (1024 * 63 + 1020)

print os.system('./ex13 ' + args)
