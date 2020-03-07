import sys
word_count={}
file=open("input.txt",'r')
for line in file:
	line=line.strip()
	for word in line.split(' '):
		print(word+" "+str(1))


