import  sys
current_uid=""
current_count=0
print("USERS WHO HAVE RATED ATLEAST 10 PRODUCTS")
for line in sys.stdin:
	line=line.strip()
	uid=line.split()[0]
	count=int(line.split()[1])
	if(current_uid=="" or uid==current_uid):
		current_count+=1
	else:
		if(current_count>=10):
			print(current_uid,end=' ')
		current_count=1
	current_uid=uid
if(current_count>=10):
			print(current_uid,end=' ')