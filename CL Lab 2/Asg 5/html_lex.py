import ply.lex as lex

# List of token names.
tokens = [
	"NAME_OPEN",
	"NAME_START",
	"NAME_CLOSE",
	"SPACE",
	"IGNORE",
	"CONTENT",
	"PAPER_NAME_START",
	"SPAN_END",
	"TYPE_START1",
	"TYPE_START2",
	"TYPE_END",
	"VENUE_START",
	"YEAR_START",
	"NUMBERS"
 ]

  # Regular expression tokens
t_NAME_OPEN					=	r'<title>'
t_NAME_CLOSE				=	r'</title>'
t_IGNORE					=	r' \t'
t_PAPER_NAME_START			=	r'<span\ class=\"title\"\ itemprop=\"name\">'								
t_SPAN_END					=	r'</span>'
t_CONTENT					=	r'[A-Za-z./?!:;@&#$][A-Za-z0-9./?!:;@&#$ ]*'
t_NUMBERS					=	r'[0-9]+'
t_TYPE_START1				=	r'<div\ id="[b|c|i|j|e|r|p][0-9]+"\ class="nr">\['
t_TYPE_START2				=	r'<div\ class="nr"\ id="[b|c|i|j|e|r|p][0-9]+">\['
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
 

URL_FILE="HTML/test.html"
 
# Test it out
f=open(URL_FILE,'r')
fr=f.readlines()
for data in fr:
	print("\nTOKENIZING A NEWLINE: ")
	# Give the lexer some input
	lexer.input(data)
	# Tokenize
	while True:
		tok = lexer.token()
		if not tok:
	 		break      # No more input
		print(tok)
		
	 	




     

