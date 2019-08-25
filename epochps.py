#!/usr/bin/env python
import os
import stat
import numpy as np
import shutil
import re
import argparse
import ast

parser = argparse.ArgumentParser(description='setup multiple epoch simulations for parameter sweep')
parser.add_argument('template',help='template input file. the keywords \'$0\',\'$1\',... in the template file are replaced by xi,yi,... in params')
parser.add_argument('params',help='parameter lists. syntax : [[x1,x2,...],[y1,y2,...],...]')
parser.add_argument('runscript',help='run script for each simulation')
parser.add_argument('-n','--runall',type=str,help='name of the script to run all simulations. default file name is \'epochps_runall.sh\'')
parser.add_argument('-p','--ppscript',type=str,help='post process script')
args=parser.parse_args()

srcfile=args.template
params = ast.literal_eval(args.params)
runscript=args.runscript
if args.runall is not None:
	epscript=args.runall
else:
	epscript='epochps_runall.sh'
if args.ppscript is not None:
	ppscript=args.ppscript

#params = [[1e+19,1e+20,1e+21],[1000,500,100],['a','b','c'],['d','e']]
dirlist=[]
taglist=[]
with open(srcfile) as f:
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
	with open(dir+'/input.deck',mode='w') as f:
		f.write(dst)
	with open(dir+'/deck.file',mode='w') as f:
		f.write('.')
	if args.ppscript is not None:
		shutil.copyfile(ppscript,dir+'/'+ppscript)
		os.chmod(dir+'/'+ppscript,0777)
	shutil.copyfile(runscript,dir+'/'+runscript)
	os.chmod(dir+'/'+runscript,0777)
	dirlist.append(dir)
	taglist.append('\''+tag+'\'')

with open(epscript,mode='w') as f:
	f.write('#!/bin/sh\n')
	f.write('dirlist=' + '(' + ' '.join(dirlist) + ')\n' )	
	f.write('taglist=' + '(' + ' '.join(taglist) + ')\n' )
	f.write('i=0\n')
	f.write('for ((i=0;i<\"${#dirlist[@]}\";i++))\n')
	f.write('do')
	f.write('	echo \"processing ${taglist[$i]}\" \n')
	f.write('	cd ${dirlist[$i]}\n')
	f.write('	./' + runscript + '\n')
	f.write('	cd ../\n')
	f.write('done\n')
	os.chmod(epscript,0777)

print('simulation count : ' + str(n))
print('input deck template : ' + srcfile)
print('run script  : ' + runscript)
if args.ppscript is not None:
	print('post process script  : ' + ppscript)
print('type ./'+epscript+' to start simulations')
