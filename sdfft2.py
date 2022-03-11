#!/usr/bin/env python3
import numpy as np
import sdf
import os
import sys
import argparse
import sympy as sp
import matplotlib.pyplot as plt

def parselim(limstr):
	l0,l1 = limstr.split(",")
	l0 = l0.lstrip('([').lstrip()
	l1 = l1.rstrip(')]').rstrip()
	return [float(l0),float(l1)]

parser = argparse.ArgumentParser(description='create density map images')
parser.add_argument('indir',help='set input directory')
parser.add_argument('outdir',default='fft',nargs='?')
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
xlen = abs(data[0][0] - data[0][-1])
ylen = abs(data[1][0] - data[1][-1]) 
area = xlen*ylen
dim = (len(data[0])-1,len(data[1])-1)
mlen = max(dim)

ec = 0.5*8.854187e-12
bc = 0.5/1.256637e-6

if not os.path.exists(args.outdir):
	os.makedirs(args.outdir)

#fig,ax=plt.subplots(figsize=(6, 6))
#particles plot
#ax.set_xlim(0,mlen)
#ax.set_ylim(0,mlen)
#ax.set_aspect('equal')

def sdffft(file):
	print('processing '+ file)
	
	data = sdf.read(file)
	if hasattr(data,'Electric_Field_Ex') and \
	hasattr(data,'Electric_Field_Ey') and \
	hasattr(data,'Electric_Field_Ez'):
		ex = getattr(data,'Electric_Field_Ex').data
		ey = getattr(data,'Electric_Field_Ey').data
		ez = getattr(data,'Electric_Field_Ez').data
		een = area * ec*(ex*ex + ey*ey + ez*ez) / ex.size

	if hasattr(data,'Magnetic_Field_Bx') and \
	hasattr(data,'Magnetic_Field_By') and \
	hasattr(data,'Magnetic_Field_Bz'):
		bx = getattr(data,'Magnetic_Field_Bx').data
		by = getattr(data,'Magnetic_Field_By').data
		bz = getattr(data,'Magnetic_Field_Bz').data
		ben = area * bc*(bx*bx + by*by + bz*bz) / bx.size

	kenF = True
	if hasattr(data,'Derived_Number_Density'):
		nd = getattr(data,'Derived_Number_Density').data
		if hasattr(data,'Derived_EkBar'):
			ek = getattr(data,'Derived_EkBar').data
		elif hasattr(data,'Derived_Average_Particle_Energy'):
			ek = getattr(data,'Derived_Average_Particle_Energy').data
		else:
			kenF = False
	else:
		kenF = False
	
	if kenF:
		ken = area * (ek*nd) / nd.size

	ten = een + ben + ken
	tenfft = np.abs(np.fft.fftshift(np.fft.fft2(ten)))
	
	outfile = os.path.join(args.outdir,os.path.splitext(os.path.basename(file))[0]+'.png')
	plt.imsave(outfile,tenfft,cmap='gray')

for file in files:	
	sdffft(file)
