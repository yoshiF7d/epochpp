#!/usr/bin/env python
import numpy as np
import sdf
import os
import sys
import argparse
import re
import contextlib

def parselim(limstr):
	l0,l1 = limstr.split(",")
	l0 = l0.lstrip('([').lstrip()
	l1 = l1.rstrip(')]').rstrip()
	return [float(l0),float(l1)]

parser = argparse.ArgumentParser(description='create density map images')
parser.add_argument('indir',help='set input directory')
parser.add_argument('outfile',help='set output file',nargs='?')
parser.add_argument('-f','--flim',type=str,help='process from fmin th file to fmax th file (index starts from 0). usage : -f [fmin,fmax]',metavar='[fmin,fmax]')

args=parser.parse_args()

files = [file for file in os.listdir(args.indir) if file.endswith('.sdf')]
files.sort()

if args.flim is not None:
	args.flim = list(map(int,parselim(args.flim)))
	files = files[args.flim[0]:args.flim[1]]
else:
	args.flim = [0,len(files)-1]
	
data = sdf.read(files[0]).Grid_Grid.data
area = abs((data[0][0] - data[0][-1]) * (data[1][0] - data[1][-1]) ) 

output = args.output and open(args.output, 'w') or sys.stdout
output.write("filename\telectric energy [J]\tmagnetic energy [J]\tkinetic energy [J]\n")

ec = 0.5*8.854187e-12
bc = 0.5/1.256637e-6

def sdfen(file):
	print('processing '+ file)
	
	if hasattr(data,'Electric_Field_Ex') and \
	hasattr(data,'Electric_Field_Ey') and \
	hasattr(data,'Electric_Field_Ez'):
		eenF = true
		ex = np.array(getattr(data,'Electric_Field_Ex').data)
		ey = np.array(getattr(data,'Electric_Field_Ey').data)
		ez = np.array(getattr(data,'Electric_Field_Ez').data)
		een = area * ec*(ex*ex + ey*ey + ez*ez).sum() / ex.size
	else:
		eenF = false

	if hasattr(data,'Magnetic_Field_Bx') and \
	hasattr(data,'Magnetic_Field_By') and \
	hasattr(data,'Magnetic_Field_Bz'):
		benF = true
		bx = np.array(getattr(data,'Magnetic_Field_Bx').data)
		by = np.array(getattr(data,'Magnetic_Field_By').data)
		bz = np.array(getattr(data,'Magnetic_Field_Bz').data)
		ben = area * bc*(bx*bx + by*by + bz*bz).sum() / bx.size
	else:
		benF = false

	kenF = true
	if hasattr(data,'Derived_Number_Density'):
		nd = np.array(getattr(data,'Derived_Number_Density').data)
		if hasattr(data,'Derived_EkBar'):
			ek = np.array(getattr(data,'Derived_EkBar').data)
		elif hasattr(data,'Derived_Average_Particle_Energy'):
			ek = np.array(getattr(data,'Derived_Average_Particle_Energy').data)
		else:
			kenF = false
	else:
		kenF = false
	
	if kenF:
		ken = area * (ek*nd).sum() / nd.size
	
	output.write(f'{file}\t{een}\t{ben}\t{ken}')
	
for file in files:	
	sdfen(file)

if output is not sys.stdout:
	output.close()
