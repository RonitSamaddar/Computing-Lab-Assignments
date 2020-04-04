import sys

def pad(s):
	if(int(s)<10):
		s='0'+s
	return s


curr_r=-1
curr_c=-1
curr_val=0
for line in sys.stdin:
	line=line.strip()
	r,c,n,v=line.split(' ')
	if(n=='E'):
		for cp in range(1,51):
			print(pad(str(cp)),str(c),str(r),"B",str(v))
	if(r!=curr_r or c!=curr_c):
		#That is if start of new pair
		if(curr_r!=-1):
			for cp in range(1,51):
				print(str(r),pad(str(cp)),str(c),"A",str(v))
		#Now setting new values again
		add_dict={}
		curr_r=r
		curr_c=c
	else:
		if(n=='A'):
			curr_val+=float(v)
		elif(n=='B'):
			curr_val-=float(v)
		elif(n=='C'):
			curr_val-=float(v)
		elif(n=='D'):
			curr_val+=float(v)
	
if(curr_r!=-1):
	for cp in range(1,51):
		print(str(r),pad(str(cp)),str(c),"A",str(v))

#For E we print row,col
#For X =(D-B-C+A)




