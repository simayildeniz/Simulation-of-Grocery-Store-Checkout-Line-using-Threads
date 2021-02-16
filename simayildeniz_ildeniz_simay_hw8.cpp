# include <iostream>
# include <string>
# include <chrono>
# include <thread>
# include <mutex>
# include <random>
# include <time.h>
# include <iomanip>
# include <ctime>
# include "HW8DynIntQueue.h"

using namespace std;


HW8DynIntQueue myQueue;

mutex myMutex, coutMutex, mutateQueue; // myMutex is for main thread counter

int maximumCust, threshold_SecondCash;
int totalCust = 0, mainThreatCount = 0;

int min_Cust, max_Cust;
int min_Cash, max_Cash;

bool wait_1 = false;
bool wait_2 = false;

// taken from HW document//
int random_range(const int & min, const int & max) 
{   
	static mt19937 generator(time(0));
	uniform_int_distribution<int> distribution(min, max);
	return distribution(generator); 
} 
//end//
//taken from Lab 13 materials//
void timeFunction()
{
	time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now()); 
	struct tm *ptm = new struct tm;  
	localtime_s(ptm, &tt);  
	cout << "Simulation start " << put_time(ptm,"%X") << endl; 
}
//end//

void arriveAtTheStore(HW8DynIntQueue & myQueue, int & totalCust, const int & maximumCust, int & min_Cust, int & max_Cust)
{
	while (totalCust < maximumCust)
	{
		//taken from Lab 13 //
		time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now()); 
		struct tm *ptm = new struct tm;  
		localtime_s(ptm, &tt);  
		//end//

		totalCust = totalCust + 1; // increase the number of customer
		
		mutateQueue.lock();
		myQueue.enqueue(totalCust); // add to queue
		coutMutex.lock();

		//print message here:
		cout << "New customer with ID " << totalCust << " has arrived (queue size is " << myQueue.getCurrentSize() << "): " << put_time(ptm,"%X") << "\n";

		coutMutex.unlock();
		mutateQueue.unlock();

		this_thread::sleep_for(chrono::seconds(random_range(min_Cust, max_Cust)));
	}

}

void mainThreat(HW8DynIntQueue & myQueue, int & min_Cash, int & max_Cash, int & maximumCust, int cashID)
{
	int temp;

	while (mainThreatCount < maximumCust)
	{
		mutateQueue.lock();

		if (cashID == 1)
		{
			// if not sleeping
			if (wait_1 == false)
			{
				//cout << "dummy 1" << endl;
				mutateQueue.unlock();
				wait_1 = true;

				this_thread::sleep_for(chrono::seconds(random_range(min_Cash, max_Cash)));
			}
			else if (myQueue.isEmpty() == false)
			{
				//cout << "dummy 2" << endl;
				myQueue.dequeue(temp);

				int currentSize = myQueue.getCurrentSize();
				
				myMutex.lock();
				mainThreatCount = mainThreatCount + 1;
				myMutex.unlock();

				//taken from lab 13//
				time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now());  
				struct tm *ptm = new struct tm;  
				localtime_s(ptm, &tt); 
				//end//

				coutMutex.lock();
				cout << "Cashier " << cashID << " started transaction with customer " << temp << " (queue size is " << currentSize << "): " << put_time(ptm,"%X") << "\n";
				coutMutex.unlock();

				mutateQueue.unlock();

				time_t ttt = chrono::system_clock::to_time_t (chrono::system_clock::now());  
				struct tm *ptmT = new struct tm;  
				localtime_s(ptmT, &ttt); 
				this_thread::sleep_for(chrono::seconds(random_range(min_Cash, max_Cash)));

				coutMutex.lock();
				cout << "Cashier " << cashID << " finished transaction with customer " << temp << " " << put_time(ptmT,"%X") << "\n";
				coutMutex.unlock();
			}
			else
			{
				// nothing to do 
				// just unlock the queue mutex
				mutateQueue.unlock();
			}
		}
		else if (cashID == 2)
		{
			// nearly the same for the first cashier except the threshold inputted from the user
			// if not sleeping
			if (wait_2 == false)
			{
				//cout << "dummy 1- 2nd cond" << endl;
				mutateQueue.unlock();
				wait_2 = true;

				this_thread::sleep_for(chrono::seconds(random_range(min_Cash, max_Cash)));
			}
			else if (myQueue.isEmpty() == false)
			{
				int currentSize = myQueue.getCurrentSize();
				// threshold check here:
				if (currentSize >= threshold_SecondCash )
				{
					myQueue.dequeue(temp);
					currentSize = myQueue.getCurrentSize();
					myMutex.lock();
					mainThreatCount = mainThreatCount + 1;
					myMutex.unlock();

					//taken from lab 13//
					time_t tt = chrono::system_clock::to_time_t (chrono::system_clock::now());  
					struct tm *ptm = new struct tm;  
					localtime_s(ptm, &tt); 
					//end//

					coutMutex.lock();
					cout << "Cashier " << cashID << " started transaction with customer " << temp << " (queue size is " << currentSize << "): " << put_time(ptm,"%X") << "\n";
					coutMutex.unlock();

					mutateQueue.unlock();

					//taken from lab 13//
					time_t ttt = chrono::system_clock::to_time_t (chrono::system_clock::now());  
					struct tm *ptmT = new struct tm;  
					localtime_s(ptmT, &ttt); 
					this_thread::sleep_for(chrono::seconds(random_range(min_Cash, max_Cash)));
					//end//

					coutMutex.lock();
					cout << "Cashier " << cashID << " finished transaction with customer " << temp << " " << put_time(ptmT,"%X") << "\n";
					coutMutex.unlock();

				}
				//if threshold is not met:
				else if (myQueue.getCurrentSize() < threshold_SecondCash)
				{
					// just unlock the queue mutex
					wait_2 = false;
					mutateQueue.unlock();
				}
			}
			else
			{
				mutateQueue.unlock();
			}
		}
	}
}

int main()
{
	cout << "Please enter the total number of customers: ";
	cin >> maximumCust;
	cout << "Please enter the number of customers waiting in the queue to open the second cashier: ";
	cin >> threshold_SecondCash;
	cout << "Please enter the inter-arrival time range between two customers: " << endl;
	cout << "Min: ";
	cin >> min_Cust;
	cout << "Max: ";
	cin >> max_Cust;
	cout << "Please enter the checkout time range of cashiers: " << endl;
	cout << "Min: ";
	cin >> min_Cash;
	cout << "Max: ";
	cin >> max_Cash;

	timeFunction(); // also prints thread starts at.... 

	//PARAMETERS FOR MAIN THREAD (i.e. Cashier Thread) : HW8DynIntQueue & myQueue, int & min_Cash, int & max_Cash, int & maximumCust, int cashID
	thread thrCash_1(&mainThreat, ref(myQueue), min_Cash, max_Cash, maximumCust, 1);
	thread thrCash_2(&mainThreat, ref(myQueue), min_Cash, max_Cash, maximumCust, 2);

	//PARAMETERS FOR THE CUSTOMER THREAD : HW8DynIntQueue & myQueue, int & totalCust, const int & maximumCust, int & min_Cust, int & max_Cust
	thread thrCust(&arriveAtTheStore, ref(myQueue), totalCust, maximumCust, min_Cust, max_Cust);

	thrCash_1.join();
	thrCash_2.join();
	thrCust.join();

	cout << "Simualtion ends " << "\n";
	return 0;
}