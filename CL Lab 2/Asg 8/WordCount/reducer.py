import  sys
index=0
current_word=""
current_count=0
for line in sys.stdin:
	line=line.strip()
	word=line.split()[0]
	count=int(line.split()[1])
	if(current_word=="" or word==current_word):
		current_count+=1
	else:
		print(current_word,current_count)
		current_count=1
	current_word=word
print(current_word,current_count)


	