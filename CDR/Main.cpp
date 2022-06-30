#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <fstream>
#include <vector>
#include <sstream>
#include <queue>
#include <thread>
#include <mutex>
#include <ctime>

using namespace std;

const int CUSTOMER_SIZE = 20000;
const int OPERATOR_SIZE = 500;

class RawEntry {
   public:
      // MSISDN
      int val1;
      // Operator Brand Name
      string val2;
      // Operator MMC/MNC
      int val3;
      // Call type
      string val4;
      // Duration: in seconds
      int val5;
      // Download: MB downloaded if type is GPRS
      int val6;
      // Upload: MB uploaded if type is GPRS
      int val7;
      // Third party MSISDN (-1 indicates empty)
      int val8;
      // Third party operator MMC/MNC:
      int val9;
      RawEntry(string val1, string val2, string val3, string val4, string val5, string val6, string val7, string val8, string val9) {
         this->val1 = stoi(val1);
         this->val2 = val2;
         this->val3 = stoi(val3);
         this->val4 = val4;
         this->val5 = stoi(val5);
         this->val6 = stoi(val6);
         this->val7 = stoi(val7);
         this->val8 = ((val8 == "") ? -1 : stoi(val8));
         this->val9 = stoi(val9);
      }
};

class CustomerBillingEntry {
   public:
      int key;
      // Outgoing voice calls duration to a subscriber within the mobile operator
      int val1;
      // Incoming voice calls duration to a subscriber within the mobile operator
      int val2;
      // Outgoing voice calls duration to a subscriber outside the mobile operator
      int val3;
      // Incoming voice calls duration to a subscriber outside the mobile operator
      int val4;
      // SMS messages sent within the mobile operator
      int val5;
      // SMS messages received within the mobile operator
      int val6;
      // SMS messages sent outside the mobile operator
      int val7;
      // SMS messages received outside the mobile operator
      int val8;
      // MB downloaded
      int val9;
      // MB uploaded
      int val10;
      CustomerBillingEntry(int key) {
         this->key = key;
         this->val1 = 0;
         this->val2 = 0;
         this->val3 = 0;
         this->val4 = 0;
         this->val5 = 0;
         this->val6 = 0;
         this->val7 = 0;
         this->val8 = 0;
         this->val9 = 0;
         this->val10 = 0;
      }
};

class OperatorEntry {
   public:
      int key;
      // Incoming voice call durations
      int val1;
      // Outgoing voice call durations
      int val2;
      // Incoming SMS messages
      int val3;
      // Outgoing SMS messages
      int val4;
      // MB downloaded
      int val5;
      // MB uploaded
      int val6;
     
      OperatorEntry(int key) {
         this->key = key;
         this->val1 = 0;
         this->val2 = 0;
         this->val3 = 0;
         this->val4 = 0;
         this->val5 = 0;
         this->val6 = 0;
      }
};

class CutomerHashTable {
   private:
      CustomerBillingEntry **t;
   public:
      CutomerHashTable() {
         t = new CustomerBillingEntry * [CUSTOMER_SIZE];
         for (int i = 0; i< CUSTOMER_SIZE; i++) {
            t[i] = NULL;
         }
      }

      int HashFunc(int k) {
         return k % CUSTOMER_SIZE;
      }

      void AddData(int k,int val1, int val2, int val3, int val4, int val5, int val6, int val7, int val8, int val9, int val10) {
         int h = HashFunc(k);
         while (t[h] != NULL && t[h]->key != k) {
            h = HashFunc(h + 1);
         }
         if (t[h] == NULL)
            t[h] = new CustomerBillingEntry(k);
         t[h]->val1 = t[h]->val1 + val1;
         t[h]->val2 = t[h]->val2 + val2;
         t[h]->val3 = t[h]->val3 + val3;
         t[h]->val4 = t[h]->val4 + val4;
         t[h]->val5 = t[h]->val5 + val5;
         t[h]->val6 = t[h]->val6 + val6;
         t[h]->val7 = t[h]->val7 + val7;
         t[h]->val8 = t[h]->val8 + val8;
         t[h]->val9 = t[h]->val9 + val9;
         t[h]->val10 = t[h]->val10 + val10;
      }

      void PrintTable() {
        cout << "The following data would be printed for each customer\n";
        cout << "MSISDN\n";
        cout << "Outgoing voice calls duration to a subscriber within the mobile operator (val1)\n";
        cout << "Incoming voice calls duration to a subscriber within the mobile operator (val2)\n";
        cout << "Outgoing voice calls duration to a subscriber outside the mobile operator (val3)\n";
        cout << "Incoming voice calls duration to a subscriber outside the mobile operator (val4)\n";
        cout << "SMS messages sent within the mobile operator (val5)\n";
        cout << "SMS messages received within the mobile operator (val6)\n";
        cout << "SMS messages sent outside the mobile operator (val7)\n";
        cout << "SMS messages received outside the mobile operator (val8)\n";
        cout << "MB downloaded (val9)\n";
        cout << "MB uploaded (val10)\n\n";
        cout << "MSISDN | val1 | val2 | val3 | val4 | val5 | val6 | val7 | val8 | val9 | val10\n";

        for (int i = 0; i < CUSTOMER_SIZE; i++) {
            if (t[i] != NULL) {
                cout << t[i]->key << " | " << t[i]->val1 << " | " << t[i]->val2 << " | " << t[i]->val3 << " | " << t[i]->val4 << " | " << t[i]->val5 << " | " << t[i]->val6 << " | " << t[i]->val7 << " | " << t[i]->val8 << " | " << t[i]->val9 << " | "<< t[i]->val10 << "\n";
            }
        }
      }
};

class OperatorHashTable {
   private:
      OperatorEntry **t;
   public:
      OperatorHashTable() {
         t = new OperatorEntry * [OPERATOR_SIZE];
         for (int i = 0; i< OPERATOR_SIZE; i++) {
            t[i] = NULL;
         }
      }

      int HashFunc(int k) {
         return k % OPERATOR_SIZE;
      }

      void AddData(int k,int val1, int val2, int val3, int val4, int val5, int val6) {
         int h = HashFunc(k);
         while (t[h] != NULL && t[h]->key != k) {
            h = HashFunc(h + 1);
         }
         if (t[h] == NULL)
            t[h] = new OperatorEntry(k);
         t[h]->val1 = t[h]->val1 + val1;
         t[h]->val2 = t[h]->val2 + val2;
         t[h]->val3 = t[h]->val3 + val3;
         t[h]->val4 = t[h]->val4 + val4;
         t[h]->val5 = t[h]->val5 + val5;
         t[h]->val6 = t[h]->val6 + val6;
      }

      void PrintTable() {
        cout << "The following data would be printed for each operator\n";
        cout << "MMC/MNC\n";
        cout << "Incoming voice call durations (val1)\n";
        cout << "Outgoing voice call durations (val2)\n";
        cout << "Incoming SMS messages (val3)\n";
        cout << "Outgoing SMS messages (val4)\n";
        cout << "MB downloaded (val5)\n";
        cout << "MB uploaded (val6)\n\n";
        cout << "MSISDN | val1 | val2 | val3 | val4 | val5 | val6\n";

        for (int i = 0; i < OPERATOR_SIZE; i++) {
            if (t[i] != NULL) {
                cout << t[i]->key << " | " << t[i]->val1 << " | " << t[i]->val2 << " | " << t[i]->val3 << " | " << t[i]->val4 << " | " << t[i]->val5 << " | " << t[i]->val6 << "\n";
            }
        }
      }
};

// shared queue
queue<RawEntry> pipe;
// mutex for pipe
mutex mutexPipe;
// mutex for DB
mutex mutexDB;
bool praserFinished = false;
// databases
CutomerHashTable cutomerDB;
OperatorHashTable operatorDB;

void FilePraser() {
   string line;
   // open CDR file 
   ifstream CDR("file.cdr");
   // read line by line
   while (getline (CDR, line)) {
      string array[9];
      stringstream ss(line);
      int i = 0;
      while( ss.good() )
      {
         string substr;
         getline( ss, substr, '|' );
         array[i] = substr;
         i++;
      }

      RawEntry* raw = new RawEntry(array[0], array[1], array[2], array[3], array[4], array[5], array[6], array[7], array[8]);
      mutexPipe.lock();
      pipe.push(*raw);
      mutexPipe.unlock();
   }
   // close file
   CDR.close();
   praserFinished = true;
}

void AggregateData() {

   int msisdn1, msisdn2;
   int mmc1, mmc2;
   int duration, download, upload;
   string type;
   
   while((!praserFinished || pipe.size() > 0)) {
      mutexPipe.lock();
      if(pipe.size() > 0) {
         RawEntry data = pipe.front();
         pipe.pop();
         mutexPipe.unlock();

         msisdn1 = data.val1;
         mmc1 = data.val3;
         type = data.val4;
         duration = data.val5;
         download = data.val6;
         upload = data.val7;

         mutexDB.lock();
         if(type == "MOC") { // "MOC: outgoing voice call"
            operatorDB.AddData(mmc1, 0, duration, 0, 0, download, upload);
            if(mmc1 == mmc2) { // within mobile operator
               cutomerDB.AddData(msisdn1, duration, 0, 0, 0, 0, 0, 0, 0, download, upload);
            } else {
               cutomerDB.AddData(msisdn1, 0, 0, duration, 0, 0, 0, 0, 0, download, upload);
            }
         } else if(type == "MTC") { // MTC: incoming voice call
            operatorDB.AddData(mmc1, duration, 0, 0, 0, download, upload);
            if(mmc1 == mmc2) { // within mobile operator
               cutomerDB.AddData(msisdn1, 0, duration, 0, 0, 0, 0, 0, 0, download, upload);
            } else {
               cutomerDB.AddData(msisdn1, 0, 0, 0, duration, 0, 0, 0, 0, download, upload);
            }
         } else if(type == "SMS-MO") { // SMS-MO: outgoing message
            operatorDB.AddData(mmc1, 0, 0, 0, 1, download, upload);
            if(mmc1 == mmc2) { // within mobile operator
               cutomerDB.AddData(msisdn1, 0, 0, 0, 0, 1, 0, 0, 0, download, upload);
            } else {
               cutomerDB.AddData(msisdn1, 0, 0, 0, 0, 0, 0, 1, 0, download, upload);
            }
         } else if(type == "SMS-MT") { // SMS-MT: incoming message
            operatorDB.AddData(mmc1, 0, 0, 1, 0, download, upload);
            if(mmc1 == mmc2) { // within mobile operator
               cutomerDB.AddData(msisdn1, 0, 0, 0, 0, 0, 1, 0, 0, download, upload);
            } else {
               cutomerDB.AddData(msisdn1, 0, 0, 0, 0, 0, 0, 0, 1, download, upload);
            }
         } else if(type == "GPRS") { // GPRS: internet
            operatorDB.AddData(mmc1, 0, 0, 0, 0, download, upload);
            cutomerDB.AddData(msisdn1, 0, 0, 0, 0, 0, 0, 0, 0, download, upload);
         }
         mutexDB.unlock();
      } else {
         mutexPipe.unlock();
      }
   }
}
  
int main() {

   const clock_t begin_time = clock();

   // init threads
   thread fileParserThread(FilePraser);
   thread aggregateDataThread1(AggregateData);
   thread aggregateDataThread2(AggregateData);
   thread aggregateDataThread3(AggregateData);
   thread aggregateDataThread4(AggregateData);
   thread aggregateDataThread5(AggregateData);
  
   // wait for all threads to finish 
   fileParserThread.join();
   aggregateDataThread1.join();
   aggregateDataThread2.join();
   aggregateDataThread3.join();
   aggregateDataThread4.join();
   aggregateDataThread5.join();

   cout << "Time Taken : " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

   // print customer DB 
   cutomerDB.PrintTable();

   // print operatorDB
   operatorDB.PrintTable();

   return 0;
}