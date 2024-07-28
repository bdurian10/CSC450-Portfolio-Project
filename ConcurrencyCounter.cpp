/*
*ConcurrencyCounter.cpp
* Created by:bhdur
*Jul 28, 2024
*2:47:17 PM
*/
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

using namespace std;

//Global atomic counter
static atomic<int> counter = 0;

//Mutex and condition variable to sequence tasks
mutex mtx;
condition_variable cv;
bool finished_incrementing = false;

//Function to increment counter
void increment()
{
	for(int i = 0; i < 20; i++)
	{
		counter++;
		cout << "Counter Increment: " << counter << endl;
	}

	//Notify program when incrementing is complete
	{
		lock_guard<mutex> lock(mtx);
		finished_incrementing = true;
	}
	cv.notify_one();
}

//Function to decrement counter
void decrement()
{
	//Wait until incrementing is complete
	unique_lock<mutex> lock(mtx);
	//Use a while loop to prevent spurious wakeup
	while (finished_incrementing != true)
	{
		cv.wait(lock,[]{return finished_incrementing;});
	}
	for(int i = 0; i < 20; i++)
	{
		counter--;
		cout << "Counter Decrement: " << counter << endl;
	}
}

int main()
{
	thread t1(increment);
	thread t2(decrement);

	t1.join();
	t2.join();

	cout << "Final Counter Value: " << counter << endl;

	return 0;
}
