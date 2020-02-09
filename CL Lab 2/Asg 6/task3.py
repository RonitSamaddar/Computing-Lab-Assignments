from task1 import publication_records
from task2 import venue_list

def get_prof_record(prof_name):
	records=[]
	for element in publication_records:
		target=prof_name.lower()
		value=element[0].lower()
		if(target in value):
			records.append(element)
	return records

print("Enter professor's name")
prof_name=input()
#print(prof_name)
prof_records=get_prof_record(prof_name)

print("PART 1 : PAPERS IN SELECTED VENUES")
#print(venue_list)
papers1=[]
for element in prof_records:
	venue=element[3].lower()
	if(element[3] in venue_list or venue in venue_list):
		print("\n")
		print(element)
		papers1.append(element)
#print(prof_venues)

print("\nPART 2 : PAPERS ATLEAST 6 PAGES LONG(FULL PAPERS)")
#print(venue_list)
papers2=[]
for element in papers1:
	if element[7]>=6:
		print("\n")
		print(element[2])
		papers2.append(element)

print("\nPART 3 : TOP CONFERENCES FULL PAPER")
#print(venue_list)
count=0
for element in papers2:
	ptype=element[1].lower()
	if ptype[0]=='c':
		count+=1
print("COUNT = "+str(count))