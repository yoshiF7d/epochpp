#!/usr/bin/env python
import os
import stat
import numpy as np
import shutil
import re
import argparse
import ast
import sys
import collections

parser = argparse.ArgumentParser(description='setup multiple epoch simulations for parameter sweeping',formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('template',help='template input file. the keywords \'$0\',\'$1\',... in the template file are replaced by xi,yi,... in params')
parser.add_argument('params',help='parameter lists. syntax : \"[[x1,x2,...],[y1,y2,...],...]\".')
parser.add_argument('runscript',help='script to run each simulation')
parser.add_argument('-i','--inputfile',type=str,default='input.deck',help='name of the input file made from template input file')
parser.add_argument('-r','--runallfile',type=str,default='epochps_runall.py',help='name of the script to run all simulations.')
parser.add_argument('-c','--copyfiles',type=str,help='files that are copied to all simulation folders. syntax : \"[\'file1\',\'file2\',...]\"')
args=parser.parse_args()

class ListParser:
	def __init__(self,string=''):
		self.set(string)
	def set(self,string):
		self.string = string
		self.length = len(string)
		self.index = 0
	def parse(self,string=None):
		if(string is not None):
			self.set(string)
		return self.parse_mod()[0]
	
	def parse_mod(self):
		list=[]
		i = self.index
		while i < self.length:
			c = self.string[i]
#			msg = c + ' : '
			if c == '[':
#				msg += 'Left Bracket'
				self.index = i+1
				list.append(self.parse_mod())
				i = self.index-1
			elif c == ']':
#				msg += 'Right Bracket'
				if self.index < i:
					list.append(self.string[self.index:i])
				self.index = i+1
				break
			elif c == ',':
#				msg += 'Comma'
				if self.index < i:
					list.append(self.string[self.index:i])
				self.index = i+1
			elif c == '\'':
#				msg += 'Single Quote'
				self.index = i+1+self.string[i+1:].index('\'')+1
				list.append(self.string[i+1:self.index-1])
				i = self.index-1
			elif c == '\"':
#				msg += 'Double Quote'
				self.index = i+1+elf.string[i+1:].index('\"')+1
				list.append(self.string[i+2:self.index-2])
				i = self.index-1
#			else:
#				msg += 'Else'
#			print(msg)
			i+=1
		else:
			if self.index < i:
				list.append(self.string[self.index:])
			self.index = self.length-1
		return list

listParser = ListParser()
params=listParser.parse(args.params)

if args.copyfiles is not None:
	copyfiles=listParser.parse(args.copyfiles)

#params = [[1e+19,1e+20,1e+21],[1000,500,100],['a','b','c'],['d','e']]
dirlist=[]
taglist=[]
with open(args.template) as f:
	src=f.read()

n = 1
for i in range(len(params)):
	n *= len(params[i])

ind = np.zeros(len(params),dtype=np.uint8)

for i in range(n):
	ii=i
	for j in range(len(params)):
		ind[j]=ii%len(params[j])
		ii = ii//len(params[j])
	dir = ''.join(map(str,ind))
	dst = src
	tag=dir + ' : { '
	for j in range(len(params)):
		ss = '$'+str(j)
		rs = str(params[j][ind[j]])
		dst = dst.replace(ss,rs)
		if j != 0:
			tag += ' , '
		tag += ss + ' -> ' + rs 
	tag+=' }'
	print(tag)
	if not os.path.exists(dir):
		os.mkdir(dir)
	with open(dir+'/'+args.inputfile,mode='w') as f:
		f.write(dst)
#	with open(dir+'/deck.file',mode='w') as f:
#		f.write('.')
	if args.copyfiles is not None:
		if isinstance(copyfiles,basestring):
			if os.path.exists(copyfiles):
				shutil.copyfile(copyfiles,dir+'/'+copyfiles)
				os.chmod(dir+'/'+copyfiles,0777)
		elif isinstance(copyfiles, collections.Iterable):
			for file in copyfiles:
				if os.path.exists(file):
					shutil.copyfile(file,dir+'/'+file)
					os.chmod(dir+'/'+file,0777)
	
	shutil.copyfile(args.runscript,dir+'/'+args.runscript)
	os.chmod(dir+'/'+args.runscript,0777)
	dirlist.append('\''+dir+'\'')
	taglist.append('\''+tag+'\'')

with open(args.runallfile,mode='w') as f:
	f.write('#!/usr/bin/env python\n')
	f.write('import subprocess\n')
	f.write('import os\n')
	f.write('dirlist=' + '(' + ','.join(dirlist) + ')\n' )	
	f.write('taglist=' + '(' + ','.join(taglist) + ')\n' )
	f.write('for i in range(len(dirlist)):\n')
	f.write('\tprint(\"simulation \"+taglist[i])\n')
	f.write('\tsubprocess.call(\'./'+args.runscript+'\',cwd=dirlist[i]) \n')
	os.chmod(args.runallfile,0777)

print('simulation count : ' + str(n))
print('input deck template : ' + args.template)
print('run script  : ' + args.runscript)
if args.copyfiles is not None:
	print('copyfiles  : ' + str(copyfiles))
print('type ./'+args.runallfile+' to start simulations')
