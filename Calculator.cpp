#include <iostream>
#include <unordered_map>
#include <stack>
#include <string>
#include <cmath>


using namespace std;

//My LinkedList Class
template <typename T>
class Link
{
	T value;
	Link* next;
public:
	Link* getNext() { return next; }
	void setNext(Link* _next) { this->next = _next; }
	T getValue() { return value; }
	void setValue(T _value) { this->value = _value; }
	Link(T _value) { this->value = _value; }
	Link() {}
	~Link()
	{
		this->value = NULL;
		this->next = NULL;
	}

	static Link* GetNode(Link* firstLink, int pos)
	{
		Link* cur = firstLink;
		while (pos > 0)
		{
			if (cur->getNext() == NULL)
				return NULL;
			cur = cur->getNext();
			pos--;
		}
		return cur;
	}

	static int GetValue(Link* firstLink, int pos)
	{
		return GetNode(firstLink, pos)->getValue();
	}

	static void Push(Link* firstLink, T value)
	{
		Link* node = new Link;
		node->setValue(value);
		Link* cur = firstLink;
		while (cur->getNext())
			cur = cur->getNext();
		cur->setNext(node);
	}

	static void Add(Link** firstLink, T value, int pos)
	{
		Link* node = new Link(value);
		Link* headRoom = new Link(-1);
		headRoom->setNext(*firstLink);
		Link* cur = headRoom;
		while (cur->getNext() && pos > 0)
		{
			cur = cur->getNext();
			pos--;
		}
		node->setNext(cur->getNext());
		cur->setNext(node);
		*firstLink = headRoom->getNext();
	}

	static void Remove(Link** firstLink, int pos)
	{
		Link* headRoom = new Link(-1);
		headRoom->setNext(*firstLink);
		Link* cur = headRoom;
		while (cur->getNext() && pos > 0)
		{
			cur = cur->getNext();
			pos--;
		}
		cur->setNext(cur->getNext()->getNext());
		*firstLink = headRoom->getNext();
	}

	static string OutputList(Link* list)
	{
		Link* cur = list;
		string output = "";
		while (cur != NULL)
		{
			output += to_string(cur->getValue()) + " ";
			cur = cur->getNext();
		}
		return output.substr(0, output.size() - 1);
	}

	static Link* interweave(Link* L1, Link* L2, int step)
	{
		Link* L3 = new Link();
		int stepper = 0;
		while (L1)
		{
			if (stepper == step && L2)
			{
				Push(L3, L2->getValue());
				stepper = 0;
				L2 = L2->getNext();
			}
			else
			{
				Push(L3, L1->getValue());
				stepper++;
				L1 = L1->getNext();
			}
		}
		return L3->getNext();
	}
};

class Token //Token will be the base for Operator and Operand. 
{
public:
	virtual bool isOperator() = 0;
};

//WIP
class Operand : public Token //Double or string. getVal will return either the double or a lookup of the string in the public hash.
{
private:
	double value;
	string label;
	//bool valid;
public:
	bool isOperator() {return false; }
	double getValue();
	Operand(double val, string lab = "") { value = val; label = lab; }
	//Ability to subscribe to a hash to update or null value when it's changed?
};

class Operator : public Token //+,-,*,/,^,let; precedence is determined by two sets of bits, the first 29 are the number of parenthesis, the last 3 are the operator precedence
{
private:
	int precedence;
	char symbol;
public:
	bool isOperator() { return true; }
	int getPrecedence() { return precedence; }
	void increasePrecedence(int times=1) { precedence += 4 * times; }//By adding/subtracting 4 we adjust the parenthesis depth without affecting the operator precedence
	void decreasePrecedence(int times=1) { precedence -= 4 * times; }
	char getSymbol() { return symbol; }
	virtual Operand* calculate(Operand* a, Operand* b) = 0;
};

//WIP
class Assign : public Operator
{
private:
	int precedence = 0;
	const char symbol = '=';
public:
	Operand* Operator::calculate(Operand* a, Operand* b)
	{
		//TODO assign a to b
		return b;
		
	}
	Assign() {};
};

class Add : public Operator
{
private:
	int precedence = 1;
	const char symbol = '+';
public:
	Operand* Operator::calculate(Operand* a, Operand* b)
	{
		return new Operand(a->getValue() + b->getValue());
	}
	Add() {};
};

class Subtract : public Operator
{
private:
	int precedence = 1;
	const char symbol = '-';
public:
	Operand* Operator::calculate(Operand* a, Operand* b)
	{
		return new Operand(a->getValue() - b->getValue());
	}
	Subtract() {};
};

class Multiply : public Operator
{
private:
	int precedence = 2;
	const char symbol = '*';
public:
	Operand* Operator::calculate(Operand* a, Operand* b)
	{
		return new Operand(a->getValue() * b->getValue());
	}
	Multiply() {};
};

class Divide : public Operator
{
private:
	int precedence = 2;
	const char symbol = '/';
public:
	Operand* Operator::calculate(Operand* a, Operand* b)
	{
		if (b->getValue() == 0)
			throw new exception("Divide-By-Zero");
		return new Operand(a->getValue() / b->getValue());
	}
	Divide() {};
};

class Exponentiate : public Operator
{
private:
	int precedence = 3;
	const char symbol = '^';
public:
	Operand* Operator::calculate(Operand* a, Operand* b)
	{
		return new Operand(pow(a->getValue(), b->getValue()));
	}
	Exponentiate() {};
};

//Expand Token Factory to make Operands as well
//Figure out how to deal with negative numbers
class TokenFactory
{
public:
	/*static Token* create(string tok)
	{
		if()
	}*/
	static Operator* create(char op)
	{
		switch (op)
		{
		case '+':
			return new Add();
			break;
		case '-':
			return new Subtract();
			break;
		case '*':
			return new Multiply();
			break;
		case '/':
			return new Divide();
			break;
		case '^':
			return new Exponentiate();
			break;
		case '=':
			return new Assign();
			break;
		default:
			throw new exception("Bad Operator");
		}
	}
};

class Expression
{
	//Will have a Linked List for storing the postfix expression and a stack for working the equation
private:
	stack<Token*>* workingSpace;
	Link<Token*>* expression;
public:
	Expression()
	{
		expression = new Link<Token*>();
		workingSpace = new stack<Token*>();
	}

	Expression(Link<Token*>* postfix)
	{
		expression = postfix;
		workingSpace = new stack<Token*>();
	}

	void Add(Token* element)
	{
		Link<Token*>::Push(expression, element);
	}

	double getResult()
	{
		while (expression->getNext())
		{

		}
	}
};


class Calc
{
protected:
	unordered_map<string,double> Memory;

public:

};

int main()
{

	return 0;
}