import gzip 
fp=gzip.open('../reviews.json.gz') 
for line in fp: 
	review=eval(line)
	#print(review.keys())
	#dict_keys(['reviewerID', 'asin', 'reviewerName', 'helpful', 'reviewText', 'overall', 'summary', 'unixReviewTime', 'reviewTime'])
	if(review["overall"]>=4):
		print(review["asin"],1)
	

	