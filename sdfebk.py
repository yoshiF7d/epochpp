#!/usr/bin/env python3
import numpy as np
import sdf
import os
import sys
import argparse
import sympy as sp

def parselim(limstr):
	l0,l1 = limstr.split(",")
	l0 = l0.lstrip('([').lstrip()
	l1 = l1.rstrip(')]').rstrip()
	return [float(l0),float(l1)]

def readTable(file):
	ext = os.path.splitext(file)[-1]
	if ext == '.npy':
		table = np.load(file)
	else:
		table = np.loadtxt(file)
	return table

parser = argparse.ArgumentParser(description='create density map images')
parser.add_argument('indir',help='set input directory')
parser.add_argument('outfile',help='set output file',nargs='?')
parser.add_argument('-f','--flim',type=str,help='process from fmin th file to fmax th file (index starts from 0). usage : -f [fmin,fmax]',metavar='[fmin,fmax]')
parser.add_argument('-m','--mask',type=str,help='mask file')
parser.add_argument('-e','--equation',type=str,help='specify mask by equation. eg:x>y')

args=parser.parse_args()

files = [file for file in os.listdir(args.indir) if file.endswith('.sdf')]
files.sort()

if args.flim is not None:
	args.flim = list(map(int,parselim(args.flim)))
	files = files[args.flim[0]:args.flim[1]]
else:
	args.flim = [0,len(files)-1]

if args.mask is not None:
	args.mask = readTable(args.mask)

data = sdf.read(files[0]).Grid_Grid.data
xlen = abs(data[0][0] - data[0][-1])
ylen = abs(data[1][0] - data[1][-1]) 
area = xlen*ylen
dim = (len(data[0])-1,len(data[1])-1)
print(dim)

if args.equation is not None:
	x,y = sp.symbols('x y')
	eq = sp.parse_expr(args.equation)
	func = sp.lambdify([x,y],eq)
	args.mask = np.zeros(dim)
	
	for i in range(dim[0]):
		for j in range(dim[1]):
			val = func(0.5*(data[0][i]+data[0][i+1]),0.5*(data[1][j]+data[1][j+1]))
			if val==True or val==False:
				val = bool(val)
			args.mask[i,j] = val
	
	np.savetxt('dmask.txt',args.mask,fmt='%.1e')

output = args.outfile and open(args.outfile, 'w') or sys.stdout
output.write("filename\telectric energy [J]\tmagnetic energy [J]\tkinetic energy [J]\ttotal energy [J]\n")

ec = 0.5*8.854187e-12
bc = 0.5/1.256637e-6

def sdfen(file):
	if output is not sys.stdout:
		print('processing '+ file)
	
	data = sdf.read(file)
	if hasattr(data,'Electric_Field_Ex') and \
	hasattr(data,'Electric_Field_Ey') and \
	hasattr(data,'Electric_Field_Ez'):
		ex = getattr(data,'Electric_Field_Ex').data
		ey = getattr(data,'Electric_Field_Ey').data
		ez = getattr(data,'Electric_Field_Ez').data
		een = area * ec*(ex*ex + ey*ey + ez*ez) / ex.size
	else:
		een = ''

	if hasattr(data,'Magnetic_Field_Bx') and \
	hasattr(data,'Magnetic_Field_By') and \
	hasattr(data,'Magnetic_Field_Bz'):
		bx = getattr(data,'Magnetic_Field_Bx').data
		by = getattr(data,'Magnetic_Field_By').data
		bz = getattr(data,'Magnetic_Field_Bz').data
		ben = area * bc*(bx*bx + by*by + bz*bz) / bx.size
	else:
		ben = ''

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
	else:
		ken = ''

	if args.mask is not None:
		een = een * args.mask
		ben = ben * args.mask
		ken = ken * args.mask

	een = een.sum()
	ben = ben.sum()
	ken = ken.sum()
	ten = een+ben+ken

	output.write(f'{file}\t{een:e}\t{ben:e}\t{ken:e}\t{ten:e}\n')

for file in files:	
	sdfen(file)

if output is not sys.stdout:
	output.close()
