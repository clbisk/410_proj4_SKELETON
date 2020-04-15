#include <string>
#include "stdlib.h"

#include "../includes/waiter.h"
#include "../includes/externs.h"

using namespace std;

Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){ }

Waiter::~Waiter(){ }

//gets next Order(s) from file_IO
int Waiter::getNext(ORDER &anOrder) {
	//myIO will automatically be initialized the first time myIO.getNext() is called
	return myIO.getNext(anOrder);	//file_IO.getNext() already returns either success if anOrder written with an error, or the right error code
}

//contains a loop that will get orders from filename one at a time
//then puts them in order_in_Q then signals baker(s) using cv_order_inQ
//so they can be consumed by baker(s)
//when finished exits loop and signals baker(s) using cv_order_inQ that
//it is done using b_WaiterIsFinished
void Waiter::beWaiter() {
	//while a next order can be successfully gotten
	ORDER nextOrder = ORDER();
	int nextGotten = getNext(nextOrder);
	while (nextGotten == SUCCESS) {
		unique_lock<mutex> lck(mutex_order_inQ);

		//put into order_in_Q
		order_in_Q.push(nextOrder);

		//notify all the bakers there is a new order
		cv_order_inQ.notify_all();

		//update the next order to run through the loop again
		nextGotten = getNext(nextOrder);
	}

	//signal that it is done taking orders
	b_WaiterIsFinished = true;
}
