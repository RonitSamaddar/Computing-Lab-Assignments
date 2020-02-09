import ply.lex as lex
import ply.yacc as yacc
import sys
import os

URL_FILE=[]
listt=list(os.listdir("HTML"))
for l in listt:
	URL_FILE.append("HTML/"+l)
index=[-1]
name=[]
yr=[]
publication_records=[]
flag=[0,0,0,0,0]


# List of token names.
tokens = [
	"NAME_OPEN",
	"NAME_CLOSE",
	"CONTENT",
	"PAPER_NAME_START",
	"SPAN_END",
	"TYPE_START",
	"TYPE_END",
	"VENUE_START",
	"YEAR_START",
	"YEAR_START2",
	"YEAR_END2",
	"VOLUME_START",
	"ISSUE_START",
	"PAGE_START",
	"MINUS",
	"NUMBERS"
 ]

  # Regular expression tokens
t_NAME_OPEN					=	r'<title>'
t_NAME_CLOSE				=	r'</title>'
t_PAPER_NAME_START			=	r'<span\ class=\"title\"\ itemprop=\"name\">'								
t_SPAN_END					=	r'</span>'
t_CONTENT					=	r'[A-Za-z./?!:;@&#$][A-Za-z0-9./?!:;@&#$ ]*'
t_NUMBERS					=	r'[0-9]+'
t_TYPE_START				=	r'<div\ class="nr"\ id="[b|c|i|j|e|r|p][0-9]+">\['
t_TYPE_END					=	r'\]</div>'
t_VENUE_START				=	r'<span\ itemprop="name">'
t_YEAR_START				=	r'<span\ itemprop="datePublished">'
t_YEAR_START2				=	r'<li\ class="year">'
t_YEAR_END2					=	r'</li>'
t_VOLUME_START				=	r'<span\ itemprop="volumeNumber">'
t_ISSUE_START				=	r'<span\ itemprop="issueNumber">'
t_PAGE_START				=	r'<span\ itemprop="pagination">'
t_MINUS						=	r'-'
def t_NEWLINE(t):
     r'\n+'
     t.lexer.lineno += len(t.value)
def t_error(t):
     t.lexer.skip(1)
 

# Build the lexer
lexer = lex.lex()




def p_professor_name(p):
    'start : NAME_OPEN CONTENT NAME_CLOSE'
    p[2]=p[2][6:]
    name.append(p[2])
    flag[0]=1
  	
    #print("FOUND NAME = "+name[len(name)-1])

def p_type(p):
	'start : TYPE_START CONTENT TYPE_END'
	c=p[2][0]
	#print(c)
	if(c=='b'):
		ptype="Books and Theses"
	elif(c=='c'):
		ptype="Conference and Workshop Papers"
	elif(c=='i'):
		ptype="Informal and Other Publications"
	elif(c=='j'):
		ptype="Journal Articles"
	elif(c=='e'):
		ptype="Editorship"
	elif(c=='r'):
		ptype="Reference Works"
	elif(c=='p'):
		ptype="Parts in Books or Collections"
	#paper_type=ptype
	index[0]=index[0]+1
	publication_records.append(["","","","",0,0,0,0])
	#print("FOUND NAME = "+name[len(name)-1]+" AT INDEX = "+str(index[0]))
	#print("FOUND TYPE "+ptype+" AT INDEX = "+str(index[0]))
	publication_records[index[0]][0]=name[len(name)-1]
	publication_records[index[0]][1]=ptype
	#details[0]=name[len(name)-1]
	#details[1]=ptype
	#print("FOUND NAME = "+name[len(name)-1])
	flag[1]=1
	flag[2]=0
	

def p_paper_name(p):
	'start : PAPER_NAME_START CONTENT SPAN_END'
	#print("FOUND PAPER = "+p[2]+" AT INDEX = "+str(index[0]))
	#paper.append(p[2])
	publication_records[index[0]][2]=p[2]
	#details[2]=p[2]
	#print("FOUND NAME = "+name[len(name)-1])
	flag[2]=1
	

#def p_venue_year(p):
	#'start : VENUE_START CONTENT SPAN_END SPAN_END YEAR_START NUMBERS SPAN_END'
	#venue.append(p[2])
	#year=int(p[6])
	
	
	#details[3]=p[2]
	#details[4]=year
		#=["","","","",0]
	
	#print("FOUND NAME = "+name[len(name)-1])
	#print("FOUND VENUE = "+p[2])
	#print("FOUND YEAR = "+str(year)+" AT INDEX "+str(index))


def p_venue_name(p):
	'start : VENUE_START CONTENT SPAN_END'
	if(flag[0]==flag[1]==flag[2]==1):
		#print("FOUND VENUE = "+p[2]+" AT INDEX "+str(index[0]))
		publication_records[index[0]][3]=p[2]

def p_publish_year(p):
	'start : YEAR_START NUMBERS SPAN_END'
	#print("FOUND YEAR = "+p[2]+" AT INDEX "+str(index[0]))
	year=int(p[2])
	publication_records[index[0]][4]=year

def p_publish_year2(p):
	'start : YEAR_START2 NUMBERS YEAR_END2'
	#print("FOUND YEAR = "+p[2]+" AT INDEX "+str(index[0]))
	yr.append(int(p[2]))

def p_volume_number(p):
	'start : VOLUME_START NUMBERS SPAN_END'
	#print("FOUND YEAR = "+p[2]+" AT INDEX "+str(index[0]))
	volume=int(p[2])
	publication_records[index[0]][5]=volume
	#print("FOUND VOLUNE = "+str(volume))

def p_issue_number(p):
	'start : ISSUE_START NUMBERS SPAN_END'
	#print("FOUND YEAR = "+p[2]+" AT INDEX "+str(index[0]))
	issue=int(p[2])
	publication_records[index[0]][6]=issue
	#print("FOUND ISSUE = "+str(issue))

def p_page_number(p):
	'start : PAGE_START NUMBERS MINUS NUMBERS SPAN_END'
	#print("FOUND YEAR = "+p[2]+" AT INDEX "+str(index[0]))
	page=int(p[4])-int(p[2])
	#print("FOUND PAGE = "+str(page))
	publication_records[index[0]][7]=page
	publication_records[index[0]][4]=yr[len(yr)-1]


	
	#print(name,paper,paper_type,venue,year)

def p_error(p):
	pass





parser = yacc.yacc()
def strip_symbols(old_str):
	new_str=""
	allowed="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 _"
	for c in old_str:
		if c in allowed:
			new_str=new_str+c
	return new_str
def paper_filter(publication_records,to_find):
	match_list=[]
	for element in publication_records:
		if(element[0]=="" or element[1]=="" or element[2]==""):
			continue

		flag=1

		#MATCHING NAME:
		name1=element[0].lower()
		name2=to_find[0].lower()
		check_name2=name2.replace(' ','')
		if(check_name2.isalpha()!=True):
			print("Only alphabets allowed in name")
			return []
		else:
			if((name2!="_") and (name2 not in name1)):
				flag=0

		#MATCHING PAPER TYPE:
		type1=(element[1].lower())[0]
		type2=to_find[1].lower()
		#print(type2)
		#print(type(type2))
		if(type2 not in "abcdefghijklmnopqrstuvwxyz"):
			print("Only alphabet is allowed in publication type")
			return []
		else:
			if((type2!='_') and (type1!=type2)):
				flag=0

		#MATCHING PAPER:
		paper1=element[2].lower()
		paper1=strip_symbols(paper1)
		paper2=to_find[2].lower()
		paper2=strip_symbols(paper2)
		if((paper2!="_") and (paper2 not in paper1)):
				flag=0

		#MATCHING VENUE:
		venue1=element[3].lower()
		venue1=strip_symbols(venue1)
		venue2=to_find[3].lower()
		venue2=strip_symbols(venue2)
		if((venue1!="") and (venue2!="_") and (venue2 not in venue1)):
				flag=0

		year1=element[4]
		year2=to_find[4]
		if((year1!=0) and (year2!=-1) and (year2!=year1)):
			flag=0

		if(flag==1):
			match_list.append(element)
			#print("MATCHED")
		"""if(name2 in name1):

			print("Element = ")
			print(element)
			print("\nTo match")
			print([name1,type1,paper1,venue1,year1])
			print("\nTo find")
			print(to_find)
			print("\nFiltered To find")
			print([name2,type2,paper2,venue2,year2])
			print(flag)
			sys.stdin.read(1)
		"""

	return match_list






lexer=lex.lex()
for url in URL_FILE:
	fstring=""
	# Test it out
	print("PARSING FOR "+str(url))
	f=open(url,'r')
	fr=f.readlines()
	for line in fr:
		line=line.replace('\n','')
		if(len(line)!=0):
			fstring=fstring+line
	parser.parse(fstring,debug=0)
print("PARSING DONE")

