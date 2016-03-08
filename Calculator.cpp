#include <iostream>
#include <unordered_map>
#include <stack>
#include <queue>
#include <string>
#include <cmath>
#include <exception>

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

class Calc//Will accept the input and if the input begins with 'let' Calc will evaluate the right of the '=' and assign it to the variable. If input begins with 'quit' will exit.
{

	class Token //Token will be the base for Operator and Operand. 
	{
	public:
		virtual bool isOperator() = 0;
	};

	class Operand : public Token //Double or string. getVal will return either the double or a lookup of the string in the public hash.
	{
	private:
		double value;
		string label = "";
		unordered_map<string, double>* memPtr;
		//bool valid;
	public:
		bool isOperator() { return false; }
		double getValue()
		{
			if (label != "")
				return getValFromMem();
			else
				return value;
		}
		string getLabel() { return label; }
		Operand(double val, string lab = "", unordered_map<string, double>* _memPtr = nullptr) { value = val; label = lab; memPtr = _memPtr; }
		double getValFromMem()
		{
			if (memPtr->find(label) == memPtr->end())
				throw new exception("Undefined variable");
			return memPtr->at(label);
		}
		//Ability to subscribe to a hash to update or null value when it's changed?
	};

	class Operator : public Token //+,-,*,/,^,let; precedence is determined by two sets of bits, the first 29 are the number of parenthesis, the last 3 are the operator precedence
	{
	protected:
		int precedence;
		char symbol;
	public:
		bool isOperator() { return true; }
		int getPrecedence() { return precedence; }
		void increasePrecedence(int times = 1) { precedence += 4 * times; }//By adding/subtracting 4 we adjust the parenthesis depth without affecting the operator precedence
		char getSymbol() { return symbol; }
		virtual Operand* calculate(Operand* a, Operand* b) = 0;
	};

	class Assign : public Operator
	{
	private:
		unordered_map<string, double>* memPtr;
	public:
		Operand* Operator::calculate(Operand* a, Operand* b)
		{
			memPtr->insert_or_assign(b->getLabel(), a->getValue());
			return b;
		}
		Assign(unordered_map<string, double>* _memPtr) { memPtr = _memPtr; precedence = 0; symbol = '='; }
	};

	class Add : public Operator
	{
	public:
		Operand* Operator::calculate(Operand* a, Operand* b)
		{
			return new Operand(a->getValue() + b->getValue());
		}
		Add() { precedence = 1; symbol = '+'; };
	};

	class Subtract : public Operator
	{
	public:
		Operand* Operator::calculate(Operand* a, Operand* b)
		{
			return new Operand(b->getValue() - a->getValue());
		}
		Subtract() { precedence = 1; symbol = '-'; };
	};

	class Multiply : public Operator
	{
	public:
		Operand* Operator::calculate(Operand* a, Operand* b)
		{
			return new Operand(a->getValue() * b->getValue());
		}
		Multiply() { precedence = 2; symbol = '*'; };
	};

	class Divide : public Operator
	{
	public:
		Operand* Operator::calculate(Operand* a, Operand* b)
		{
			if (a->getValue() == 0)
				throw new exception("Divide-By-Zero");
			return new Operand(b->getValue() / a->getValue());
		}
		Divide() { precedence = 2; symbol = '/'; };
	};

	class Exponentiate : public Operator
	{
	public:
		Operand* Operator::calculate(Operand* a, Operand* b)
		{
			return new Operand(pow(b->getValue(), a->getValue()));
		}
		Exponentiate() { precedence = 3; symbol = '^'; };
	};

	class TokenFactory
	{
	private:
		unordered_map<string, double>* memPtr;
	public:
		Token* create(string tok)
		{
			if (tok.size() == 1)
				switch (tok[0])
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
					return new Assign(memPtr);
					break;
				}

			const char* cstr = tok.c_str();
			char* nptr;
			double val = strtod(cstr, &nptr);

			if (val == 0 && nptr == cstr)//tok was not a valid double or operator, so it must be a variable
				return new Operand(val, tok, memPtr);
			else
				return new Operand(val);
		}
		TokenFactory(unordered_map<string, double>* memory) { memPtr = memory; }
	};

	class Expression
	{
		//Will have a queueed List for storing the postfix expression and a stack for working the equation
	private:
		stack<Token*>* workingSpace;
		queue<Token*> expression;
		string infix;
		unordered_map<string, double>* memory;
	public:

		Expression(string _infix, unordered_map<string, double>* _memory)
		{
			infix = _infix;
			memory = _memory;
			expression = infixToPostfix();
		}

		queue<Token*> infixToPostfix()
		{
			const string validNums = ".0123456789";
			const string validOps = "/*-+^=";
			TokenFactory* fac = new TokenFactory(memory);
			queue<Token*> exp;
			string tokenTemp = "";
			int parenLevel = 0;
			//Go through and mark the precidences, also convert to queueedList<Token>

			for (int i = 0; i < infix.size(); i++)
			{
				if (infix[i] == ' ')
					continue;
				if (infix[i] == '(')
				{
					parenLevel++;
					continue;
				}
				if (infix[i] == ')')
				{
					parenLevel--;
					continue;
				}
				if (validOps.find(infix[i]) != string::npos)//If the character is an operator
				{
					Token* tok = fac->create(tokenTemp);
					exp.push(tok);
					tokenTemp = "";
					tok = fac->create(string(1, infix[i]));
					for (int j = 0; j < parenLevel; j++)
						((Operator*)tok)->increasePrecedence();
					exp.push(tok);
					continue;
				}
				else if (validNums.find(infix[i]) != string::npos)//If the character is part of a number
				{
					tokenTemp = tokenTemp + infix[i];
					continue;
				}
				else//The character is part of a variable name
				{
					tokenTemp = tokenTemp + infix[i];
					continue;
				}
			}
			Token* tok = fac->create(tokenTemp);
			exp.push(tok);
			tokenTemp = "";

			stack<Token*> opStack;
			queue<Token*> opQueue;

			for (Token* cur = exp.front(); exp.size() > 0; exp.pop())
			{
				cur = exp.front();
				if (!cur->isOperator())
				{
					opQueue.push(cur);
					continue;
				}
				Operator* curOp = (Operator*)cur;
				while (curOp != NULL)
				{
					if (opStack.empty())
					{
						opStack.push(cur);
						break;
					}
					Operator* topOp = (Operator*)opStack.top();

					if (curOp->getPrecedence() > topOp->getPrecedence())
					{
						opStack.push(cur);
						break;
					}
					if (curOp->getPrecedence() == topOp->getPrecedence())
					{
						if (topOp->getSymbol() != '^')
						{
							opQueue.push(opStack.top());
							opStack.pop();
						}
						opStack.push(cur);
						break;
					}
					if (curOp->getPrecedence() < topOp->getPrecedence())
					{
						opQueue.push(opStack.top());
						opStack.pop();
						continue;
					}
				}
			}
			while (!opStack.empty())
			{
				opQueue.push(opStack.top());
				opStack.pop();
			}
			return opQueue;
		}

		Operand* getResult()
		{
			workingSpace = new stack<Token*>();
			while (!expression.empty())
			{
				workingSpace->push(expression.front());
				expression.pop();
				if (workingSpace->top()->isOperator())
				{
					Operator* func = (Operator*)workingSpace->top();
					workingSpace->pop();
					Operand* numA = (Operand*)workingSpace->top();
					workingSpace->pop();
					Operand* numB = (Operand*)workingSpace->top();
					workingSpace->pop();

					Operand* result = func->calculate(numA, numB);
					workingSpace->push(result);
				}
			}
			Operand* result = (Operand*)(workingSpace->top());
			return result;
		}
	};

private:
	Expression* exp;
protected:
	unordered_map<string,double>* memory;
public:
	string getInput() 
	{
		string in = "";
		getline(cin, in);
		return in;
	}

	void parseInput(string input)
	{
		exp = new Expression(input, memory);
	}

	void start()
	{
		string in = getInput();;
		while (in != "quit")
		{
			try
			{
				Operand* result;
				if (in.substr(0, 3) == "let")
				{
					int splitPoint = in.find('=');
					string varName = in.substr(3, splitPoint - 3);
					int spaceTrimL = varName.find_first_not_of(' ');
					int spaceTrimR = varName.find_last_not_of(' ');
					varName = varName.substr(spaceTrimL, (spaceTrimR - spaceTrimL+1));
					string expressionStr = in.substr(splitPoint+1);
					parseInput(expressionStr);
					result = exp->getResult();
					memory->insert_or_assign(varName, result->getValue());
				}
				else
				{
					parseInput(in);
					result = exp->getResult();
				}
				cout << result->getValue() << endl;
			}
			catch (exception* e)
			{
				cout << e->what() << endl;
			}
			in = getInput();
		}
	}
	Calc() { memory = new unordered_map<string, double>(); };
};

template <typename T>
class HashMap
{
	//Array to hold values from 0-s, s=size ***Array holds a struct, T for the value and string for the key
	//Function to convert string to int between 0 and s ***RESEARCH HASH FXNS {Add up (int)chars and % s
	//Get, Set, Remove, Resize (Double size when full)

};

int main()
{
	Calc* calc = new Calc();

	calc->start();

	return 0;
}