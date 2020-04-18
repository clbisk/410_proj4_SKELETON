#include <mutex>
#include <string>

#include "../includes/baker.h"
#include "../includes/externs.h"
using namespace std;

//ID is just a number used to identify this particular baker 
//(used with PRINT statements)
Baker::Baker(int id):id(id) { }

Baker::~Baker() { }

//bake, box and append to anOrder.boxes vector
//if order has 13 donuts there should be 2 boxes
//1 with 12 donuts, 1 with 1 donut
void Baker::bake_and_box(ORDER &anOrder) {
	//bake the donuts: create a Donut object for number_donuts 
	//(I recommend storing them in a vector for the next step) -- Cecilia
	int numDonuts = anOrder.number_donuts;
	vector<DONUT> donuts(numDonuts);
	
	//box the donuts and add them to the order
	for (int i = 0; i < anOrder.number_donuts; i += 12) {
		int donutsLeft = (anOrder.number_donuts - i);
		Box newBox = Box();

		//add 12 Donuts, or until you are out of baked Donuts
		if (donutsLeft < 12) {
			//add donutsLeft to the box
			for (int j = 0; j < donutsLeft; j++) {
				newBox.addDonut(donuts.front());
				//remove the donut just added
				donuts.erase(donuts.begin());
			}
		}
		else {
			//add 12 donuts to the box
			for (int j = 0; j < 12; j++) {
				newBox.addDonut(donuts.front());
				//remove the donut just added 
				donuts.erase(donuts.begin());
			}
		}

		anOrder.boxes.push_back(newBox);
	}
}

//as long as there are orders in order_in_Q then
//for each order:
//	create box(es) filled with number of donuts in the order
//  then place finished order on order_outvector
//  if waiter is finished (b_WaiterIsFinished) then
//  finish up remaining orders in order_in_Q and exit
//
//You will use cv_order_inQ to be notified by waiter 
//when either order_in_Q.size() > 0 or b_WaiterIsFinished == true
//hint: wait for something to be in order_in_Q or b_WaiterIsFinished == true
void Baker::beBaker() {
//	cout << "hello baker starting" << endl;

	{
		//get the lock for checking b_waiterIsFinished so it isn't being read and written simultaneously
		unique_lock<mutex> inQ_lock(mutex_order_inQ);

		if (!b_WaiterIsFinished) {
			//wait until the waiter is done touching the queue before we try to read it
			cv_order_inQ.wait(inQ_lock);
		}
	}

	while (true) {
		ORDER currOrder = ORDER();

		//if there's something to bake, bake it
		{
			//get the lock for checking the order_in_Q so it isn't being read and written simultaneously
			unique_lock<mutex> inQ_lock(mutex_order_inQ);

			if (!order_in_Q.empty()) {
				currOrder = order_in_Q.front();

				//remove the first ORDER in the in_Q
				order_in_Q.pop();
			}
		}

		//if we just got an order from in_Q, fill it and add it to the out_Q
		//(doesn't need to be protected by the previous inQ_lock)
		if (currOrder.order_number != UNINITIALIZED) {
			//bake Donuts and fill them into Boxes
			bake_and_box(currOrder);

			//nobody touch my out queue
			lock_guard<mutex> lock(mutex_order_outQ);

			//next add the order to the outVector
			order_out_Vector.push_back(currOrder);
		}

		{
			//get the lock for checking b_waiterIsFinished so it isn't being read and written simultaneously
			//also protects checking if in_Q is empty
			unique_lock<mutex> inQ_lock(mutex_order_inQ);

			//don't exit unless we're sure the waiter has read all the orders
			if (!b_WaiterIsFinished) {
				//if it wasn't done, wait for one more
				cv_order_inQ.wait(inQ_lock);
				//now we have something in order_in_Q so we can loop

			} else if (order_in_Q.empty()) {
				//the waiter is done and the queue is empty; I can clock out
				break;
			}
		}
	}
//	cout << "bye bye baker" << endl;
}
