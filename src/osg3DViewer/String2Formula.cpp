

#include "String2Formula.h"

#include "log.h"

// 分别为栈外和栈内操作符优先级
//map<char, int> out =
//{ {'#', 0}, {'(', 8}, {'^', 6}, {'*', 4}, {'/', 4}, {'%', 4},
//{'+', 2},{'-', 2},{')', 1} };
//
//map<char, int> inside = 
//{ {'#', 0}, {'(', 1}, {'^', 7}, {'*', 5}, {'/', 5}, {'%', 5},
//{'+', 3},{'-', 3},{')', 8} };


String2Formula::String2Formula()
{
	out['#'] = 0;
	out['('] = 8;
	out['^'] = 6;
	out['*'] = 4;
	out['/'] = 4;
	out['%'] = 4;
	out['+'] = 2;
	out['-'] = 2;
	out[')'] = 1;

	inside['#'] = 0;
	inside['('] = 1;
	inside['^'] = 7;
	inside['*'] = 5;
	inside['/'] = 5;
	inside['%'] = 5;
	inside['+'] = 3;
	inside['-'] = 3;
	inside[')'] = 8;

	availChars.push_back('#');
	availChars.push_back('(');
	availChars.push_back('^');
	availChars.push_back('*');
	availChars.push_back('/');
	availChars.push_back('%');
	availChars.push_back('+');
	availChars.push_back('-');
	availChars.push_back(')');

}

String2Formula::~String2Formula()
{

}


bool String2Formula::isCSVFileName(std::string str)
{
	bool is;
	if (str.find("csv") != std::string::npos)
	{
		is = true;
	}
	else
	{
		is = false;
	}
	return is;
}

bool String2Formula::isOperChar(char c)
{
	for (int i = 0; i < availChars.size();++i)
	{
		if (availChars[i]==c)
		{
			return true;
		}
	}

	return false;
}

double String2Formula::getEleValue(std::string ele)
{


	double value;
	return value;
}

// 中缀表达式->后缀表达式
vector<NodeNew> String2Formula::midToBack(string mid)
{
	vector<NodeNew> back;
	if (mid.size() == 0) return back;

	// 去空格
	string s;
	for (int i = 0; i < mid.length(); i++)
	{
		char c = mid[i];
		if (c != ' ')
			s.push_back(c);
	}

	// 处理负数
	if (s[0] == '-')
	{
		s.insert(0, "0");
	}
	for (int i = 0; i < s.size() - 1; i++)
	{
		if (s[i] == '(' && s[i + 1] == '-')
			s.insert(i + 1, "0");
	}
	mid = s;

	stack<char> op;
	op.push('#');
	bool flag = false; //记录上次存入的是否为数字
	for (int i = 0; i < mid.size(); i++)
	{
		std::string ele;
		if (!isOperChar(mid[i]))
		{ // 操作数不进栈
			if (flag == false)
			{
				flag = true;
				//int tmp = mid[i] - '0';
				ele.push_back(mid[i]);
				back.push_back(NodeNew(ele, false));
			}
			else
			{
				//back.back().num *= 10;
				//back.back().num += mid[i] - '0';
				back.back().num.push_back(mid[i]);
			}
		}
		else if (out[mid[i]] > inside[op.top()])
		{ // 操作符比栈顶操作符优先级高，则进栈
			flag = false;
			op.push(mid[i]);
		}
		else if (out[mid[i]] < inside[op.top()])
		{ // 操作符比栈顶操作符优先级低，则栈顶操作符出栈
			flag = false;
			back.push_back(NodeNew(op.top(), true));
			op.pop();
			i--;
		}
		else
		{ // 操作符与栈顶操作符优先级相同，则匹配括号
			flag = false;
			op.pop();
		}
	}
	while (op.top() != '#')
	{

		back.push_back(NodeNew(op.top(), true));

		op.pop();
	}
	return back;
}

// 求解后缀表达式
double String2Formula::getValue(vector<NodeNew> back)
{
	if (back.size() == 0)
		return 0;

	stack<double> nums;
	for (int i = 0; i < back.size();++i)
	{
		NodeNew e = back[i];
		if (!e.isop)
		{
			nums.push(e.fvalue);
		}
		else /*if (e.op != '('||e.op != ')') */
		{
			double b = nums.top();
			nums.pop();
			double a = nums.top();
			nums.pop();
			double res;
			switch (e.op)
			{
			case '+':
				res = a + b; break;
			case '-':
				res = a - b; break;
			case '*':
				res = a * b; break;
			case '/':
				if (fabs(b) < 0.0000001)
				{	
					QMessageBox::warning(NULL, "warning", QString::fromLocal8Bit("存在分母为0！"), QMessageBox::Yes);
					LogInfo(QString("存在分母为0 "));
					return 0.0;
				
				}
				res = a / b; break;
			case '^':
				res = pow(a, b); break;
			case '%':
				res = int(a) % int(b); break;
			default:
				break;
			}
			nums.push(res);
		}
	}
	return nums.top();
}