
#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <cmath>
#include <QMessageBox>
#include <QString>
using namespace std;

class NodeNew {
public:
	//NodeNew()
	//{
	//	num = 0;
	//	op = '+';
	//}
	string num; //操作数
	char op; //操作符
	bool isop; //true表示操作符，false表示操作数
	float fvalue;

	NodeNew(string num, bool isop)
	{
		this->op = '+';
		this->num = num;
		this->isop = false;
	};
	NodeNew(char op, bool isop) {
		this->num = "";
		this->op = op;
		this->isop = true;

	}
};

class String2Formula
{
public:

	String2Formula();
	~String2Formula();
	bool isCSVFileName(std::string str);
	vector<NodeNew> midToBack(string mid);
	double getValue(vector<NodeNew> back);

	double getEleValue(std::string ele);
	bool isOperChar(char c);

	map<char,int> out;
	map<char, int> inside;
	vector<char> availChars; // 有效识别的操作字符
};

