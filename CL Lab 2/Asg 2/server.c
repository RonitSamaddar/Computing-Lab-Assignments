//HEADER FILES
#include<stdio.h>					// printf()
#include<stdlib.h>					// malloc()
#include<string.h>					// strlen()
#include<math.h>					// pow()


//MACRO DEFINATIONS
#define WORD_LENGTH 256
#define MAX_OPERAND 30


//FUNCTION PROTOTYPES
char *eval_postfix(char *);			// function to evaluate postfix given in string and return result or error



//MAIN FUNCTION
int main()
{
	
	char *str;


	str=(char *)malloc(WORD_LENGTH*sizeof(char));
	gets(str);
	printf("%s\n",eval_postfix(str));

}


//OTHER FUNCTION
char *eval_postfix(char *str)
{

	int len;						//length of string
	char *ret;						//return value(either result of operation or error message)
	int i;							//loop variable
	char c;							//variable to store and process each character
	float arr[MAX_OPERAND];			//array to store numbers in the postfix
	int top;						//variable for indexing the array
	double token;					//getting each token(number) in the postfix expression
	int nflag;						//flag that the current token is a number
	int fflag;						//flag that the current number is a floating point number
	int x;							//power of 10 to divide with		




	len=strlen(str);
	top=-1;
	token=0;
	ret=(char *)malloc(WORD_LENGTH*sizeof(int));
	nflag=0;
	fflag=0;
	x=10;
	for(i=0;i<len;i++)
	{
		c=str[i];


		//Checking Type Of c		
		//Type 1:Alphabet
		if(isalpha(c)!=0)
		{
			//letters should not be there;
			ret="ERROR : ONLY NUMBERS AND OPERATORS ALLOWED";
			return ret;
		}
		else if(isdigit(c)!=0)
		{
			nflag=1;
			if(fflag==1)
			{
				//Already decimal point occured before
				token=token+((0+(c-'0'))*1.0/x);
				x=x*10;
			}
			else
			{
				token=token*10+(0+(c-'0'));
			}
		}
		else if(c=='.')
		{
			if(((i==0)||(isdigit(str[i-1])==0))&&((i==len-1)||(isdigit(str[i+1])==0)))
			{//if there is no digit before and also after the '.'
				ret="ERROR : INCORRECT '.' IN STRING";
				return ret;
			}
			else if(fflag==1)
			{
				ret="ERROR : INCORRECT '.' IN STRING";
				return ret;
			}
			else
			{
				fflag=1;
			}
		}
		else if(c=='+'||c=='-'||c=='*'||c=='/')
		{
			if(((i>0)&&(str[i-1]!=' '))||((i<len-1)&&(str[i+1]!=' ')))
			{
				//if either previous or next character is not a blank
				sprintf(ret,"ERROR: ILLEGAL PLACEMENT OF OPERATOR %c",c);
				return ret;
			}
			if(top<=0)
			{
				//if two former operands are not there
				sprintf(ret,"ERROR: NOT ENOUGH OPERANDS FOR %c",c);
				return ret;
			}
			else
			{
				//valid operator
				top=top-2+1;//pop 2 operands,push 1 operand;
			}
		}
		else if(c==' ')
		{
			//processing of token
			if(nflag==1)
			{
				top++;
				printf("%lf\n",token);
			}
			

			nflag=0;
			fflag=0;
			x=10;
			token=0;
		}
		else
		{
			ret="ERROR : ONLY NUMBERS AND OPERATORS ALLOWED";
			return ret;
		}
		
	}
	//processing of token
	if(nflag==1)
	{
		top++;
		printf("%lf\n",token);
	}

	if(top>0)
	{
		ret="ERROR : NOT ENOUGH OPERATORS";
		return ret;
	}
	
	
	return "";
}