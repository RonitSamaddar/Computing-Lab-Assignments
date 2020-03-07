import  sys
current_iid=""
current_price_flag=0
current_view_count=0
print("ITEMS WITH PRICE MORE THEN 3USD AND APPEARING IN ALSO VIEWED OF ATLEAST 5 OTHER ITEMS")
for line in sys.stdin:
	line=line.strip()
	iid=line.split()[0]
	price=float(line.split()[1])
	view=int(line.split()[2])
	if(current_iid=="" or iid==current_iid):
		if(current_price_flag==0 and price==1):
			current_price_flag=price
		if(view==1):
			current_view_count+=1
	else:
		if(current_price_flag==1 and current_view_count>=5):
			print(current_iid)
		if(price==1):
			current_price_flag=1
		else:
			current_price_flag=0
		if(view==1):
			current_view_count=1
		else:
			current_view_count=0
	current_iid=iid
if(current_price_flag==1 and current_view_count>=5):
			print(current_iid)