#!/usr/bin/env python3
import numpy as np
import sympy as sp
import os
import sys
import argparse

def parselim(limstr):
	l0,l1 = limstr.split(",")
	l0 = l0.lstrip('([').lstrip()
	l1 = l1.rstrip(')]').rstrip()
	return [int(l0),int(l1)]

parser = argparse.ArgumentParser(description='create mask array from input equation')
parser.add_argument('dim',help='dimension of input array')
parser.add_argument('expr',help='input equation (ex. x > y)')
parser.add_argument('outfile',help='set output file',default='mask.npy',nargs='?')
args=parser.parse_args()

args.dim = parselim(args.dim)

arr = np.zeros(args.dim)
row,col = arr.shape
x,y = sp.symbols('x y')
eq = sp.parse_expr(args.expr)

for i in range(row):
	for j in range(col):
		val=eq.subs([(x,j),(y,i)])
		if val==True or val==False:
			val=bool(val)
		arr[i,j] = val

print(arr)

out,ext = os.path.splitext(args.outfile)
if ext == '.npy':
	np.save(out,arr,allow_pickle=False)
elif ext == '.csv':
	np.savetxt(args.outfile,arr,delimiter=',')
else:
	np.savetxt(args.outfile,arr)

