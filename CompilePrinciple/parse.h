#pragma once

#include<algorithm>
#include<fstream>
#include<map>
#include<set>
#include<string>
#include<iostream>
#include<vector>
#include<stack>
#include"scan.h"

using namespace std;
//'#'Ϊ�ķ��е�����������'@'Ϊ��Ŀ���е�Բ��

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

enum ACTION{SHIFTIN,REDUCTION,ACC,_ERROR};//ACTION��״̬
struct Prod
{//����ʽ�ṹ��
	string non_term;
	vector<string>and_words;
	bool operator ==(const Prod&right)const
	{
		if (non_term == right.non_term&&and_words == right.and_words)
			return true;
		return false;
	}
};
struct Transfer
{//��Ŀ��ת�ƹ�ϵ
	int iBefore, iAfter;
	string symbol;
};
struct ActionItem
{//ACTION����
	ACTION state;
	int extra;  //��stateΪerror��accʱ��Ч
};
class Item
{//��Ŀ
public:
	Prod prod;
	int dot_pos;
	bool operator ==(const Item&i)const
	{
		if (dot_pos == i.dot_pos&&prod==i.prod)
			return true;
		return false;
	}

	void print()const 
	{
		cout << prod.non_term << "->";
		for (int i = 0; i < dot_pos; ++i)
		{
			cout << prod.and_words[i] << " ";
		}
		cout << "@ ";
		for (int i = dot_pos; i < prod.and_words.size(); ++i)
		{
			cout << prod.and_words[i] << " ";
		}
		cout << "\n";
	}
};
struct CompareItem
{
	bool operator()(const Item&left, const Item&right)
	{
		if (left.prod.non_term < right.prod.non_term)
			return true;
		if (left.prod.non_term > right.prod.non_term)
			return false;
		if (left.prod.and_words < right.prod.and_words)
			return true;
		if (left.prod.and_words > right.prod.and_words)
			return false;
		if (left.dot_pos < right.dot_pos)
			return true;
		if (left.dot_pos > right.dot_pos)
			return false;
		return false;
	}
};
struct CompareTransfer
{
	bool operator()(const Transfer&left, const Transfer&right)
	{
		if (left.iBefore < right.iBefore)
			return true;
		if (left.iBefore > right.iBefore)
			return false;
		if (left.iAfter < right.iAfter)
			return true;
		if (left.iAfter > right.iAfter)
			return false;
		if (left.symbol < right.symbol)
			return true;
		if (left.symbol > right.symbol)
			return false;
		return false;
	}
};
class Closure
{//�հ�
public:
	set<Item,CompareItem>prods;

	bool operator ==(const Closure& clo)const
	{
		if (prods.size() != clo.prods.size())
			return false;
		for (auto it_l = prods.begin(), it_r = clo.prods.begin(); it_l != prods.end(); it_l++, it_r++)
		{
			if (!(*it_l == *it_r))
				return false;
		}
		return true;
	}
	void print()
	{
		for (auto it = prods.begin(); it != prods.end(); it++)
		{
			it->print();
		}
	}
};
class TreeNode
{
public:
	string str;
	TreeNode*pptn;
	vector<TreeNode*>pctns;
	TreeNode(string s, TreeNode*p) { str = s; pptn = p; }
};
class Parse
{
public :
	Parse();

	void parse(string fp);

	void print();
private:
	map<string, set<string>>first;  //first����
	map<string, set<string>>follow;  //follow����

	vector<string> non_terminal;   //���ս������
	vector<string> terminal;     //�ս������
	vector<Prod>productions;   //����ʽ����
	
	vector<Closure>itemList;   //��Ŀ������
	set<Transfer,CompareTransfer>goList;    //��Ŀ��֮���ת�ƹ�ϵ
	vector<vector<ActionItem>> actionTable;  //Action��
	vector<vector<int>> gotoTable;   //Goto��

	string start_symbol;  //��ʼ����

	Scan scan;  //�ʷ�ɨ����

	vector<TreeNode*>trees;  //�﷨��

	void divide_words(string grammer,set<string>&nt,set<string>&symbol);
	bool is_non_terminal(string str);
	Closure calc_closure(Closure clo);
	Closure go(Closure clo, string x);

	void find_first();
	void find_follow();
	void make_item();
	bool make_table();

	void analyse();
	void outPut();
	void dfs(TreeNode*tn, int n,ofstream&os);

	int get_index(const vector<string>&vec, const string&str);
};