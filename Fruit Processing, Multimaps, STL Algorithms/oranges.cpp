// selecting oranges
// converting vectors to multimaps
// Mikhail Nesterenko
// 2/17/2020

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <deque>
#include <string>
#include <map>

using std::cin; using std::cout; using std::endl;
using std::string;
using std::vector;
using std::multimap;

enum class Variety {orange, pear, apple};
vector<string> colors = {"red", "green", "yellow"};

struct Fruit{
   Variety v;
   string color; // red, green or orange
};

int main(){
   srand(time(nullptr));

   // set size randomly and it indicates number of pair elements in the map
   int size = rand()%100+1;
   // define multimap with key as Fruit Variety and value as Color
   multimap<Variety, string> tree;

   for(int i=0;i<size;i++) {
       // insert elements in random order
       tree.emplace(static_cast<Variety>(rand() % 3), colors[rand()%3]);
   }

    // select fruit variety randomly
    Variety variety = static_cast<Variety>(rand() % 3);
    string fruit;
    if(variety == Variety::orange) {
        fruit = "Orange";
    } else if(variety == Variety::apple) {
        fruit = "Apple";
    } else {
        fruit = "Peal";
    }

    cout << "Color of the " << fruit << endl;

    for (auto it = tree.lower_bound(variety), end = tree.upper_bound(variety); it != end; ++it) {
        cout << it->second << ",";
    }

   cout << endl;
}