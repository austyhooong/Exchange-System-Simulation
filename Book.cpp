#include "ExchangeCache.h"
#include "Book.h"
void Book::transact()
{
	while (!sellSet.empty() and !buySet.empty()
		and sellSet.begin()->first.price == buySet.begin()->first.price)
	{
		auto [sold, soldInd] = *sellSet.begin();
		auto [bought, boughtInd] = *buySet.begin();
		int tradedQuantity = min(sold.quantity, bought.quantity);
		if (sold.quantity == bought.quantity)
		{
			//need to removed from the vector too!
			/*auto removed = cache.historyBook[tickerId][boughtInd];
			swap(cache.historyBook[tickerId].back(), removed);*/

			sellSet.erase(sellSet.begin());
			buySet.erase(buySet.begin());
		}
		else if (sold.quantity > bought.quantity)
		{
			buySet.erase(buySet.begin());
			auto [remainingSell, ind] = *sellSet.begin();
			sellSet.erase(sellSet.begin());
			remainingSell.quantity -= tradedQuantity;
			sellSet.emplace(remainingSell, ind);
		}
		else
		{
			sellSet.erase(sellSet.begin());
			auto [remainingBuy, ind] = *buySet.begin();
			buySet.erase(buySet.begin());
			remainingBuy.quantity -= tradedQuantity;
			buySet.emplace(remainingBuy, ind);
		}
		//string transactInfo = "Trade," + tickerId + "," + to_string(sold.price) + "," + to_string(tradedQuantity);
		tradeBook.emplace_back(sold.tickerID, sold.price, tradedQuantity);
		cout << "Trade Completed: " << tickerId << " Quantity: " << tradedQuantity << " Price: " << sold.price << '\n';
	}

}

ostream& operator<<(ostream& output, const Book& exchange)
{
	output << "History of completed trades: " << '\n';
	for (auto const& t : exchange.tradeBook)
	{
		output << t;
	}
	output << "Pending sell orders: " << '\n';
	for (auto const& [sell, idx] : exchange.sellSet)
	{
		output << sell;
	}
	output << "Pending buy orders: " << '\n';
	for (auto const& [buy, idx] : exchange.buySet)
	{
		output << buy;
	}
	return output;
}

