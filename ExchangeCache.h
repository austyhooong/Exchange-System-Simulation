#ifndef EXCHANGECACHE_H
#define EXCHANGECACHE_H    // this will be a flag such that if this file is included again since the token is already defined the rest of this code wont be run
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <iostream>
#include <fstream>
#include <unordered_map> 
#include <limits>
#include <set>
#include <string_view>

#include <boost/algorithm/string.hpp>

using namespace std;

enum class Order
{
	Sell,
	Buy,
};

enum class alarmCheck
{
	E_Order = 0,
	E_Transaction = 1,
	E_ID = 2,
	E_Price = 3,
	E_Quantity = 4,
};

class Book;
class ExchangeCache;

void bookProcess(string& book, ExchangeCache& cache);

class Trade
{
public:
	string tickerID;
	double price;
	int quantity;
	Trade() = default;
	Trade(string ticker, double price, int size) : tickerID(ticker), price(price), quantity(size) {};
	friend ostream& operator<<(ostream& os, const Trade& s);
};
//trade, contract_ID, price,size
ostream& operator<<(ostream& os, const Trade& t);

class Sell
{
private:

public:
	Order side = Order::Sell; //enum keep it simple no need oop
	string tickerID;
	double price;
	int quantity;
	Sell() = default;
	Sell(string info, string ticker, double price, int size) :  tickerID(ticker), price(price), quantity(size) {};
	friend ostream& operator<<(ostream& os, const Sell& s);
};
//order, side, contract_ID, price, size
ostream& operator<<(ostream& os, const Sell& s);

class Buy
{
public:
	Order side = Order::Buy; //enum keep it simple no need oop
	string tickerID;
	double price;
	int quantity;
	Buy() = default;
	Buy(string info, string ticker, double price, int size) :  tickerID(ticker), price(price), quantity(size) {}
	friend ostream& operator<<(ostream& os, const Buy& s);
};
//order, side, contract_ID, price, size
ostream& operator<<(ostream& os, const Buy& b);

struct CompBuy
{
	bool operator()(const pair<Buy, int>& lhs, const pair<Buy, int>& rhs) const
	{
		return lhs.first.price > rhs.first.price or lhs.first.price == rhs.first.price and lhs.second < rhs.second;
	}
};

struct CompSell
{
	bool operator() (const pair<Sell, int>& lhs, const pair<Sell, int>& rhs) const
	{
		return lhs.first.price < rhs.first.price or lhs.first.price == rhs.first.price and lhs.second < rhs.second;
	}
};
void bookProcess(string&, ExchangeCache&);
bool alarm(const vector<string>&);
void UIMessage();
Trade tradeBuilder(const vector<string>&);
Buy buyBuilder(const vector<string>&, const string& book);
Sell sellBuilder(const vector<string>&, const string& book);

class ExchangeCache
{
private:
	static ofstream cacheFileP;    // static function can only access static member while non static function can access all data members
	static ofstream cacheFileC;
	int maxSize;
	friend class Book;
	friend ostream& operator<<(ostream& os, const Book& exchange);
	friend void bookProcess(string&, ExchangeCache&);
	unordered_map<string, shared_ptr<Book>> activeTransaction;

public:
	void recordBook(string tickerId, shared_ptr<Book> exchange)
	{
		activeTransaction[tickerId] = exchange;
		//book dosnt have to access from bottom to top;
		//top call bottom's function
		//design pattern: strategy? composite pattern?
		//static is dependency ingection: child to send notification (pointer to function)
	}

	void saveFile(const string& fileName = "");
	void saveUserFile(const string& fileName);
	void display(ostream&, string tickerId);
	
	void load();
	~ExchangeCache()
	{
		saveFile();
	}
};


inline void UIMessage()
{
	cout << "Type 'Exit' to exit\n";
	cout << "Type 'Save' to save the current state of the pending orders\n";
	cout << "Type 'Display' to show all the pending trades for a selected security\n";
	cout << "Type your order in the form of 'ORDER,SIDE,TICKER_ID,PRICE,SIZE'\n";
}

inline Trade tradeBuilder(const vector<string>& details)
{
	//trade, contract_ID, price,size
	Trade t(details[2], stod(details[3]), stoi(details[4]));
	return t;
}

inline Buy buyBuilder(const vector<string>& details, const string& book)
{
	//order,side,contract_ID,price,size
	Buy b(book, details[2], stod(details[3]), stoi(details[4]));
	return b;
}

inline Sell sellBuilder(const vector<string>& details, const string& book)
{
	//order,side,contract_ID,price,size
	Sell s(book, details[2], stod(details[3]), stoi(details[4]));
	return s;
}

#endif