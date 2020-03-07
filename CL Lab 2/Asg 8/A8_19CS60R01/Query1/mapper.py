import gzip 
fp=gzip.open('../reviews.json.gz') 
for line in fp: 
	review=eval(line)
	print(review) 