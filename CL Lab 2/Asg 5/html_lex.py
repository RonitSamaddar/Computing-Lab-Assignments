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
work_list=[]
flag=0


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
	work_list.append(["","","","",0])
	work_list[index[0]][0]=name[len(name)-1]
	work_list[index[0]][1]=ptype
	#details[0]=name[len(name)-1]
	#details[1]=ptype
	#print("FOUND NAME = "+name[len(name)-1])
	#print("FOUND TYPE "+ptype+" AT INDEX = "+str(index))

def p_paper_name(p):
	'start : PAPER_NAME_START CONTENT SPAN_END'
	#paper.append(p[2])
	work_list[index[0]][2]=p[2]
	#details[2]=p[2]
	#print("FOUND NAME = "+name[len(name)-1])
	#print("FOUND PAPER = "+p[2]+" AT INDEX = "+str(index))

def p_venue_year(p):
	'start : VENUE_START CONTENT SPAN_END SPAN_END YEAR_START NUMBERS SPAN_END'
	#venue.append(p[2])
	year=int(p[6])
	work_list[index[0]][3]=p[2]
	work_list[index[0]][4]=year
	#details[3]=p[2]
	#details[4]=year
		#=["","","","",0]
	
	#print("FOUND NAME = "+name[len(name)-1])
	#print("FOUND VENUE = "+p[2])
	#print("FOUND YEAR = "+str(year)+" AT INDEX "+str(index))


#def p_venue_name(p):
#	'start : VENUE_START CONTENT SPAN_END'
#	print("FOUND VENUE = "+p[2])
#def p_publish_year(p):
#	'start : YEAR_START NUMBERS SPAN_END'
#	year=int(p[2])
	
	#print(name,paper,paper_type,venue,year)

def p_error(p):
	pass





parser = yacc.yacc()


lexer=lex.lex()
for url in URL_FILE:
	fstring=""
	# Test it out
	print("PARSING FOR "+string(url))
	f=open(url,'r')
	fr=f.readlines()
	for line in fr:
		line=line.replace('\n','')
		if(len(line)!=0):
			fstring=fstring+line
	parser.parse(fstring,debug=0)
print("PARSING DONE")








	 	




     

