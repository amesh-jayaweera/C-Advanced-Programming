// jamming peaches
// non STL-algorithm code to be replaced by algorthms
// Mikhail Nesterenko
// 9/30/2021

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <deque>
#include <string>

using std::cin; using std::cout; using std::endl;
using std::string;
using std::vector; using std::deque;

struct Peaches{
   double weight; // oz
   bool ripe;  // ripe or not
   void print() const { cout << (ripe?"ripe":"green") << ", " <<  weight << endl; }
};

int main(){
   srand(time(nullptr));
   const double minWeight = 8.;
   const double maxWeight = 3.;

   cout << "Input basket size: ";
   int size;
   cin >> size;

   vector <Peaches> basket(size);

   // assign random weight and ripeness peaches in the basket
   // replace with generate()
   generate(basket.begin(), basket.end(), [&maxWeight,&minWeight](){
       Peaches peach;
       peach.weight = minWeight + 
	           static_cast<double>(rand())/RAND_MAX*(maxWeight - minWeight);
      peach.ripe = rand() % 2;
      return peach;
   });

   // for_each() possibly
   cout << "all peaches"<< endl;
   for_each(basket.begin(), basket.end(), [](Peaches peach) { peach.print(); });

   // moving all the ripe peaches from basket to peck
   // remove_copy_if() with back_inserter()/front_inserter() or equivalents
   deque<Peaches> peck;
   remove_copy_if(basket.begin(), basket.end(), std::back_inserter(peck), [](Peaches peach){return !peach.ripe;});  
   basket.erase(remove_if(basket.begin(), basket.end(), [](Peaches peach) { return peach.ripe;}), basket.end());

   // for_each() possibly
   cout << "peaches remainng in the basket"<< endl;
   for_each(basket.begin(), basket.end(), [](Peaches peach) { peach.print(); });

   cout << endl;

   // for_each() possibly
   cout << "peaches moved to the peck"<< endl;
   for_each(peck.begin(), peck.end(), [](Peaches peach) { peach.print(); });

   // prints every "space" peach in the peck
   // 
   const int space=3; 
   cout << "\nevery " << space << "\'d peach in the peck"<< endl;

   // replace with advance()/next()/distance()
   // no iterator arithmetic
   auto it = peck.cbegin();
    while (it < peck.end()) {
        (*it).print();
        std::advance(it, space);
    }

   // putting all small ripe peaches in a jam
   // use a binder to create a functor with configurable max weight
   // accumulate() or count_if() then remove_if()
   const double weightToJam = 10.0;
   double jamWeight = 0; 

   int removed_elements = count_if(peck.begin(), peck.end(), [&weightToJam, &jamWeight](Peaches peach) { if(peach.weight < weightToJam) jamWeight += peach.weight; return peach.weight < weightToJam;});
   peck.erase(remove_if(peck.begin(), peck.end(), [&weightToJam](Peaches peach) { return peach.weight < weightToJam;}), peck.end());	 

   cout << "Weight of jam is: " << jamWeight << endl;
}