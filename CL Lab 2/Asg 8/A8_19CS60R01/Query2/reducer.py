import  sys
current_iid=""
current_count=0
current_max_iid=""
current_max_count=0
print("ITEM ID WITH MAXIMUM RATINGS OF 4 OR 5")
for line in sys.stdin:
	line=line.strip()
	iid=line.split()[0]
	count=int(line.split()[1])
	if(current_iid=="" or iid==current_iid):
		current_count+=1
	else:
		if(current_count>=current_max_count):
			current_max_iid=current_iid
			current_max_count=current_count
		current_count=1
	current_iid=iid
if(current_count>=current_max_count):
			current_max_iid=current_iid
			current_max_count=current_count
print(current_max_iid)