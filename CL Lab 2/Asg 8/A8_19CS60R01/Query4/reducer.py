import  sys
current_uid=""
current_count=0
user_count=[[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]]#Will be maintained in descending order of counts
for line in sys.stdin:
	line=line.strip()
	uid=line.split()[0]
	count=int(line.split()[1])
	if(current_uid=="" or uid==current_uid):
		current_count+=1
	else:
		#print(current_count,current_uid)
		if(current_count>user_count[9][1]):
			user_count[9]=[current_uid,current_count]
			for i in range(8,-1,-1):
				if(user_count[i][1]<current_count):
					tid=user_count[i][0]
					tcount=user_count[i][1]
					user_count[i]=user_count[i+1]
					user_count[i+1]=[tid,tcount]
				#print(user_count)
				#print("\n")
		current_count=1
	current_uid=uid
print("TOP 10 USER IDS WITH REVIEWS OF MORE THAN 20 CHARACTERS")
for element in user_count:
	print(element[0])