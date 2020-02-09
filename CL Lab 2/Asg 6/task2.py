import urllib.request, urllib.error, urllib.parse
import ply.lex as lex
import ply.yacc as yacc


#<font color="blue">&#9658;</font>
#			</span>
#			Natural language processing
#			<div class="table-responsive" style="display:none;" id="nlp-conferences">
#
#<a href="http://dblp.org/db/conf/nips/index.html">NeurIPS/NIPS</a>


venue_list=[]
count=[0]
# List of token names.
tokens = [
	"CONF_OPEN1","CONF_OPEN2"
 ]

  # Regular expression tokens

def t_CONF_OPEN1(t):
	r'<a\ href="http://dblp.org/db/conf/[A-Za-z0-9][A-Za-z0-9 .]*/index.html">'
	add=t.value
	r=add.rfind('/')
	venue=add[33:r]
	venue_list.append(venue)
	count[0]+=1
	#print(t)
def t_CONF_OPEN2(t):
	r'<a\ href="http://dblp.org/db/conf/[A-Za-z0-9][A-Za-z0-9 .]*/">'
	add=t.value
	r=add.rfind('/')
	venue=add[33:r]
	venue_list.append(venue)
	count[0]+=1
	#print(t)
def t_NEWLINE(t):
     r'\n+'
     t.lexer.lineno += len(t.value)
def t_error(t):
     t.lexer.skip(1)
 

# Build the lexer
lexer = lex.lex()




url="http://csrankings.org"
response = urllib.request.urlopen(url)
webContent = response.read()
data=str(webContent)
data=data.replace("&amp;","&")
f = open("CSRankings.html", 'wb')
f.write(webContent)
lexer.input(data)
 
 # Tokenize
while True:
    tok = lexer.token()
    if not tok: 
        break      # No more input

#print(venue_list)
#print(count[0])