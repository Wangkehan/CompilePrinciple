#include"parse.h"

void Parse::divide_words(string grammar, set<string>&nt, set<string>&symbol)
{
	if (grammar.size() == 0)
		return;
	for (size_t i = 0; i < grammar.length() - 1; ++i) 
	{
		if (grammar[i] == '-' && grammar[i + 1] == '>') 
		{
			string left = grammar.substr(0, i);
			string right = grammar.substr(i + 2, grammar.length() - 1);
			nt.insert(left);
			symbol.insert(left);
			if (start_symbol.size() == 0)
				start_symbol = left;
			vector<string> or_words;
			SplitString(right, or_words, "|");
			for (string s : or_words)
			{
				Prod p;
				p.non_term = left;
				SplitString(s, p.and_words, " ");
				productions.push_back(p);

				for (string ss : p.and_words)
				{
					if (ss != "#")
					{
						symbol.insert(ss);
					}
				}
			}
			break;
		}
	}
}

void Parse::find_first() {
	vector<int>before;
	for (auto it = first.begin(); it != first.end(); it++)
	{
		before.push_back(it->second.size());
	}
	for (auto pro : productions)
	{
		int k = 1;
		bool cont = true;
		while (cont&&k <= pro.and_words.size())
		{
			if (is_non_terminal(pro.and_words[k - 1]))
			{
				bool flag = false;
				for (string s : first.find(pro.and_words[k - 1])->second)
				{
					if (s != "#")
						first.find(pro.non_term)->second.insert(s);
					else
						flag = true;
				}
				cont = flag;
			}
			else
			{
				first.find(pro.non_term)->second.insert(pro.and_words[k - 1]);
				cont = false;
			}
			++k;
		}
		if (cont)
			first.find(pro.non_term)->second.insert("#");
	}
	vector<int>after;
	for (auto it = first.begin(); it != first.end(); it++)
	{
		after.push_back(it->second.size());
	}
	bool f = false;
	for (int i = 0; i < before.size(); ++i)
	{
		if (before[i] != after[i])
		{
			f = true;
			break;
		}
	}
	if (f)
		find_first();
}

Parse::Parse(){}

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}
bool Parse::is_non_terminal(string str)
{
	bool flag = false;
	for (auto j = non_terminal.begin(); j != non_terminal.end(); j++)
	{
		if (str == *j)
		{
			flag = true;
			break;
		}
	}
	return flag;
}
void Parse::print()
{
	cout << "---------------------------------------\nFirst:\n";
	for (string s : non_terminal)
	{
		cout << s << "\t\t{ ";
		auto fs = first.find(s)->second;
		for (auto it = fs.begin(); it != fs.end();)
		{
			cout << *it;
			if (++it != fs.end())
				cout << ",";
		}
		cout << " }\n";
	}
	cout << "----------------------------------------\nFollow:\n";
	for (string s : non_terminal)
	{
		cout << s << "\t\t{ ";
		auto fs = follow.find(s)->second;
		for (auto it = fs.begin(); it != fs.end();)
		{
			cout << *it;
			if (++it != fs.end())
				cout << ",";
		}
		cout << " }\n";
	}
	cout << "----------------------------------------\nItems:\n";
	for (int i = 0; i < itemList.size(); ++i)
	{
		cout << "I" << i << ":\n";
		itemList[i].print();
	}

	cout << "\n输出SLR分析表到a.csv文件中.\n";
	ofstream os("a.csv");
	os << ",";
	for (string s : terminal)
	{
		if(s==",")
			os << "\""<<s << "\",";
		else
			os << s << ",";
	}
	os << "#,";
	for (string s : non_terminal)
	{
		os << s << ",";
	}
	os << "\n";
	for (int i = 0; i < actionTable.size(); ++i)
	{
		os << i << ",";
		for (int j = 0; j < actionTable[i].size(); ++j)
		{
			switch (actionTable[i][j].state)
			{
			case ACC:
				os << "acc,";
				break;
			case _ERROR:
				os << ",";
				break;
			case SHIFTIN:
				os << "s" << actionTable[i][j].extra << ",";
				break;
			case REDUCTION:
				os << "r" << actionTable[i][j].extra << ",";
				break;
			default:
				break;
			}
		}
		for (int j = 0; j < gotoTable[i].size(); ++j)
		{
			if (gotoTable[i][j] != -1)
				os << gotoTable[i][j] << ",";
			else
				os << ",";
		}
		os << "\n";
	}
}

void Parse::find_follow()
{
	vector<int>before;
	for (auto it = follow.begin(); it != follow.end(); it++)
	{
		before.push_back(it->second.size());
	}
	for (auto pro : productions)
	{
		for (int i = 0; i < pro.and_words.size(); ++i)
		{
			if (is_non_terminal(pro.and_words[i]))
			{
				if (i < pro.and_words.size() - 1)
				{
					if (is_non_terminal(pro.and_words[i + 1]))
					{
						bool flag = false;
						set<string> s = first.find(pro.and_words[i + 1])->second;
						for (string ss : s)
						{
							if (ss != "#")
							{
								follow.find(pro.and_words[i])->second.insert(ss);
							}
							else
								flag = true;
						}
						if (flag)
						{
							for (string ss : follow.find(pro.non_term)->second)
							{
								follow.find(pro.and_words[i])->second.insert(ss);
							}
						}
					}
					else
					{
						follow.find(pro.and_words[i])->second.insert(pro.and_words[i + 1]);
					}
				}
				else
				{
					for (string ss : follow.find(pro.non_term)->second)
					{
						follow.find(pro.and_words[i])->second.insert(ss);
					}
				}
			}
		}
	}
	vector<int>after;
	for (auto it = follow.begin(); it != follow.end(); it++)
	{
		after.push_back(it->second.size());
	}
	bool f = false;
	for (int i = 0; i < before.size(); ++i)
	{
		if (before[i] != after[i])
		{
			f = true;
			break;
		}
	}
	if (f)
		find_follow();
}

Closure Parse::calc_closure(Closure clo)
{
	int before = clo.prods.size();
	Closure _clo = clo;
	for (Item it : clo.prods)
	{
		if (it.dot_pos < it.prod.and_words.size())
		{
			if (is_non_terminal(it.prod.and_words[it.dot_pos]))
			{
				for (auto pro : productions)
				{
					if (pro.non_term == it.prod.and_words[it.dot_pos])
					{
						Item i;
						i.dot_pos = 0;
						i.prod = pro;
						_clo.prods.insert(i);
					}
				}
			}
		}
	}
	int after = _clo.prods.size();
	if (after != before)
	{
		return calc_closure(_clo);
	}
	else
		return _clo;
}

Closure Parse::go(Closure clo, string x)
{
	Closure j;
	for (Item it : clo.prods)
	{
		if (it.dot_pos < it.prod.and_words.size())
		{
			if (it.prod.and_words[it.dot_pos] == x)
			{
				it.dot_pos++;
				j.prods.insert(it);
			}
		}
	}
	return calc_closure(j);
}

void Parse::make_item()
{
	int before = itemList.size();
	vector<string>symbol;
	symbol.insert(symbol.end(), non_terminal.begin(), non_terminal.end());
	symbol.insert(symbol.end(), terminal.begin(), terminal.end());
	for (int i = 0; i < itemList.size(); ++i)
	{
		for (string str : symbol)
		{
			Closure clo = go(itemList[i], str);
			if (clo.prods.size() > 0)
			{
				bool flag = false;
				for (int j = 0; j < itemList.size(); ++j)
				{
					if (clo == itemList[j])
					{
						Transfer t;
						t.iBefore = i;
						t.iAfter = j;
						t.symbol = str;
						goList.insert(t);
						flag = true;
						break;
					}
				}
				if (!flag)
				{
					itemList.push_back(clo);
					Transfer t;
					t.iBefore = i;
					t.iAfter = itemList.size() - 1;
					t.symbol = str;
					goList.insert(t);
				}
			}
		}
	}
	int after = itemList.size();
	if (before != after)
	{
		make_item();
	}
}

bool Parse::make_table()
{
	for (int i = 0; i < itemList.size(); ++i)
	{
		vector<ActionItem> vec;
		for (int j = 0; j < terminal.size()+1; ++j)
		{
			ActionItem ai;
			ai.state = _ERROR;
			vec.push_back(ai);
		}
		actionTable.push_back(vec);

		vector<int> _vec;
		for (int k = 0; k < non_terminal.size(); ++k)
		{
			_vec.push_back(-1);
		}
		gotoTable.push_back(_vec);
	}
	for (int i = 0; i < itemList.size(); ++i)
	{
		for (auto it = itemList[i].prods.begin(); it != itemList[i].prods.end(); it++)
		{
			if (it->dot_pos < it->prod.and_words.size() && it->prod.and_words[it->dot_pos] != "#")
			{
				if (!is_non_terminal(it->prod.and_words[it->dot_pos]))
				{
					for (auto it_go = goList.begin(); it_go != goList.end(); it_go++)
					{
						if (it_go->iBefore == i&&it_go->symbol == it->prod.and_words[it->dot_pos])
						{
							int pos = get_index(terminal, it->prod.and_words[it->dot_pos]);
							if (actionTable[i][pos].state == SHIFTIN&&actionTable[i][pos].extra != it_go->iAfter)//移进移进冲突
							{
								return false;
							}
							actionTable[i][pos].state = SHIFTIN;
							actionTable[i][pos].extra= it_go->iAfter;
						}
					}
				}
			}
			else
			{
				if (it->prod.non_term == start_symbol)
				{
					actionTable[i][actionTable[i].size()-1].state = ACC;
				}
				else
				{
					int index;
					for (int j = 0; j < productions.size(); ++j)
					{
						if (it->prod == productions[j])
						{
							index = j;
							break;
						}
					}
					for (string s : follow.find(it->prod.non_term)->second)
					{
						int pos;
						if (s == "#")
							pos = actionTable[i].size() - 1;
						else
							pos = get_index(terminal, s);
						if (actionTable[i][pos].state == REDUCTION&&actionTable[i][pos].extra != index)  //归约归约冲突
						{
							return false;
						}
						if (actionTable[i][pos].state == _ERROR)
						{
							actionTable[i][pos].state = REDUCTION;
							actionTable[i][pos].extra = index;
						}
					}
				}
			}
		}
	}

	for (auto it = goList.begin(); it != goList.end(); it++)
	{
		if (is_non_terminal(it->symbol))
		{
			gotoTable[it->iBefore][get_index(non_terminal, it->symbol)] = it->iAfter;
		}
	}
	return true;
}

int Parse::get_index(const vector<string>&vec, const string&str)
{
	for (int i = 0; i < vec.size(); ++i)
	{
		if (vec[i] == str)
			return i;
	}
	return -1;
}

void Parse::analyse()
{
	stack<int>state;  //状态栈
	stack<TreeNode*>t;  //符号栈
	state.push(0);
	string _start_symbol;
	for (int i = 0; i < productions.size(); ++i)
	{
		if (productions[i].non_term == start_symbol)
		{
			_start_symbol = productions[i].and_words[0];
		}
	}
	
	bool flag = false;
	Token tLastUnused;
	while (true)
	{
		Token token;
		if (flag)
		{
			token = tLastUnused;
		}
		else
		{
			token= scan.getNextToken();
		}
		int s = state.top();
		string input;
		switch (token.tokenType)
		{
		case ENDFILE:
			input = "#";
			break;
		case ID:
			input = "ID";
			break;
		case NUM:
			input = "NUM";
			break;
		case ERROR:
			exit(-1);
			break;
		default:
			input = token.token;
			break;
		}
		int index;
		if (input == "#")
		{
			index = actionTable[0].size() - 1;
		}
		else
		{
			index = get_index(terminal, input);;
		}
		ActionItem ai = actionTable[s][index];
		TreeNode*p = NULL;
		switch (ai.state)
		{
		case SHIFTIN:
			state.push(ai.extra);
			p = new TreeNode(input,NULL);
			t.push(p);
			flag = false;
			break;
		case REDUCTION:
			p = new TreeNode(productions[ai.extra].non_term, NULL);
			for (int i = 0; i < productions[ai.extra].and_words.size(); ++i)
			{
				if (productions[ai.extra].and_words[i] == "#")
				{
					TreeNode*ptn = new TreeNode("#", p);
					p->pctns.push_back(ptn);
					continue;
				}
				state.pop();
				TreeNode*pst = t.top();
				t.pop();
				pst->pptn = p;
				p->pctns.push_back(pst);
			}
			state.push(gotoTable[state.top()][get_index(non_terminal, productions[ai.extra].non_term)]);
			t.push(p);

			if (productions[ai.extra].non_term == _start_symbol)
			{
				stack<int>s;  
				stack<TreeNode*>_t;  
				state.swap(s);
				t.swap(_t);
				state.push(0);
				TreeNode *tn = new TreeNode(start_symbol, NULL);
				tn->pctns.push_back(p);
				p->pptn = tn;
				trees.push_back(tn);

				if (token.tokenType == ENDFILE)
				{
					cout << "语法分析结束.\n";
					outPut();
					return;
				}
			}

			flag = true;
			tLastUnused = token;
			break;
		case ACC:
			break;
		case _ERROR:
			cout << "语法分析出错，行号:" << token.lineNo << ".\n";
			exit(-1);
			break;
		default:
			break;
		}
	}
}

void Parse::parse(string fp)
{
	scan.readSourceCode(fp);
	scan.deleteComments();
	scan.scan();
	scan.outPut("token.txt");
	cout << "词法分析结束.\n\n";

	cout << "正在读取文法规则...\n";
	const char* filename = "wenfa.txt";
	ifstream inFile(filename);
	if (!inFile)
	{
		cout << "Filed to open file " << filename << endl;
		exit(-1);
	}
	string st;
	set<string> nt, symbol;
	while (!inFile.eof())
	{
		getline(inFile, st);
		divide_words(st, nt, symbol);
	}

	set_difference(symbol.begin(), symbol.end(), nt.begin(), nt.end(), back_inserter(terminal));
	copy(nt.begin(), nt.end(), back_inserter(non_terminal));
	inFile.close();
	for (auto i = non_terminal.begin(); i != non_terminal.end(); ++i)
	{
		set<string>tmp, ttmp;
		first.insert(make_pair(*i, tmp));
		if (*i == start_symbol)
		{
			ttmp.insert("#");
		}
		follow.insert(make_pair(*i, ttmp));
	}
	cout << "计算First集合.\n";
	find_first();
	cout << "计算Follow集合.\n";
	find_follow();
	cout << "计算LR(0)项目集.\n";
	for (auto pro : productions)
	{
		if (pro.non_term == start_symbol)
		{
			Closure clo;
			Item i;
			i.dot_pos = 0;
			i.prod = pro;
			clo.prods.insert(i);
			itemList.push_back(calc_closure(clo));
			break;
		}
	}
	make_item();
	cout << "生成SLR(1)分析表.\n";
	if (make_table())
		cout << "该文法属于SLR(1)文法.\n";
	else
	{
		cout << "该文法不属于SLR(1)文法.\n";
		exit(-1);
	}
	print();

	analyse();
}

void Parse::outPut()
{
	string file = "tree.txt";
	cout << "输出语法树到" << file << ".\n";
	ofstream os(file);
	for (int i = 0; i < trees.size(); ++i)
	{
		TreeNode*tn = trees[i];
		dfs(tn, 0,os);
	}
}

void Parse::dfs(TreeNode*tn, int n,ofstream&os)
{
	for (int i = 0; i < n; ++i)
	{
		os << "    ";
	}
	os << tn->str << "\n";
	++n;
	for (int i = tn->pctns.size()-1; i >=0; --i)
	{
		dfs(tn->pctns[i], n,os);
	}
}