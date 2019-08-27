#!/usr/bin/env python
import matplotlib as mpl
mpl.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import colorConverter
import sdf_helper as sh
import sys
import os
import contextlib
from collections import defaultdict
import subprocess

class DummyFile(object):
	def write(self,x): pass
@contextlib.contextmanager
def nostdout() :
	save_stdout = sys.stdout
	sys.stdout = DummyFile()
	yield
	sys.stdout = save_stdout

files = [file for file in os.listdir('.') if file.endswith('.sdf')]
files.sort()

c1 = colorConverter.to_rgba('white',alpha=0.0)
c2 = colorConverter.to_rgba('black',alpha=0.8)
cmapne=mpl.colors.LinearSegmentedColormap.from_list('cmapne',[c1,c2],256)
cmapey=plt.get_cmap('bwr')
nemin=0
nemax=2e+25
eymin=-5e+12
eymax=5e+12
if not os.path.exists('out'):
	os.mkdir('out')

for file in files:
	with nostdout():
		data=sh.getdata(file)
	ne=np.flipud(data.Derived_Number_Density_electron.data.transpose())
	ey=np.flipud(data.Electric_Field_Ey.data.transpose())
	fig,ax=plt.subplots()
	ax.imshow(ey,cmap=cmapey,norm=mpl.colors.Normalize(eymin,eymax),interpolation='none')
	ax.imshow(ne,cmap=cmapne,norm=mpl.colors.Normalize(nemin,nemax),interpolation='none')
	plt.savefig('out/'+os.path.splitext(os.path.basename(file))[0]+'.png')
	plt.close()

args = ['ffmpeg','-r', '30', '-i', 'out/%04d.png', '-vcodec','libx264','-pix_fmt','yuv420p','-r','30','-y', 'out.mp4']
subprocess.call(args)
args = ['rm','-r','out']
subprocess.call(args)