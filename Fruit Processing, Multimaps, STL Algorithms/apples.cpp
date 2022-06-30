// sorting apples
// non STL-algorithm code to be replaced by algorthms
// Mikhail Nesterenko
// 09/30/2021


#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <deque>
#include <string>

using std::cin; using std::cout; using std::endl;
using std::string;
using std::vector; using std::deque;

struct Apples{
   double weight; // oz
   string color;  // red or green
   void print() const { cout << color << ", " <<  weight << endl; }
};


int main(){
   srand(time(nullptr));
   const double minWeight = 8.;
   const double maxWeight = 3.;

   cout << "Input crate size: ";
   int size;
   cin >> size;

   vector <Apples> crate(size);

   // assign random weight and color to apples in the crate
   // replace with generate()
   generate(crate.begin(), crate.end(), [&maxWeight,&minWeight](){
       Apples apple;
       apple.weight = minWeight + 
	           static_cast<double>(rand())/RAND_MAX*(maxWeight - minWeight);
      apple.color = rand() % 2 == 1 ? "green" : "red";
      return apple;
   });

   // for_each() possibly
   cout << "all appleas"<< endl;
   for_each(crate.begin(), crate.end(), [](Apples apple) { apple.print(); });


   cout << "Enter weight to find: ";
   double toFind;
   cin >> toFind;

   // count_if()
   int cnt = 0;
   cnt = count_if(crate.cbegin(), crate.cend(), [&toFind](Apples apple) { return apple.weight > toFind;});

    cout << "There are " << cnt << " apples heavier than " 
	<< toFind << " oz" <<  endl;

     // find_if()
    cout << "at positions ";
    auto it = find_if(begin(crate), end(crate), [&toFind](Apples apple) { return apple.weight > toFind; });
    while (it != end(crate)) {
        cout << distance(begin(crate), it) << ", ";
        it = find_if(next(it), end(crate), [&toFind](Apples apple) { return apple.weight > toFind; });
    }
    cout << endl;

   // max_element()
   double heaviest = max_element(crate.begin(), crate.end(), [](Apples a, Apples b){ return a.weight < b.weight;})->weight;
   cout << "Heaviest apple weighs: " << heaviest << " oz" << endl;


   // for_each() or accumulate()
   double sum = 0;
   for_each(crate.begin(), crate.end(), [&sum](Apples apple) { sum += apple.weight; });
   cout << "Total apple weight is: " << sum << " oz" << endl;

   // transform();
   cout << "How much should they grow: ";
   double toGrow;
   cin >> toGrow;
   transform(crate.begin(), crate.end(), crate.begin(), [&toGrow](Apples apple) { apple.weight += toGrow; return apple; }); 

   // remove_if()
   cout << "Input minimum acceptable weight: ";
   double minAccept;
   cin >> minAccept;
   
   crate.erase(remove_if(crate.begin(), crate.end(), [&minAccept](Apples apple) {return apple.weight < minAccept;}), crate.end());	 
   cout << "removed " << size - crate.size() << " elements" << endl;

   // bubble sort, replace with sort()
   sort(crate.begin(), crate.end(), [](Apples a , Apples b) {return a.weight < b.weight;});

   // for_each() possibly
   cout << "sorted remaining apples"<< endl;
   for_each(crate.begin(), crate.end(), [](Apples apple) { apple.print(); });
}