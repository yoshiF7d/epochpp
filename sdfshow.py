#!/usr/bin/env python
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import sdf_helper as sh
import os
import sys
import argparse
import re
import contextlib

class DummyFile(object):
	def write(self,x): pass
@contextlib.contextmanager
def nostdout() :
	save_stdout = sys.stdout
	sys.stdout = DummyFile()
	yield
	sys.stdout = save_stdout

def parselim(limstr):
	l0,l1 = limstr.split(",")
	l0 = l0.lstrip('([').lstrip()
	l1 = l1.rstrip(')]').rstrip()
	return [float(l0),float(l1)]

parser = argparse.ArgumentParser(description='create density map images')
parser.add_argument('indir',help='set input directory')
parser.add_argument('variable',help='set variable name',nargs='?')
parser.add_argument('outdir',help='set output directory',default=os.getcwd(),nargs='?')
parser.add_argument('-l','--lim',type=float,help='set limits',metavar='[min,max]')
parser.add_argument('-M','--max',type=float,help='set max')
parser.add_argument('-c','--highlight',help='highlight value in range [hmin,hmax]',metavar='[hmin,hmax]')
parser.add_argument('-f','--flim',type=str,help='process from fmin th file to fmax th file (index starts from 0). usage : -f [fmin,fmax]',metavar='[fmin,fmax]')

args=parser.parse_args()

files = [file for file in os.listdir(args.indir) if file.endswith('.sdf')]
files.sort()

if args.flim is not None:
	args.flim = list(map(int,parselim(args.flim)))
	files = files[args.flim[0]:args.flim[1]]
else:
	args.flim = [0,len(files)-1]

with nostdout():
	data = sh.getdata(files[0])

if args.variable is None or not hasattr(data,args.variable):
	print('choose variable from list shown below (mesh variable only)')
	for x in  data.__dict__:
		print(x)
	sys.exit()

var = getattr(data,args.variable).data

if not os.path.exists(args.outdir):
	os.makedirs(args.outdir)

if args.highlight is not None:
	args.highlight = parselim(args.highlight)

if args.lim is not None:
	args.highlight = parselim(args.lim)
	
def sdfen(file):
	if file.endswith('.sdf'):
		print('processing '+ file)
		with nostdout():
			data=sh.getdata(file)
		var = np.flipud(getattr(data,args.variable).data.transpose())
		if args.lim is None:
			min = np.min(var) 
			max = np.max(var)
		else:
			min,max = args.lim
		print('min : ' + str(min))
		print('max : ' + str(max))
		if args.highlight is not None:
			print 'hmin : ' + str(hmin)
			print 'hmax : ' + str(hmax)
			hhmin = (args.highlight[0]-min)/(max-min)
			hhmax = (args.highlight[1]-min)/(max-min)
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

		plt.title(args.variable,fontname='Arial',fontsize=20)
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

