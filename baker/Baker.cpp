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
	//(I recommend storing them in a vector for the next step)
	//...
	int numDonuts = anOrder.number_donuts;
	vector<DONUT> donuts(numDonuts);
	
	//box the donuts and add them to the order
	for (int i = 0; i < anOrder.number_donuts; i += 12) {
		int donutsLeft = (anOrder.number_donuts - i);
		Box newBox = Box();

		//add 12 Donuts, or until you are out of baked Donuts
		//...
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
	while (!order_in_Q.empty()) {	//as long as there are orders in order_in_Q
		//make sure order_in_Q is not locked (wait on cv_order_inQ)

		unique_lock<mutex> lck(mutex_order_outQ);
		ORDER currOrder = order_in_Q.front();

		//bake Donuts and fill them into Boxes
		bake_and_box(currOrder);

		//next add the order to the outVector
		order_out_Vector.push_back(currOrder);

		//remove the first ORDER in the in_Q
		order_in_Q.pop();

		while (!b_WaiterIsFinished) {
			unique_lock<mutex> inQ_lock(mutex_order_inQ);
			cv_order_inQ.wait(inQ_lock);
		}
	}
}
