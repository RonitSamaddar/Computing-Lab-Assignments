import gzip 
fi=gzip.open('../items.json.gz') 
for line in fi: 
	review=eval(line)
	#print(review.keys())
	#dict_keys(['asin', 'related', 'title', 'price', 'salesRank', 'imUrl', 'brand', 'categories', 'description'])
	if('price' in review.keys() and review['price']>2):
		print(review['asin'],1,0)

fr=gzip.open('../reviews.json.gz') 
for line in fr: 
	review=eval(line)
	#print(review.keys())
	#dict_keys(['reviewerID', 'asin', 'reviewerName', 'helpful', 'reviewText', 'overall', 'summary', 'unixReviewTime', 'reviewTime'])
	if('overall' in review.keys() and review['overall']==5):
		print(review['asin'],0,1)


	