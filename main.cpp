//
//  main.cpp
//  Finalproject
//
//  Created by Erik Bowers and Nicholas Bouckaert for ECE3220
//  Project was edited and update for use in CMP3380 by Nicholas Bouckaert, Josh Eastman, ***ADD names here***
//

//......................COMPILE USING C++11 STANDARDS.........................//////


#define COMPILER 0      ////...........IF USING XCODE COMPILE USING 0.....GNU COMPILER USE 1...............///////////
						////...........This is needed primarily because XCode handles file locations very oddly....../////////
#define SLEEPTIME 5  //in seconds

#define ACCOUNTS "accounts.txt"
//#define STOCKS "stocks.txt"

#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <string.h>
#include <unistd.h>
#include <iomanip>
#include <sqlite3.h>

#define db "accounts.db"
sqlite3 *dbfile;

using namespace std;

string id_key;
vector<string>stock_n;
vector<int>stock_p;
vector<double>stock_v;
int share_price;
int balance;

int callback(void *NotUsed, int argc, char **argv,char **azColName);
int callback(void *NotUsed, int argc, char **argv,char **azColName);

string input_file = "accounts.txt";// stores the list of accounts for the program and their passwords
class Log_in{
public:
    Log_in(){};
    ~Log_in(){};
    void login();
    string success;
    string id;
    void locate_account(string entered_id, string entered_password );
    int database_connection (string entered_id, string entered_password);// attempt to connect to a database
    void getTableData ( string entered_id, string entered_password);
    
};
class Account:public Log_in{
    protected:
        string account_id;
        string account_password;
        int account_type;// 1 is admin, 5 is user
        //double balance;
        vector<string> stock_name;
        vector<int> number_shares;
        vector<double> stock_price;
        vector<double> availableStocks;
    
    public:
        Account(){};
        Account(string user_id, string password, int type);
        ~Account(){};
        void operator+(int add);
        void operator-(int subtract);
        void update_user_file (string name, int number, int update);
        void buyStocks(string name, int number);
        //void locate_account(string entered_id, string entered_password );
        virtual void menu();
        void display_profolio();
        void read_accountinfo();
        void readstock_price();
        void loadstocks();// may use later in the code
        void setstockformarketcharge();
        void search_forstock(string stock); // will open available stocks and search for that stock in the folder
        int checkBackgroundUpdate(string stock_name, int numStocks, double price, int action);
    
    
};
int Log_in :: database_connection (string entered_id, string entered_password)
{
    int rc;
    char *zErrMsg = 0;
    
    
    rc = sqlite3_open("stock.db", &dbfile);
    
    if( rc)
    {
        fprintf(stderr, "Can it open database: %s\n", sqlite3_errmsg(dbfile));
        return -1;
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }
    getTableData( entered_id, entered_password);
    //sqlite3_close(dbfile);
    
    return 0;
    
}
void Log_in :: getTableData(string entered_id, string entered_password)
{
    
    sqlite3_stmt *statement;
    char *err_msg = 0;
    string s;
    string m = "Select * from accounts where user = ";
    s = m + "'" + entered_id + "' and password = '" + entered_password + "'";
    // used to see the print statement
    cout << s << endl;
    
    char *str = &s[0];
    char* query =  str;
    
    if ( sqlite3_prepare(dbfile, query, -1, &statement, 0 ) == SQLITE_OK )
    {
        int ctotal = sqlite3_column_count(statement);
        int res = 0;
        vector<string> information;
        char user[] = "nabyq6";
        sqlite3_bind_text(statement, 1, user, sizeof(user), SQLITE_STATIC);
        
        
        //res = sqlite3_exec(dbfile, query, callback, 0, &err_msg);
        if (res != SQLITE_OK ) {
            
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            
            sqlite3_free(err_msg);
            exit(-1);
        }
        // good idea pain in the ass to implement
       while(1)
       {
            res = sqlite3_step(statement);
            
            if ( res == SQLITE_ROW )
            {
                for ( int i = 0; i < ctotal; i++ )
                {
                    string s = (char*)sqlite3_column_text(statement, i);
                    //print or format the output as you want
                    information.push_back(s);
                    //cout << s << " " ;
                    
                    
                }
                cout << endl;
                id_key= information[0];
                success = information[3];
                balance = stoi(information[4]);
            }
            
            if ( res == SQLITE_DONE || res==SQLITE_ERROR)
            {
                cout << "End of result done " << endl;
                break;
            }
        }
        
    }
}
class Administrator: public Account {//extension of the case account class
    
private:
    string admin_id;
    string admin_password;
    int admin_type;
public:
    Administrator(){};
    Administrator(string user_id, string password, int type);
    ~Administrator(){};
    virtual void menu();
    void change_password();
    void delete_account();
    void create_account(int type);
    virtual void display_all();
    void backgroundUpdate();
};


void Account:: operator-(int subtract){//simple and not completely need but demomstates operators which is a requirement to meet
    balance = balance - subtract;
}
void Account:: operator+(int add){//simple and not completely need but demomstates operators which is a requirement to meet
    balance = balance + add;
}
int Account::checkBackgroundUpdate(string NewStock, int numStocks, double price, int action)
{
	this->read_accountinfo();
	this->readstock_price();


	int i = 0;


	for(i=0;i<stock_name.size();i++)//iterate until stock is found
	{
		if(action == 1)
		{
		if(stock_name[i].compare(NewStock)==0)
		{
			//cout << "Cost:" << stock_price[i] << endl;
			if(stock_price[i] <= price)
		{
				cout << account_id << " buying " << numStocks  << " "<< NewStock << " stocks for " << stock_price[i]<< endl;
			buyStocks(NewStock, numStocks);
			return 1;
		}
			break;
		}
		}

		if(action == 2)
		{
			if(stock_name[i].compare(NewStock)==0)
			{
				if(stock_price[i] >= price)
				{
					cout << account_id << " selling " << numStocks  << " "<< NewStock << " stocks for " << stock_price[i]<< endl;
                    numStocks = 0 - numStocks;
                    buyStocks(NewStock, numStocks);
                    return 1;
				}
				break;
			}

		}
	}
	return 0;

}

void Administrator::backgroundUpdate()
{
													/*This function will loop and buy/sell user stocks based on their preferences.
													 * Right now the only way to exit out of this loop is to use ctrl c to exit
													 * the entire program.  Mulithreading would be one solution to this and we could
													 * even run this function in the background, but that is currently
													 * a little out of scope for this project.
													 *
													 * In the future, multithreading will hopefully be implemented
													 */
//	ifstream input;
//	input.open("inputfiles/set_to_sell_or_buy.txt");

	int exit = 0;
	cout << "Auto update is now running. Message will be displayed when stocks are bought or sold" << endl;
	cout << "ctrl c to exit" << endl;
	//cin >> exit;

	int numStocks = 0;
	double price = 0;
	int action = 0;
	string account_id;
	string stock_name;

	while(exit != -1)
	{
//	cin >> exit;
	sleep(SLEEPTIME);//sleep takes in seconds

	ifstream input;
	input.open("inputfiles/set_to_sell_or_buy.txt");

	ofstream outputTemp;
	outputTemp.open("inputfiles/temp.txt", ios::app);    				/*This temp file is used so that we can essentially
																		 *delete the line from the set to buy/sell file once
																		 *that stock has been bought. We rewrite the file and then delete
																		 *the original and rename the temp file*/

//		this->readstock_price();//I don't know why "this->" is needed
		while(input >> account_id >> stock_name >> numStocks >> price >> action)
		{
			Account temp(account_id, "password", 5);
			//The buy/sell functions will need modified to take the name and number as arguments instead of asking for them


				if(temp.checkBackgroundUpdate(stock_name, numStocks, price, action) == 0)
				{
					outputTemp << account_id << " " << stock_name << " " << numStocks << " " << price << " " << action << endl;
				}
				else
				{
					//This is run instead so the line that was run is not written back to the file
				}



		}

		outputTemp.close();
		input.close();
	    remove("inputfiles/set_to_sell_or_buy.txt");
	    rename("inputfiles/temp.txt","inputfiles/set_to_sell_or_buy.txt");

	//	input.clear();          ///These ensure we go back to the top of the file
	//	input.seekg(0, ios::beg);

	}


	return;

}

void Administrator:: change_password(){ // allows admin to reset a password. However they can not see the original password

	/*This function is not working on linux. I can't follow it well enough to to try and get it working.  I
	 * edited the file names and it straight up froze my machine.
	 */
    
    // backwords way of changing the file and i dont like it all since im rewriteing the whole file. Luis when grading this if you wont mind emailing my school email pawprint is nabyq6 i would like to know if that was an easier way of doing this. I tried setting the trunc flag however whenever i did the file would never open.
    string user_name, new_pw, space;
    string file_password, file_id;
    vector<string> id, password;
    vector<int> level;
    char *err_msg = 0;
    
        getline( cin, space);
        cout<<"Enter the name of the user whose password needs to be change:"<<endl;
        getline(cin, user_name);
    
        cout<<"Enter the password you would like to"<<endl;
        getline(cin, new_pw);
    
        string m = "Update Accounts Set password = '";
        string s;
        s = m + new_pw + "' where user = '" + user_name+"'" ;
        cout<<s<<endl;
    
        char *str = &s[0];
        char* query =  str;
        int res = 0;
    
        res = sqlite3_exec(dbfile, query, 0, 0, &err_msg);
        if (res != SQLITE_OK )
        {
            
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            
            sqlite3_free(err_msg);
            exit(-1);
        }
        if ( res == SQLITE_DONE || res==SQLITE_ERROR)
        {
            cout << "stocks loaded " << endl;
        }
        cout << "** Password has been Updated**"<<endl<<endl;
}
void Administrator::delete_account(){
    string user_name,space, s;
    char *err_msg = 0;
    
    getline(cin, space);//take in the enter from the user and dont use
    cout<<"Enter the user name of the account you would like to delete"<<endl;
    getline(cin,user_name);
    
    string m = "Delete from accounts where user = '";
    s = m + user_name + "'" ;
    cout << s << endl;
    
    char *str = &s[0];
    char* query =  str;
    int res = 0;
    
    res = sqlite3_exec(dbfile, query, 0, 0, &err_msg);
    if (res != SQLITE_OK )
    {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        exit(-1);
    }
    if ( res == SQLITE_DONE || res==SQLITE_ERROR)
    {
        cout << "stocks loaded " << endl;
    }
    cout << "** Account has been deleted** " << endl;
}
void Administrator:: create_account(int type){//allows the admin to create an account once admin is logged in.
    string user_name, password, s;
    double bal;
    string space;
    char *err_msg = 0;
    
        getline(cin, space);//take in the enter from the user and dont use
        cout<<"Enter the User name you would like to use"<<endl;
        getline(cin, user_name);
        cout<<"Enter the password you would like to"<<endl;
        getline(cin, password);
        if( type != 1)
        {
        cout<<"Enter the users beginning balance:"<<endl;
        cin>>bal;
        }
        else
            bal = 0;
    
        string m = "Insert Into accounts VALUES (";
        s = m + "?" + ",'" + user_name+ "','" + password +"','" + to_string(type) + "','" + to_string(bal)+"')" ;
        cout<<s<<endl;
    
        char *str = &s[0];
        char* query =  str;
        int res = 0;
    
        res = sqlite3_exec(dbfile, query, 0, 0, &err_msg);
        if (res != SQLITE_OK )
        {
            
            fprintf(stderr, "Failed to select data\n");
            fprintf(stderr, "SQL error: %s\n", err_msg);
            
            sqlite3_free(err_msg);
            exit(-1);
        }
        if ( res == SQLITE_DONE || res==SQLITE_ERROR)
        {
            cout << "stocks loaded " << endl;
        }
        cout << "**New account has been add to the database** " << endl;
    
}
int callback5(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    for (int i = 0; i < argc; i++) {
        
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        printf("%s = %s\n", azColName[i+1], argv[i+1] ? argv[i+1] : "NULL");
        printf("%s = %s\n", azColName[i+2], argv[i+2] ? argv[i+2] : "NULL");
        printf("%s = %s\n", azColName[i+3], argv[i+3] ? argv[i+3] : "NULL");
        printf("%s = %s\n\n", azColName[i+4], argv[i+4] ? argv[i+4] : "NULL");
        i=i+4;
    }
    return 0;
}
void Administrator:: display_all()
{// displays all the accounts with the company
    
    char *err_msg = 0;
    string s;
    string m = "Select * from accounts";
    // used to see the print statement
    cout << s << endl;
    
    char *str = &m[0];
    char* query =  str;
    int res = 0;
    
    res = sqlite3_exec(dbfile, query, callback5, 0, &err_msg);
    if (res != SQLITE_OK )
    {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        exit(-1);
    }
    if ( res == SQLITE_DONE || res==SQLITE_ERROR)
    {
        cout << "stocks loaded " << endl;
    }
    
}

/*
 * When logging in as another user there is no way to log out as them.  It throws an error.
 * I don't know how you implemented this. -Erik
 */


void Administrator:: menu()// menu for the admin user with the type = 5 in file
{
    string space;
    int choice = 0;
try{
    do{
        if( admin_type == 1 )
       {
        cout<<"Select what you would like to do"<<endl
        <<"\t1: See all accounts: "<<endl
        <<"\t2: Create account: "<<endl
        <<"\t3: Change user password: "<<endl
        <<"\t4: Login with user profolio "<<endl
        <<"\t5: To Exit your profolio: "<<endl
        <<"\t6: Delete an Account" << endl;
        cin>>choice;
        
        switch( choice ){
                
            case 1://See all accounts
                display_all();
                break;
            case 2:// Create account
                int type;
                cout<<"what type account will this be:"<<endl
                <<"1:admin"<<endl
                <<"5:user"<<endl;
                cin>>type;
                create_account(type);
                break;
            case 3://change user password or id
                change_password();
                break;
            case 4://Display users profolio
                cout<<"Please have the client login"<<endl;;
                getline(cin, space);
                login();
                break;
            case 5://exit
                throw 0;
                break;
            case 6:// this will allow admin to delete an account
                delete_account();
                break;
            case 7://Run sleep/background updating
            	backgroundUpdate();
            	break;
            default:
                break;
            }
       }
    }while( choice != 5);
}
    
    catch(...){
        cout<<"you have been logged out of your account"<<endl;// thrown whenever the account is signed out of
    }
}
Administrator::Administrator( string user_id, string password, int type)//admin constructor
{
    admin_id = user_id;
    admin_password = password;
    admin_type = type;
}
//.........END HEADER........//
//..........from Erik's functions........//


void Account::update_user_file( string name, int number, int update)//updates the users file if stocks are brought or solid.
{
    char *err_msg = 0;
    string s;
    if( update == 1 )
    {
    string m = "Insert Into stocks( id, stock, shares_number ) VALUES (' ";
        s = m + id_key + "','" + name+ "','" + to_string(number)+"')" ;
    // used to see the print statement
    cout << s << endl;
    }
    else
    {
        string m = "Update stocks Set shares_number = '";
        s = m + to_string(number) + "' where id = '" + id_key+ "' and stock = '" + name +"'" ;
        // used to see the print statement
        cout << s << endl;
    }
    char *str = &s[0];
    char* query =  str;
    int res = 0;
    
    res = sqlite3_exec(dbfile, query, 0, 0, &err_msg);
    if (res != SQLITE_OK )
    {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        exit(-1);
    }
    if ( res == SQLITE_DONE || res==SQLITE_ERROR)
    {
        cout << "stocks loaded " << endl;
    }
}

void Account::buyStocks(string name, int number)// allows the user to buy stocks if the company is selling them at the current time
{
    int price;
    
    search_forstock(name);
    price = share_price;
    
    if( number * price > balance)
    {
        cout<<"Insuffient funds availabe"<<endl;
        return;
    }
    int i;
    
    for(i = 0; i < stock_name.size(); i++)
    {
        if(stock_name[i] == name)
        {
            number_shares[i] += number;
            balance = balance - (number * price);
            update_user_file(name, number_shares[i], 0);
            cout<<"shares have been brought or solid and updated"<<endl;
            return;
        }
        
    }
    balance = balance + (number * price);
    update_user_file(name, number, 1);//update user file
    number_shares.push_back(number);
    stock_name.push_back(name);
    stock_price.push_back(price);
    
	return;
}
int callback3(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    for (int i = 0; i < argc; i++) {
        
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //printf("%s = %s\n", azColName[i+1], argv[i+1] ? argv[i+1] : "NULL");
        share_price = atoi(argv[i+1]);
        cout<<"price of stock: "<<share_price<<endl;
        i=i+1;
    }
    return 0;
}
void Account::search_forstock(string stock){//functions searches to insure the stock is available for the client
    double price;
    
    char *err_msg = 0;
    string s;
    
    string m = " Select * FROM price where name = '";
    s = m + stock + "'";
    // used to see the print statement
    cout << s << endl;
    
    char *str = &s[0];
    char* query =  str;
    int res = 0;
    
    res = sqlite3_exec(dbfile, query, callback3, 0, &err_msg);
    cout<<res<<endl;
    
    if (res != SQLITE_OK )
    {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        exit(-1);
    }
    if ( res == SQLITE_DONE || res==SQLITE_ERROR)
    {
        cout << "stocks loaded " << endl;
    }
    if( res > 0)
    {
        res = price;
        throw price;
    }
}
int callback1(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    for (int i = 0; i < argc; i++) {
        
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        // printf("%s = %s\n", azColName[i+1], argv[i+1] ? argv[i+1] : "NULL");
        stock_v.push_back(atoi(argv[i+1]));
        
        i=i+2;
    }
    
    
    return 0;
}
void Account::readstock_price()
{// reads the current price of the stock from the updated file. and shows the user there stocks current value
    char *err_msg = 0;
    string s;
    
    string m = " Select price.name, price.value from price INNER JOIN stocks ON stocks.stock = price.name where stocks.id = '";
    s = m + id_key + "'";
    // used to see the print statement
    cout << s << endl;
    
    char *str = &s[0];
    char* query =  str;
    int res = 0;
    
    res = sqlite3_exec(dbfile, query, callback1, 0, &err_msg);
    if (res != SQLITE_OK )
    {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        exit(-1);
    }
    if ( res == SQLITE_DONE || res==SQLITE_ERROR)
    {
        cout << "stocks loaded " << endl;
    }
    // copying vectors
    int i;
    for(i = 0; i < stock_v.size(); i++)
    {
        stock_price.push_back(stock_v[i]);
        //cout<<stock_v[i]<<endl;
        
    }
}
int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {
        
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //printf("%s = %s\n", azColName[i+1], argv[i+1] ? argv[i+1] : "NULL");
        stock_n.push_back(argv[i+1]);
        // printf("%s = %s\n", azColName[i+2], argv[i+2] ? argv[i+2] : "NULL");
        stock_p.push_back(stoi(argv[i+2]));
        i=i+2;
        
    }
    return 0;
}
int callback2(void *NotUsed, int argc, char **argv, char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {
        
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        printf("%s = %s\n", azColName[i+1], argv[i+1] ? argv[i+1] : "NULL");
        printf("%s = %s\n\n\n", azColName[i+2], argv[i+2] ? argv[i+2] : "NULL");
        i=i+2;
        
    }
    
    
    return 0;
}
void Account::read_accountinfo()
{// reads in the users profilo include balance. companies they own stock in and how many shares they own
    

        char *err_msg = 0;
        string s;
        string m = "Select * from stocks where id = '";
        s = m + id_key + "'";
        // used to see the print statement
        cout << s << endl;
        
        char *str = &s[0];
        char* query =  str;
        int res = 0;
        
            res = sqlite3_exec(dbfile, query, callback, 0, &err_msg);
            if (res != SQLITE_OK )
                {
                
                    fprintf(stderr, "Failed to select data\n");
                    fprintf(stderr, "SQL error: %s\n", err_msg);
                    
                    sqlite3_free(err_msg);
                    exit(-1);
                }
                    if ( res == SQLITE_DONE || res==SQLITE_ERROR)
                    {
                    cout << "stocks loaded " << endl;
                    }
        // copying vectors
            int i;
    for(i = 0; i < stock_n.size(); i++)
    {
        stock_name.push_back(stock_n[i]);
        number_shares.push_back(stock_p[i]);
        
    }
    readstock_price();
}

void  Account::display_profolio(){//displays the users complete profilo for there eyes when they are logged into the account
    
    // reads the current price of the stock from the updated file. and shows the user there stocks current value
    char *err_msg = 0;
    string s;
    cout<< "Account Balance: "<< balance<<endl<<endl;
    string m = " Select price.name, price.value, stocks.shares_number from price INNER JOIN stocks ON stocks.stock = price.name where stocks.id = '";
    s = m + id_key + "'";
    // used to see the print statement
     cout << s << endl;
    
    char *str = &s[0];
    char* query =  str;
    int res = 0;
    
    res = sqlite3_exec(dbfile, query, callback2, 0, &err_msg);
    if (res != SQLITE_OK )
    {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);
        exit(-1);
    }
    if ( res == SQLITE_DONE || res==SQLITE_ERROR)
    {
        cout << "stocks loaded " << endl;
    }
}
void Account::menu(){// displays the menu for the user to decide what they want to do whenever they are logged into there account
    //User function
    int choice = 0;
    //cout<<"id: "<< id<<endl;
    try{
    read_accountinfo();
    }
    
    catch( int reading_error)
    {
    	if(reading_error == 0)
    	{
        cout<<"Error reading the users file"<<endl;
        return;
    	}
    	else
    	{
    		cout << "unable to update stock values" << endl;
    		return;
    	}
    }
    catch( char prompt )//What is this doing?  Please comment.
    {
        
    }
    do{
    try{
    if( account_type == 5 )// all opitions available to the user
        {
        cout<<"Select what you would like to do"<<endl
        <<"\t1: Buy stocks: "<<endl
        <<"\t2: Sell stocks: "<<endl
        <<"\t3: Set stock to buy or sell: "<<endl
        <<"\t4: Display protfolio: "<<endl
        <<"\t5: To Exit your protfolio: "<<endl;
        
        while(!(cin >> choice)){
        			cin.clear();

        			while(cin.get() != '\n')
        				continue;

        				cout << "Invalid input.  Try again: ";
        		}

        switch( choice ){
        
            case 1: // Buy stock
            {
            	string name;
            	int number;
            	cout << "Enter name of stock to buy: " << endl;
            	cin >> name;
            	cout << "Enter the number of Stocks you would like to buy: " << endl;

            	while(!(cin >> number)){
            		cin.clear();

            		while(cin.get() != '\n')
            			continue;

            			cout << "Invalid input.  Try again: ";
            	}
            	buyStocks(name, number);
                break;
            }
            case 2: //Sell stock
            {
            	string name;
            	int number;
            	cout << "Enter name of stock to sell: " << endl;
            	cin >> name;
            	cout << "Enter the number of Stocks you would like to sell: " << endl;

            	while(!(cin >> number)){
            		cin.clear();

            		while(cin.get() != '\n')
            			continue;

            			cout << "Invalid input.  Try again: ";
            	}
                number = 0 - number; // should turn the number negative
            	buyStocks(name, number);
                break;
            }
            case 3:
            {// set stock to buy or sell
                string wanted;
                cout<<"Enter the name of the stock you would like to bye"<<endl;
                cin>>wanted;
                
                search_forstock(wanted);
                break;
            }
            case 4:// display profolio
                display_profolio();
                break;
            case 5:// implement a throw to exit the program
                break;
            }
        }
    }//for try statement

    catch(int menuError)//throw statement from the entire try block above3
    {
    	switch(menuError)
    	{
    	case 1: cout << "Not enough money in account" << endl;
    	break;

    	case 11: cout << "Stock not found" << endl;
    	break;

    	case 2: cout << "Not enough stocks to sell" << endl;
    	break;

    	case 21: cout << "Stock not found" << endl;
    	break;

    	case 91: cout << "Unable to load stock values" << endl;
    	break;

    	default: cout << "Unknown error has occurred" << endl;
    	break;
    	}
    }
    }while( choice !=5 );
    
    cout<<"You are logged out"<<endl<<endl;
    
}
void Log_in::locate_account(string entered_id, string entered_password )
{// when user logs in it verifies that there information matches an account located in account.txt
    int type;
    int input_error = 1;
   string user_id, password;
    ifstream input;
    char out;

        database_connection( entered_id, entered_password);
        type = stoi(success);
    
                if( type == 5 ){//will create a user class
                    Account user(user_id, password, type);
                    user.menu();
                    out = 1;
                    throw out;
                }
                if( type == 1){
                    id_key = id;
                    Administrator user(user_id, password, type);//will create an admin class when type is
                    user.menu();
                    out = '1';
                    throw out;
        
        }
      throw 1;
}
Account:: Account(string user_id, string password, int type){//account class constructor with parameters
    account_id = user_id;
    account_password = password;
    account_type = type;
    
}
void Log_in:: login(){ //login is a seperate function not tieing to any object we change to this design to allow for the program to be an endless loop. before it would treminate because the deconstuctor would be called
    string user_id; string password, space;
    int incorrect = 1;
    string exit;
    do{
       
        try
        {
            if( incorrect == 2)
        {
            cout<<"************Incorrect User ID or Password:***********"<<endl<<"try again"<<endl;
            cout<<"if you would like to exit type exit, if not hit enter  "<<endl;
            getline(cin, exit);
        }
    cout<<"****************Welcome **************"<<endl;
    cout<<"Please enter in your user id: "<<endl;
    getline(cin, user_id);
    cout<<"Please enter in your password: "<<endl;
    getline(cin, password);
    locate_account( user_id, password);//seeing if their information is there
            incorrect = 2;
        }
        
       
        
        
        catch( int no_loggin)
        {
            if(no_loggin == 1)
                cout<<"Unsuccesful login"<<endl;
            incorrect = 2;
        }
        catch(...)
        {
            getline(cin, space);
            //empty catch to bypass the other catch and so we can throw out of the locate_account
            for( int i = 0; i < 1000; i++)
            {
                cout<<endl;
            }
        }
       

    }while( exit != "exit");   //This doesn't work.  Needs error checked..
   
    return;
}
int main( int argc, char** argv){//its one short main but shows the power of C++ with objects and it is beautiful beautiful
    try{
        Log_in user;
        user.login();
    }
    
    catch( int input_error)
    {
        cout<<"Error opening the accounts file"<<endl;// should ever reach this point thus this is a safe does we would know something is wayyyy wrong with login
        
    }
    return 0;
}
