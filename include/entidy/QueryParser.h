#pragma once
#include <deque>
#include <string>
#include <vector>

#include <entidy/Exception.h>

namespace entidy
{
using namespace std;

template <typename Type>
class QueryParserAdapter
{
public:
	virtual Type Evaluate(const string& token) = 0;
	virtual Type And(const Type& lhs, const Type& rhs) = 0;
	virtual Type Or(const Type& lhs, const Type& rhs) = 0;
	virtual Type Not(const Type& rhs) = 0;
};

enum TokenType
{
	Nil,
	And,
	Or,
	Not,
	BlockStart,
	BlockEnd,
	Leaf
};

class Token
{
public:
	string key;
	TokenType op;
	vector<Token> children;

	Token() { }

	Token(const string& key)
	{
		op = Parse(key);
	}

	TokenType Parse(const string& key)
	{
		this->key = key;
		if(key == " ")
			return TokenType::Nil;
		else if(key == "&")
			return TokenType::And;
		else if(key == "|")
			return TokenType::Or;
		else if(key == "!")
			return TokenType::Not;
		else if(key == "(")
			return TokenType::BlockStart;
		else if(key == ")")
			return TokenType::BlockEnd;
		else
			return TokenType::Leaf;
	}

	bool Valid()
	{
		switch(op)
		{
		case TokenType::And:
		case TokenType::Or: {
			if(children.size() != 2)
				return false;
			return children[0].Valid() && children[1].Valid();
		}
		case TokenType::Not: {
			if(children.size() != 1)
				return false;
			return children[0].Valid();
		}
		case TokenType::Leaf:
			return true;
		};

		return false;
	}

	template <typename Type>
	Type Evaluate(QueryParserAdapter<Type>* adapter)
	{
		if(op == TokenType::Leaf)
			return adapter->Evaluate(key);

		if(op == TokenType::And)
			return adapter->And(children[0].Evaluate(adapter), children[1].Evaluate(adapter));

		if(op == TokenType::Or)
			return adapter->Or(children[0].Evaluate(adapter), children[1].Evaluate(adapter));

		if(op == TokenType::Not)
			return adapter->Not(children[0].Evaluate(adapter));

		throw EntidyException("Bad Token: " + key);
	}
};

template <typename Type>
class QueryParser
{
protected:
	QueryParserAdapter<Type>* adapter;

	deque<Token> Tokenize(const string& query)
	{
		std::string const delims{" ()&|!"};
		deque<Token> tokens;

		size_t prev = 0, pos = 0;
		while((pos = query.find_first_of(delims, prev)) != std::string::npos)
		{
			if(pos == prev)
			{
				string cur = query.substr(pos, 1);
				if(cur != " ")
					tokens.push_back(Token(cur));
				++prev;
				continue;
			}

			string key = query.substr(prev, pos - prev);
			prev = pos;
			tokens.push_back(Token(key));
		}

		string key = query.substr(prev, query.size());
		//if(tokens.size() == 0)
		tokens.push_back(Token(key));

		return tokens;
	}

	bool ParseBlocks(deque<Token>& tokens)
	{
		if(tokens.size() < 3)
			return false;

		auto it = tokens.begin();
		while(it < tokens.end() - 2)
		{
			auto prev = it;
			auto cur = it + 1;
			auto next = it + 2;

			if(prev->op == TokenType::BlockStart && next->op == TokenType::BlockEnd)
			{
				it = tokens.erase(prev);
				tokens.erase(it + 1);
				return true;
			}

			if(prev->op == TokenType::BlockStart && cur->op == TokenType::BlockEnd)
			{
				it = tokens.erase(prev);
				tokens.erase(it);
				return true;
			}

			if(cur->op == TokenType::BlockStart && next->op == TokenType::BlockEnd)
			{
				it = tokens.erase(cur);
				tokens.erase(it);
				return true;
			}
			++it;
		}
		return false;
	}

	bool ParseNot(deque<Token>& tokens)
	{
		auto it = tokens.begin();
		while(it < tokens.end() - 1)
		{
			auto prev = it;
			auto cur = it + 1;

			if(prev->op == TokenType::Not && cur->Valid() && !prev->Valid())
			{
				prev->children.push_back(*cur);
				tokens.erase(cur);
				return true;
			}

			++it;
		}
		return false;
	}

	bool ParseAndOr(deque<Token>& tokens, bool op)
	{
		if(tokens.size() < 3)
			return false;

		auto it = tokens.begin();
		while(it < tokens.end() - 2)
		{
			auto prev = it;
			auto cur = it + 1;
			auto next = it + 2;

			if(((op && cur->op == TokenType::And) || (!op && cur->op == TokenType::Or)) && !cur->Valid())
			{
				if(prev->Valid() && next->Valid())
				{
					cur->children.push_back(*prev);
					cur->children.push_back(*next);
					it = tokens.erase(prev);
					tokens.erase(it + 1);
					return true;
				}
			}
			++it;
		}

		return false;
	}

	bool BuildTree(deque<Token>& tokens)
	{
		bool operate = false;
		bool done = false;
		while(!done)
		{
			done = true;

			while(ParseBlocks(tokens))
				done = false;

			if(!done)
				continue;

			while(ParseNot(tokens))
				done = false;

			if(!done)
				continue;

			while(ParseAndOr(tokens, true))
				done = false;

			if(!done)
				continue;

			if(ParseAndOr(tokens, false))
				done = false;
		}

		return tokens.size() == 1;
	}

public:
	QueryParser(QueryParserAdapter<Type>* adapter)
	{
		this->adapter = adapter;
	}

	Type Parse(const string& query)
	{
		auto tokens = Tokenize(query);
		if(!BuildTree(tokens))
			throw EntidyException("Bad Query; Check syntax: " + query);

		return tokens.front().Evaluate(adapter);
	}
};

} // namespace entidy
