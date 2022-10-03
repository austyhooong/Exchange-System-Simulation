#include "ExchangeCache.h"
#include "Book.h"

const char* OrderTypes[] = { "Sell", "Buy" };
ofstream ExchangeCache::cacheFileP = ofstream("CacheHistory_Pending.csv", ios::app);
ofstream ExchangeCache::cacheFileC = ofstream("CacheHistory_Completed.csv", ios::app);
void ExchangeCache::load()
{
	ifstream cacheFileP("CacheHistory_Pending.csv");
	if (cacheFileP.fail())
	{
		cout << "No pending transactions in the database\n";
		return;
	}
	string transactionInfo;
	unordered_map<string, vector<string>> bookInfos;
	string tickerId;
	while (getline(cacheFileP, transactionInfo))
	{
		if (transactionInfo.substr(0, 6) == "Ticker")
		{	// moves to the next ticker
			tickerId.clear();
			istringstream tickerStream(transactionInfo);
			while (getline(tickerStream, tickerId, ' '))  //there might be space in the beginning
			{
				if (tickerId.substr(0, 6) != "Ticker")
					break;
			}
			tickerId.erase(remove(tickerId.begin(), tickerId.end(), ','), tickerId.end());
		}
		else
			bookProcess(transactionInfo, *this);
	}
}

void ExchangeCache::saveUserFile(const string& fileName)
{
	ofstream userFile(fileName, ios::app);
	for (auto const& tickerInfo : activeTransaction)
	{
		//ORDER,SIDE,ticker,PRICE,SIZE (for Order) vie << overload
		auto& tickerBook = *(tickerInfo.second);
		for (auto const& sell : tickerBook.sellSet)
		{
			userFile << sell.first << '\n';
		}
		for (auto const& buy : tickerBook.buySet)
		{
			userFile << buy.first << '\n';
		}
		//TRADE, ticker, PRICE, SIZE(for trade) vie << overload
		for (auto const& trade : tickerBook.tradeBook)
		{
			userFile << trade << '\n';
		}
	}
}
void ExchangeCache::saveFile(const string& fileName)  //default value can only be stated in declaration or definition
{
	if (fileName != "")
		ofstream cacheFileP(fileName, ios::app);
	for (auto const& tickerInfo : activeTransaction)
	{
		auto& tickerBook = *(tickerInfo.second);
		//ORDER,SIDE,ticker,PRICE,SIZE (for Order) vie << overload
		for (auto const& transaction : tickerBook.sellSet)
		{
			cacheFileP << transaction.first << '\n';
		}
		for (auto const& transaction : tickerBook.buySet)
		{
			cacheFileP << transaction.first << '\n';
		}
		//TRADE, ticker, PRICE, SIZE(for trade) vie << overload
		for (auto const& trade : tickerBook.tradeBook)
		{
			cacheFileC << trade << ", ";
		}
	}
}
// inline should be implemented for those with siginificant function call overhead comparing to its function complexity
void ExchangeCache::display(ostream& output, string tickerId)
{
	if (auto it = activeTransaction.find(tickerId); it == activeTransaction.end())
	{
		cout << "There is no trade for the entered security \n";
	}
	else
	{
		//overloaded book extraction operator that can output trade comleted, pending sell and pending buy
		output << it->second;
	}
}

void bookProcess(string& book, ExchangeCache& cache)
{
	string category;
	istringstream bstream(book);
	vector<string> details;
	while (getline(bstream, category, ','))
	{
		details.push_back(category);
		if (details.size() == 5)
			break;
	}
	if (!(alarm(details))) return;

	string tickerId = boost::algorithm::to_upper_copy(details[2]);

	auto it = cache.activeTransaction.find(tickerId);
	if (it == cache.activeTransaction.end())
	{
		cache.activeTransaction[tickerId] = make_shared<Book>(tickerId);
		cache.recordBook(tickerId, cache.activeTransaction[tickerId]);
		it = cache.activeTransaction.find(tickerId);
	}
	if (details[0] == "TRADE")
	{
		auto trade = tradeBuilder(details);
		it->second->record(trade);
	}
	else
	{
		if (details[1] == "SELL")
		{
			auto sold = sellBuilder(details, book);
			it->second->record(sold);
		}
		else if (details[1] == "BUY")
		{
			auto bought = buyBuilder(details, book);
			it->second->record(bought);
		}
	}
	it->second->transact();
}

bool alarm(const vector<string>& details)     //if false => error
{
	if (details.size() != 5) return false;

	//tring testOrder(details[0].size(), ' '), testTransaction(details[1].size(), ' ');
	string testOrder = boost::algorithm::to_upper_copy(details[0]),
		testTransaction = boost::algorithm::to_upper_copy(details[1]);
	if (testOrder != "ORDER" and testOrder != "TRADE")
	{
		cout << "please enter ORDER or TRADE" << endl;
		return false;
	}

	if (testTransaction != "BUY" and testTransaction != "SELL")
	{
		cout << "please enter BUY or SELL" << '\n';
		return false;
	}

	if (string tickerId = details[to_underlying(alarmCheck::E_ID)]; tickerId.empty())
	{
		cout << "please specify the ticker ID\n";
		return false;
	}

	string price = details[static_cast<int>(alarmCheck::E_Price)];
	size_t sz = price.size();
	for (size_t j = 0; j < sz; ++j)
	{
		if (!isdigit(price[j]) and price[j] != '.')
		{
			cout << "please enter the correct price" << endl;
			return false;
		}
	}

	string quantity = details[static_cast<int>(alarmCheck::E_Quantity)];
	for (size_t j = 0; j < sz; ++j)
		try
	{
		int q = stoi(quantity);
	}
	catch (exception& e)
	{
		cout << "Please enter the correct quantity \n";
		return false;
	}
	return true;
}

//print Trade
ostream& operator<<(ostream& os, const Trade& t)
{
	//instead of storing transactionInfo, overloading the output operator to save the existing info
	os << "Trade" << ',' << t.tickerID << ',' << t.price << ',' << t.quantity;
	return os;
};

//print Sell
ostream& operator<<(ostream& os, const Sell& s)
{
	os << "Order " << OrderTypes[to_underlying(s.side)] << ',' << s.tickerID << ',' << s.price << ',' << s.quantity;
	return os;
};
//print Buy
ostream& operator<<(ostream& os, const Buy& b)
{
	os << "Order " << OrderTypes[to_underlying(b.side)] << ',' << b.tickerID << ',' << b.price << ',' << b.quantity;
	return os;
};