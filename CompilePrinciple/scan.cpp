#include"scan.h"
#include<fstream>
#include<iostream>

char Scan::getNextChar()
{
	if (charIndex == sourceCode.size())
		return '\0';
	return sourceCode[charIndex++];
}

Token Scan::getNextToken()
{
	if (tokenIndex < tokenList.size())
	{
		return tokenList[tokenIndex++];
	}
	else
	{
		exit(-1);
	}
}

void Scan::deleteComments()
{
	cout << "正在删除注释...\n";
	int state = 1;
	char ch;
	string tmp;
	bool flag = false;
	while (true)
	{
		ch = getNextChar();
		if (ch == '\0')
			break;
		if (state == 1)
		{
			if (ch == '/')
				state = 2;
			else
				tmp += ch;
		}
		else if (state == 2)
		{
			if (ch == '*')
			{
				state = 3;
				flag = true;
			}
			else
			{
				state = 1;
				tmp += "/" + ch;
			}
		}
		else if (state == 3)
		{
			if (ch == '*')
				state = 4;
		}
		else if (state == 4)
		{
			if (ch == '/')
				state = 5;
			else
				state = 3;
		}
		if (state == 5)
		{
			flag = false;
			state = 1;
		}
	}
	if (flag)
	{
		cout << "删除注释出错，没有找到注释结束符.\n";
		exit(-1);
	}
	sourceCode = tmp;
	charIndex = 0;
}

void Scan::readSourceCode(string file)
{
	cout << "正在读取源文件" << file<<"...\n";
	charIndex = 0;
	lineNo = 1;
	tokenIndex = 0;
	tokenList.clear();
	sourceCode.clear();
	ifstream is(file);
	if (!is)
	{
		cerr << "open file " << file << " failed.\n";
		exit(-1);
	}
	while (!is.eof())
	{
		string tmp;
		std::getline(is, tmp);
		sourceCode += tmp+"\n";
	}
	sourceCode.pop_back();
}

void Scan::scan()
{
	cout << "正在进行词法分析...\n";
	bool doubleSym = false;
	int state = START;
	lineNo = 1;
	char ch;
	string str;
	while (state<6)
	{
		ch = getNextChar();

		if (START == state)
		{
			state = charType(ch);
			if (state != START)
				str += ch;
		}
		else if (INNUM == state)
		{
			state = charType(ch);
			
			if (state != INNUM)
				state = DONE;
			else
				str += ch;
		}
		else if (INID == state)
		{
			state = charType(ch);
			if (state != INID)
				state = DONE;
			else
				str += ch;
		}
		else if (INDBSYM == state)
		{
			if ('=' == ch)
			{
				str += ch;
				doubleSym = true;
			}
			else
				doubleSym = false;
			state = DONE;
		}
		if (DONE == state)
		{
			Token t;
			t.lineNo = lineNo;
			t.token = str;
			t.tokenType = getTokenType(str);
			if (t.tokenType == ERROR)
			{
				cout << "词法扫描出错，未识别的符号:" << str << ".行号:" << lineNo << ".\n";
				exit(-1);
			}
			if(t.tokenType!= ENDFILE)
				tokenList.push_back(t);

			int lastState = charType(str[str.length() - 1]);
			if (lastState == INNUM || lastState == INID || (lastState == INDBSYM && doubleSym == false))
			{
				--charIndex;
				if (sourceCode[charIndex] == '\n')
					--lineNo;
			}
			str = "";
			state = START;
			if (doubleSym == true)
				doubleSym = false;
		}

		if ('\0' == ch)
		{
			Token t;
			t.lineNo = lineNo;
			t.token = "";
			t.tokenType = ENDFILE;
			tokenList.push_back(t);
			break;
		}

		if (ch == '\n') {
			++lineNo; 
		}
	}

}

DFAState Scan::charType(char ch)
{
	if (ch == ' ' || ch == '\n' || ch == '\t')
		return START;
	if (ch >= '0'&&ch <= '9')
		return INNUM;
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
		return INID;
	if (ch == '<' || ch == '>' || ch == '!' || ch == '=')
		return INDBSYM;
	return DONE;
}

TokenType Scan::getTokenType(string str)
{
	if (str == "else")
		return ELSE;
	if (str == "if")
		return IF;
	if (str == "int")
		return INT;
	if (str == "return")
		return RETURN;
	if (str == "void")
		return VOID;
	if (str == "while")
		return WHILE;
	if (str == "+")
		return PLUS;
	if (str == "-")
		return MINUS;
	if (str == "*")
		return TIMES;
	if (str == "/")
		return DIVIDE;
	if (str == "<")
		return LT;
	if (str == "<=")
		return LEQ;
	if (str == ">")
		return GT;
	if (str == ">=")
		return GEQ;
	if (str == "==")
		return EQ;
	if (str == "!=")
		return NEQ;
	if (str == "=")
		return ASSIGN;
	if (str == ";")
		return SEMI;
	if (str == ",")
		return COMMA;
	if (str == "(")
		return LPAREN;
	if (str == ")")
		return RPAREN;
	if (str == "[")
		return LMBRACKET;
	if (str == "]")
		return RMBRACKET;
	if (str == "{")
		return LBBRACKET;
	if (str == "}")
		return RBBRACKET;
	if (str == "/*")
		return LCOMMENT;
	if (str == "*/")
		return RCOMMENT;
	if (str[0] == '\0')
		return ENDFILE;
	bool flag = true;
	for (char c : str)
	{
		if (c > '9' || c < '0')
		{
			flag = false;
			break;
		}
	}
	if (flag)
		return NUM;
	flag = true;
	for (char c : str)
	{
		if ((c>'z'||c<'a')&&(c>'Z' || c<'A'))
		{
			flag = false;
			break;
		}
	}
	if (flag)
		return ID;
	return ERROR;
}

void Scan::outPut(string file)
{
	cout << "输出词法单元到" << file << "中.\n";
	ofstream os(file);
	if (!os)
	{
		cerr << "open file " << file << " failed.\n";
		return;
	}
	for (Token t: tokenList)
	{
		os << "type:" << tokenType[t.tokenType] << "\t\ttoken:" << t.token << "\t\tlineNo:" << t.lineNo << "\n";
	}
}
