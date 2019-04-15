#!/usr/bin/env python
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.collections import LineCollection
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

class Trace(object):
	def __init__(self,x,y,px,py,pz):
		self.x = x
		self.y = y
		self.px = px
		self.py = py
		self.pz = pz
	def energy(self):
		return (np.sqrt((C_me*C_c*C_c)**2 + (self.px*self.px+self.py*self.py+self.pz*self.pz)*C_c*C_c) - C_me*C_c*C_c)/C_q

def checkTraceList(traceList,args):
	if args.xmin is not None:
		for t in traceList:
			if t.x < args.xmin :
				return False
		return True
	if args.xmax is not None:
		for t in traceList:
			if t.x > args.xmax :
				return False
		return True
	if args.ymin is not None:
		for t in traceList:
			if t.y < args.ymin :
				return False
		return True
	if args.ymax is not None:
		for t in traceList:
			if t.y > args.ymax :
				return False
		return True
	if args.pxmin is not None:
		for t in traceList:
			if t.px < args.pxmin :
				return False
		return True
	if args.pxmax is not None:
		for t in traceList:
			if t.px > args.pxmax :
				return False
		return True
	if args.pymin is not None:
		for t in traceList:
			if t.py < args.pymin :
				return False
		return True
	if args.pymax is not None:
		for t in traceList:
			if t.py > args.pymax :
				return False
		return True
	if args.pzmin is not None:
		for t in traceList:
			if t.pz < args.pzmin :
				return False
		return True
	if args.pzmax is not None:
		for t in traceList:
			if t.pz > args.pzmax :
				return False
		return True
	if args.enmin is not None:
		for t in traceList:
			if t.energy() < args.enmin :
				return False
		return True
	if args.enmax is not None:
		for t in traceList:
			if t.energy() > args.enmax :
				return False
		return True
	return True

def showTraces(traces,start,xlim,ylim,mfile,tfile,tail=10):
	def update(i,traceList,ax):
		print 'drawing frame ' + str(i)
		if i != 0:
			ax.cla()
		
		lines = []
		colors = []
		for trace in traceList:
			s = i-tail if i-tail >= 0 else 0 
			e = i if i < len(trace) else len(trace)-1
			for j in range(s,e-1):
				line = [[trace[j].x,trace[j].y],[trace[j+1].x,trace[j+1].y]]
				color = [0,0,0,(1-float(e-1-j)/tail)]
				lines.append(line)
				colors.append(color)
		
		ax.add_collection(LineCollection(lines,colors=colors))
		plt.title('frame '+ str(start+i+1),fontsize=20)
	
	traceList = traces.values()
	fig,ax = plt.subplots()
	ax.set_xlabel('X [m]',fontsize=18)
	ax.set_ylabel('Y [m]',fontsize=18)
	ax.get_xaxis().set_major_formatter(mpl.ticker.FormatStrFormatter('%.0e'))
	ax.get_yaxis().set_major_formatter(mpl.ticker.FormatStrFormatter('%.0e'))
	if xlim is not None:
		ax.set_xlim(xlim[0],xlim[1])
	if ylim is not None:
		ax.set_ylim(ylim[0],ylim[1])

	if mfile is not None:
		mov.save(mfile,writer='ffmpeg')
		mov = animation.FuncAnimation(fig,update,fargs=(traceList,ax),interval=10,frames=max(map(len,traceList)))
	if tfile is not None:	
		for trace in traceList:
			x = []
			y = []
			for t in trace:
				x.append(t.x)
				y.append(t.y)
			ax.plot(x,y,'b-')
		plt.savefig(tfile)
	#for trace in traceList:
	#	x = []
	#	y = []
	#	for t in trace:
	#		x.append(t.x)
	#		y.append(t.y)
	#	plt.plot(x,y,'k-',alpha=0.5)
	#while run:
	#	run = False
	#	print 'drawing time ' + i
	#	for trace in traceList:
	#		if i < len(trace):
	#			run = True
	#			x = []
	#			y = []
	#			s = i-tail if i-tail >= 0 else 0 
	#			for j in range(s,i):
	#				x.append(trace[j].x)	
	#				y.append(trace[j].y)
	#			plt.plot(x,y,'o-')
	#	movie.append(plt)
	#	i+=1
	#animation.ArtistAnimation(fig,movie,interval=100)
def printTraces(traces,file):
	with open(file,mode='w') as f:
		f.write('\t'.join(['x','y','px','py','px','energy']))
		f.write('\n')
		for trace in traces.values():
			for t in trace:
				print 'energy : ' + t.energy() + '\n' 
				f.write('\t'.join(map(str,[t.x,t.y,t.px,t.py,t.pz,t.energy()])))
				f.write('\n')
			f.write('\n')	

parser = argparse.ArgumentParser(description='show traces of particles')
parser.add_argument('indir',help='input directory')
parser.add_argument('specie',help='specie name',nargs='?')
parser.add_argument('-n','--nfile',help='output numerical data file name')
parser.add_argument('-m','--mfile',help='output movie mp4 file name')
parser.add_argument('-t','--tfile',help='output trajectory png file name')
parser.add_argument('-x','--xlim',type=str,help='set the x limits of the plot. usage : -x "[xmin,xmax]"',metavar='[xmin,xmax]')
parser.add_argument('-y','--ylim',type=str,help='set the y limits of the plot. usage : -y "[ymin,ymax]"',metavar='[ymin,ymax]')
parser.add_argument('-f','--flim',type=str,help='process from fmin th file to fmax th file (index starts from 0). usage : -f [fmin,fmax]',metavar='[fmin,fmax]')
parser.add_argument('--xmin',type=float)
parser.add_argument('--xmax',type=float)
parser.add_argument('--ymin',type=float)
parser.add_argument('--ymax',type=float)
parser.add_argument('--pxmin',type=float)
parser.add_argument('--pxmax',type=float)
parser.add_argument('--pymin',type=float)
parser.add_argument('--pymax',type=float)
parser.add_argument('--pzmin',type=float)
parser.add_argument('--pzmax',type=float)
parser.add_argument('--enmin',type=float)
parser.add_argument('--enmax',type=float)
parser.add_argument('--idmin',type=float)
parser.add_argument('--idmax',type=float)

args=parser.parse_args()

files = [file for file in os.listdir(args.indir) if file.endswith('.sdf')]
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

for k,v in args.__dict__.items():
	print(k + " : "+ str(v))

with nostdout():
		data = sh.getdata(files[0])
if args.specie is None or not hasattr(data,'Grid_Particles_'+ args.specie):
	print 'choose specie name from the list shown below'
	matcher = matcher=re.compile('Particles_.+?_(.+)')
	for y in list(set([x.group(1) for x in (matcher.match(str) for str in data.__dict__.keys()) if x])):
		print y
	exit()	

traces = defaultdict(list)

for file in files:
	print('processing ' + file)
	with nostdout():
		data = sh.getdata(file)
	grid = getattr(data,'Grid_Particles_' + args.specie).data
	x = grid[0]
	y = grid[1]
	px = getattr(data,'Particles_Px_' + args.specie).data
	py = getattr(data,'Particles_Py_' + args.specie).data
	pz = getattr(data,'Particles_Pz_' + args.specie).data
	pid = getattr(data,'Particles_ID_' + args.specie).data
	for i,ipid in enumerate(pid):
		traces[ipid].append(Trace(x[i],y[i],px[i],py[i],pz[i]))
if args.idmin is None:
	args.idmin = min(traces.keys())
if args.idmax is None:
	args.idmax = max(traces.keys())

traces = {k:v for k,v in traces.iteritems() if checkTraceList(v,args) and k < args.idmax and k >= args.idmin}
print('processing done')

if args.xlim is not None:
	args.xlim = parselim(args.xlim)
if args.ylim is not None:
	args.ylim = parselim(args.ylim)

showTraces(traces,args.flim[0],args.xlim,args.ylim,args.mfile,args.tfile)
if args.nfile is not None:
	printTraces(traces,args.nfile) 

