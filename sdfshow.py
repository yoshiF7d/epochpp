#!/usr/bin/env python
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import sdf_helper as sh
import os
import sys
import argparse
import re

C_c=2.997925e+08
C_me=9.109383e-31
C_q=1.60217657e-19

parser = argparse.ArgumentParser(description='create density map images')
parser.add_argument('indir',help='set input directory')
parser.add_argument('variable',help='set variable name',nargs='?')
parser.add_argument('outdir',help='set output directory',default=os.getcwd(),nargs='?')
parser.add_argument('-m','--min',type=float,help='set min')
parser.add_argument('-M','--max',type=float,help='set max')
parser.add_argument('-c','--highlight',help='highlight value in range (hmin,hmax)',metavar='(hmin,hmax)')
args=parser.parse_args()

files = os.listdir(args.indir)
files.sort()
for file in files:
	if file.endswith('.sdf'):
		data = sh.getdata(file)
		print file
		break		

if args.variable is None:
	print 'choose variable from list shown below (mesh variable only)'
	for x in  data.__dict__:
		print x
	sys.exit()

var = getattr(data,args.variable).data

if not os.path.exists(args.outdir):
	os.makedirs(args.outdir)

if args.highlight is not None:
	hmin,hmax = args.highlight.split(",")
	hmin = hmin.lstrip('([').lstrip()
	hmax = hmax.rstrip(')]').rstrip()
	hmin = float(hmin)
	hmax = float(hmax)

def sdfen(file):
	if file.endswith('.sdf'):
		print('processing '+ file)
		data=sh.getdata(file)
		var = np.flipud(getattr(data,args.variable).data.transpose())
		if args.min is None:
			min = np.min(var)
		else:
			min = args.min
		
		if args.max is None:
			max = np.max(var)	
		else:
			max = args.max
		
		print 'min : ' + str(min)
		print 'max : ' + str(max)
		if args.highlight is not None:
			print 'hmin : ' + str(hmin)
			print 'hmax : ' + str(hmax)
			hhmin = (hmin-min)/(max-min)
			hhmax = (hmax-min)/(max-min)
			r = (var-min)/(max-min)
			b = (var-min)/(max-min)
			g = (var-min)/(max-min)
			r[(r>hhmin) & (r<hhmax)] = 1.
			b[(b>hhmin) & (b<hhmax)] = 0.
			g[(g>hhmin) & (g<hhmax)] = 0.
			v = 1-np.dstack((r,b,g))
			plt.imshow(v,interpolation='none')
		else: 
			plt.imshow(var,cmap=plt.get_cmap("gray_r"),norm=mpl.colors.Normalize(min,max),interpolation='none')
			plt.colorbar()

		plt.title('Particles',fontname='Arial',fontsize=30)
		ax = plt.gca()
		#ax.get_xaxis().set_major_formatter(mpl.ticker.FormatStrFormatter('%.0e'))
		#ax.get_yaxis().set_major_formatter(mpl.ticker.FormatStrFormatter('%.0e'))
		plt.xlabel('X [um]',fontname='Arial',fontsize=18)
		plt.ylabel('Y [um]',fontname='Arial',fontsize=18)
		ax.set_aspect('equal')
		plt.tight_layout()
		plt.suptitle(os.path.splitext(os.path.basename(file))[0]+' fs',fontsize=40)
		plt.savefig(args.outdir+'/'+os.path.splitext(os.path.basename(file))[0]+'.png')	
		plt.close()

for file in files:
	sdfen(file)

