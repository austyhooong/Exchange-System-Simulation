#pragma once
class Book
{
private:
	//comp adjusted for set
	set<pair<Sell, int>, CompSell> sellSet;
	set<pair<Buy, int>, CompBuy> buySet;
	//priority_queue<pair<Sell, int>, vector<pair<Sell, int>>, CompSell> sellSet; //min heap, pair int is to store the index in the vecor for sorting by order; amount of order can be obtained from the object
	//priority_queue<pair<Buy, int>, vector<pair<Buy, int>>, CompBuy> buySet; //max heap
	vector<Trade> tradeBook; //if there is a functionality to cancel contraction ID (binary search tree)
	string tickerId;
	unsigned int index;
	friend ExchangeCache;
public:
	Book(string ticker = "") : tickerId(ticker), index(0) {}

	//void display()
	//{
	//	if (tradeBook.empty())
	//		cout << "No historical trades as of now" << endl;
	//	cout << *this;
	//}
	friend ostream& operator<<(ostream& os, const Book& exchange);

	inline void record(const Sell& s)
	{
		sellSet.emplace(s, index++);
	}

	inline void record(const Buy& b)
	{
		buySet.emplace(b, index++);
	}

	inline void record(const Trade& t)
	{
		tradeBook.push_back(t);
	}
	void transact();
};
