
#include<string>
#include<vector>
using namespace std;

typedef enum
{
	ELSE=0,IF,INT,RETURN,VOID,WHILE,PLUS,MINUS,TIMES,DIVIDE,LT,LEQ,GT,GEQ,EQ,NEQ,ASSIGN,SEMI,COMMA,LPAREN,RPAREN,
	LMBRACKET,RMBRACKET,LBBRACKET,RBBRACKET,LCOMMENT,RCOMMENT,NUM,ID,ERROR,ENDFILE
}TokenType;

typedef enum
{
	START,INNUM,INID,INDBSYM,DONE
}DFAState;

class Token
{
public:
	TokenType tokenType;
	string token;
	int lineNo;
};

class Scan
{
public:
	void readSourceCode(string file);
	void deleteComments();
	void scan();
	void outPut(string file);
	Token getNextToken();
	Scan() :charIndex(0), lineNo(1),tokenIndex(0) {}
private:
	string sourceCode;
	vector<Token> tokenList;
	int charIndex;
	int lineNo;
	int tokenIndex;
	string tokenType[31] = 
	{
		"ELSE","IF","INT","RETURN","VOID","WHILE","PLUS","MINUS","TIMES","DIVIDE","LT","LEQ","GT","GEQ","EQ","NEQ",
		"ASSIGN","SEMI","COMMA","LPAREN","RPAREN","LMBRACKET","RMBRACKET","LBBRACKET","RBBRACKET","LCOMMENT","RCOMMENT",
		"NUM","ID","ERROR","ENDFILE"
	};;

	char getNextChar();
	DFAState charType(char ch);
	TokenType getTokenType(string str);
};
