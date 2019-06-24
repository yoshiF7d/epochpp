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

parser = argparse.ArgumentParser(description='show density distribution')
parser.add_argument('indir',help='input directory')
parser.add_argument('outdir',help='output directory')
parser.add_argument('specie',help='specie name',nargs='?')
parser.add_argument('-e','--elim',type=str,help='set the energy limits. usage : -e [emin,emax]',metavar='[emin,emax]')
parser.add_argument('-c','--clim',type=str,help='set the count limits for histogram plot. usage : -c [cmin,cmax]',metavar='[cmin,cmax]')
parser.add_argument('-b','--binc',type=int,default=256,help='set the bin count for histogram plot.')
parser.add_argument('-x','--xlim',type=str,help='set the x limits. usage : -x [xmin,xmax]',metavar='[xmin,xmax]')
parser.add_argument('-y','--ylim',type=str,help='set the y limits. usage : -y [ymin,ymax]',metavar='[ymin,ymax]')
parser.add_argument('-O','--overlay',type=str,help='specie name for overlay profile. example : -O "Derived_Number_Density_electron"')
parser.add_argument('-o','--olim',type=str,help='set the limits for overlay plot. usage : -o [omin,omax]',metavar='[omin,omax]')
parser.add_argument('-s','--skip',type=int,help='show only 1/SKIP particles')
parser.add_argument('-f','--flim',type=str,help='process from fmin th file to fmax th file (index starts from 0). usage : -f [fmin,fmax]',metavar='[fmin,fmax]')
args=parser.parse_args()


files = [file for file in os.listdir(args.indir) if file.endswith('.sdf')]

if not os.path.exists(args.outdir):
	os.makedirs(args.outdir)
files.sort()

def parselim(limstr):
	l0,l1 = limstr.split(",")
	l0 = l0.lstrip('([').lstrip()
	l1 = l1.rstrip(')]').rstrip()
	return [float(l0),float(l1)]

if args.flim is not None:
	args.flim = list(map(int,parselim(args.flim)))
	files = files[args.flim[0]:args.flim[1]]
else:
	args.flim = [0,len(files)-1]

with nostdout():
		data = sh.getdata(files[0])
if args.specie is None or not hasattr(data,'Grid_Particles_'+args.specie):
	print('choose specie name from the list shown below')
	matcher = matcher=re.compile('Particles_.+?_(.+)')
	for y in list(set([x.group(1) for x in (matcher.match(str) for str in data.__dict__.keys()) if x])):
		print(y)
	exit()	

if args.overlay is not None:
	if not hasattr(data,args.overlay):
		print('choose specie name from the list shown below for --overray option')
		matcher = matcher=re.compile('Particles_.+?_.+')
		for y in list(set([str for str in data.__dict__.keys if not matcher.match(str) ])):
			print(y)
		exit()

args.skip = 1 if args.skip is None else args.skip

grid = getattr(data,'Grid_Particles_' + args.specie).data
x = grid[0][::args.skip]
y = grid[1][::args.skip]

if args.xlim is not None:
	args.xlim = parselim(args.xlim)
else:
	args.xlim = [min(x),max(x)]

if args.ylim is not None:
	args.ylim = parselim(args.ylim)
else:
	args.ylim = [min(y),max(y)]

if args.elim is not None:
	args.elim = parselim(args.elim)

if args.clim is not None:
	args.clim = parselim(args.clim)

if args.olim is not None:
	args.olim = parselim(args.olim)

for k,v in args.__dict__.items():
	print(k + " : "+ str(v))

for file in files:
	print('processing '+ file)
	with nostdout():
		data=sh.getdata(file)
	grid = getattr(data,'Grid_Particles_' + args.specie).data
	x = grid[0][::args.skip]
	y = grid[1][::args.skip]
	px = getattr(data,'Particles_Px_' + args.specie).data[::args.skip]
	py = getattr(data,'Particles_Py_' + args.specie).data[::args.skip]
	pz = getattr(data,'Particles_Pz_' + args.specie).data[::args.skip]
	w = getattr(data,'Particles_Weight_' + args.specie).data[::args.skip]
	en = (np.sqrt((C_me*C_c*C_c)**2 + (px*px+py*py+pz*pz)*C_c*C_c) - C_me*C_c*C_c)/C_q
	enmin = min(en) if args.elim is None else args.elim[0]
	enmax = max(en) if args.elim is None else args.elim[1]
	enmin = tiny if enmin <= 0 else enmin
	cm = plt.get_cmap('rainbow')
	c=cm((np.log10(en+tiny)-np.log10(enmin))/(np.log10(enmax/enmin)))
	a=np.interp(np.log10(en+tiny),(np.log10(enmin),np.log10(enmax)),(0,1))
	c[:,-1]=a[:]
	fig,axes = plt.subplots(1,2,figsize=(15,8))
	if args.overlay is not None:
		overlay = getattr(data,args.overlay).data
		omin = map(min,overlay) if args.olim is None else args.olim[0]
		omax = map(max,overlay) if args.olim is None else args.olim[1]
		axes[0].imshow(
			np.flipud(overlay.transpose()),cmap=plt.get_cmap("gray_r"),
			norm=mpl.colors.Normalize(omin,omax),
			extent=[args.xlim[0],args.xlim[1],args.ylim[0],args.ylim[1]],
			interpolation='none'
		)
	axes[0].scatter(x,y,color=c,s=0.1)
	axes[0].set_title('Particles',fontsize=30)
	axes[0].get_xaxis().set_major_formatter(mpl.ticker.FormatStrFormatter('%.0e'))
	axes[0].get_yaxis().set_major_formatter(mpl.ticker.FormatStrFormatter('%.0e'))
	axes[0].set_xlim(args.xlim)
	axes[0].set_ylim(args.ylim)
	axes[0].set_xlabel('X [m]',fontsize=18)
	axes[0].set_ylabel('Y [m]',fontsize=18)
	axes[0].set_aspect('equal')	
	hist = np.histogram(en,weights=w,range=(enmin,enmax),bins=np.logspace(np.log10(enmin),np.log10(enmax),args.binc))
	cmin = min(hist[0]) if args.clim is None else args.clim[0]
	cmax = max(hist[0]) if args.clim is None else args.clim[1]
	bgx = np.logspace(np.log10(enmin),np.log10(enmax),args.binc)
	bgy = np.logspace(np.log10(cmin),np.log10(cmax),2)
	X,Y=np.meshgrid(bgx,bgy)
	Z = np.vstack((bgx[:-1],))
	axes[1].pcolormesh(X,Y,Z,cmap='rainbow',norm=LogNorm(enmin,enmax))
	axes[1].set_xscale('log')
	axes[1].set_yscale('log')
	axes[1].set_xlim([enmin,enmax])
	axes[1].set_ylim([cmin,cmax])
	axes[1].set_aspect(np.log10(enmax/enmin)/np.log10(cmax/cmin))
	axes[1].plot(0.5*(hist[1][1:]+hist[1][:-1]),hist[0]+tiny,alpha=0)
	axes[1].set_xlabel('Energy [eV]',fontsize=18)
	axes[1].set_ylabel('Count [a.u]',fontsize=18)
	axes[1].fill_between(0.5*(hist[1][1:]+hist[1][:-1]), hist[0]+tiny, y2=cmax, color='gray')
	plt.suptitle(os.path.splitext(os.path.basename(file))[0]+' fs',fontsize=40)
	fig.tight_layout()
	plt.savefig(args.outdir+'/'+os.path.splitext(os.path.basename(file))[0]+'.png')
	plt.close()

