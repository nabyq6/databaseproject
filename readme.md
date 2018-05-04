###**CS3380 Final Project**
####**Team Members**

* Nicholas Bouckaert
* Joshua Eastman
* Tsiania Hughes
* Kenneth Shaughnessy

####**Abstract**
This application is a tool that allows individuals to be entered into a database where they can log in and keep track of the stocks they buy and sell, as well as the amount of money they are budgeting for the stock market. The application is coded with algorithms that automatically update the user's balance before and after stock transactions. They can also pull their entire profile and we what stocks they currently own. Admin level users can see the contents of the entire database, as well as their personal records. 

####**Schema**

Table: Accounts
|Field   |Type   |NULL   | Key   |Extra    |
|------- |:-----:|:------:|:-------:| :---------:|
|ID      |int    |No     |Primary|Auto-Increment|
|User    |char(30)|Yes   |          |           |
|Password    |char(30)|Yes   |          |      |
|Permission    |int|Yes   |          |           |
|Balance    |double|Yes   |          |           |

<BR>
Table: Price
|Field   |Type   |NULL   |
|------- |:-----:|:------:|
|Name      |char(30)    |No     |
|Value    |int      |Yes   | 

<BR>
Table: Stocks
|Field   |Type   |NULL   | 
|------- |:-----:|:------:|
|ID      |int    |No     |
|Stock    |char(30)|Yes   |
|Share_Number      |int    |Yes     |




####**Entity Relationship Diagram**
https://drive.google.com/file/d/1Y4RSFs0ZtIlT8URlvaqN53LkzIp3vZPn/view?usp=sharing

####**CRUD**
The application exbibits full CRUD functionality in the following ways: creating new users in the database, reading all of the records in the database for admin level users, allowing users to update their profile by buying and selling a certain amount of stocks as well as changing their password for their account, and the capability to delete users also for admin level users. 

####**Video Demonstration (Link):** https://youtu.be/AsibT5uRaSc