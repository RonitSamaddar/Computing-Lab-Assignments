import sys
import gzip 
fp=gzip.open('../items.json.gz') 
for line in fp: 
	review=eval(line)
	
	#dict_keys(['asin', 'related', 'title', 'price', 'salesRank', 'imUrl', 'brand', 'categories', 'description'])
	#print(review['reviewerID'],str(1))
	if('price' in review.keys()):
		if(review['price']>3):
			print(review['asin'],1,0)
	if('related' in review.keys()):
		if('also_viewed' in review['related'].keys()):
			item_list=review['related']['also_viewed']
			for item in item_list:
				print(item,0,1)

	