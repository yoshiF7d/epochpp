#!/usr/bin/env python
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
import numpy as np
import sdf_helper as sh
import os
import sys
import argparse
import re
import contextlib
from collections import defaultdict

class DummyFile(object):
	def write(self,x): pass
@contextlib.contextmanager
def nostdout() :
	save_stdout = sys.stdout
	sys.stdout = DummyFile()
	yield
	sys.stdout = save_stdout

C_c=2.997925e+08
C_me=9.109383e-31
C_q=1.60217657e-19
tiny=1e-8

parser = argparse.ArgumentParser(description='print laser absorption on boundaries')
parser.add_argument('indir',help='input directory')
parser.add_argument('outfile',nargs='?',type=argparse.FileType('w'),default=sys.stdout,help='output file')
args=parser.parse_args()


files = [file for file in os.listdir(args.indir) if file.endswith('.sdf')]
files.sort()

args.outfile.write(' '*len(os.path.basename(files[0])) + '\t' + 'Absorption_Total_Laser_Energy_Injected__J_' + '\t' + 'Absorption_Fraction_of_Laser_Energy_Absorbed____' + '\n')

for file in files:
	with nostdout():
		data=sh.getdata(file)
	args.outfile.write(os.path.basename(file) + '\t' + str(data.Absorption_Total_Laser_Energy_Injected__J_.data) + '\t' + str(data.Absorption_Fraction_of_Laser_Energy_Absorbed____.data) + '\n')

args.outfile.close()
