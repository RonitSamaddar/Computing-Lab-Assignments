import urllib.request, urllib.error, urllib.parse

URL_FILE="professors.txt"

def extract_content(url):
    response = urllib.request.urlopen(url)
    webContent = response.read()
    return webContent

def extract_name(url):
    #print(url)
    pos=url.rfind('/')
    name=url[pos+1:]
    file_name="HTML/"+name+".html"
    return file_name

def write_html(webcontent,file_name):
    f = open(file_name, 'wb')
    f.write(webcontent)
    f.close


def main():    
	f=open(URL_FILE,'r')
	fr=f.readlines()
	for line in fr:
	    line=line.replace('\n','')
	    url=line
	    if url=='':
	        break
	    file_name=extract_name(url)
	    webcontent=extract_content(url)
	    write_html(webcontent,file_name)
	    print(file_name)
	print("DONE")

if __name__=="__main__":
	main()


    