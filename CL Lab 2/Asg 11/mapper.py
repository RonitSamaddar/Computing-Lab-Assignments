import sys

file=sys.argv[1]

def pad(s):
	if(int(s)<10):
		s='0'+s
	return s
fp=open(file,'r')

for line in fp:
	line=line.strip()
	name,row,col,val=line.split(',')
	row=pad(row)
	col=pad(col)
	print(row,col,name,val)