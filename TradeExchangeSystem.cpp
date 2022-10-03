#include "ExchangeCache.h"
#include "Book.h"
using namespace std;
//unordered_map<string, shared_ptr<Book>> activeTickers;

int main()
{
	//initialize the exchange with cached data
	ExchangeCache cache;
	cache.load();
	cout << "Please upload your exchange file ";
	string userFile;
	getline(cin, userFile);
	//cin.ignore(numeric_limits<streamsize>::max(), '\n');    //skips the number of char specified in the first parameter or all the characters up to and including the char specified in the second parameter, whichever comes first.
	ifstream tradeFile(userFile);
	if (userFile.empty() or tradeFile.fail())
	{
		cout << "reading default file " << '\n';
		tradeFile.close();
		tradeFile.clear();
		tradeFile.open("ExchangeTest.txt");
	}
	Book exchange;
	if (tradeFile.is_open())
	{
		string book;
		while (getline(tradeFile, book))    //getline removes the newline from the input stream while cin does not
		{
			bookProcess(book, cache);    //trade = already completed; order = pending to be matched
		}
		//phase 3: User Interface
		UIMessage();
		string userInput;
		
		while (getline(cin, userInput))    //getline extract and discrard the delimiter so no need to cin.ignore
		{	
			if (userInput.empty()) break;
			
			boost::algorithm::to_upper(userInput);
			string_view stringTest{ userInput };
			if (stringTest.substr(0,4) == "EXIT")    //use string_view? not compatible for getline() also could not use for alarm and bookprocess due to the requirement of stoi use
				return 0;
			else if (stringTest.substr(0, 7) == "DISPLAY")
			{
				cout << "Please enter the name of your security\n";
				string tickerId;
				cin >> tickerId;
				boost::algorithm::to_upper(tickerId);
				cache.display(cout, tickerId);
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			else if (stringTest.substr(0, 4) == "SAVE")
			{
				cout << "Please enter the name of your file ending in 'csv'\n";
				string userFile;
				cin >> userFile;
				string_view userSv{ userFile };
				if (!userSv.ends_with(".csv"))
					userFile += ".csv";
				cache.saveUserFile(userFile);
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
			else
				bookProcess(userInput, cache);
			UIMessage();
		}
	}
	tradeFile.close();
}


