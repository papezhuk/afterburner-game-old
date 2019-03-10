#! /usr/bin/env python

from __future__ import print_function
from waflib import Logs
import sys
import os

# TODO: Make this better
sys.path.append(os.path.realpath('dependencies/afterburner-engine/scripts/waflib'))
# print(sys.path)
import fwgslib

APPNAME = 'bond'
top = '.'

SUBDIRS = \
[
	"dependencies/afterburner-engine"
]

def options(opt):
	for path in SUBDIRS:
		opt.recurse(path)

def configure(conf):
	for path in SUBDIRS:
		conf.setenv(path, conf.env) # derive new env from global one
		conf.env.ENVNAME = path
		conf.msg(msg='--> ' + path, result='in progress', color='BLUE')
		# configure in standalone env
		conf.recurse(path)
		conf.msg(msg='<-- ' + path, result='done', color='BLUE')
		conf.setenv('')

def build(bld):
	for path in SUBDIRS:
		bld.recurse(path)
