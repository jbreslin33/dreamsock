#include <iostream>
#include <iomanip>
#include <mysql++>


int main(void)
{
	// -> Create a connection to the database
	Connection con("gamedata","127.0.0.1");

	// -> Create a query object that is bound to our connection
	Query query = con.query();

	// -> Assign the query to that object
	query << "SELECT * FROM paymentinfo";

	// -> Store the results from then query
	Result res = query.store();

	// -> Display the results to the console
	

	// -> Show the Field Headings
	cout.setf(ios::left);
	cout << setw(6) << "id" 
	     << setw(10)  << "playerid"
	     << setw(20)  << "datepaid"
		 << setw(20)  << "type"
		 << setw(20)  << "amount"   << endl;

	Result::iterator i;
	Row row;

	// The Result class has a read-only Random Access Iterator
	for (i = res.begin(); i != res.end(); i++) 
	{
		row = *i;
		cout << setw(6) << row["id"] 
	    << setw(10)  << row["playerid"]
	    << setw(20)  << row["datepaid"]
		<< setw(20)  << row["type"]
		<< setw(20)  << row["amount"]   << endl;
	}

	return 1;
}
