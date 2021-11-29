// replace my comments with your own
// except: you can use my comment about one-word lookahead

// you may modify the code that I have supplied if you wish


using namespace std;

#include "ReadWords.h"
#include <cctype>
#include <cstdlib>
#include <iostream>

string removePunct(string word)
{ // should return a copy of the word with all leading and trailing punctuation      
  // removed
  // punctuation elsewhere in the word should not be removed
  // note that a word could havce more than one leading/trailing punctuation symbols
  // you need to write this

  return word;
}

ReadWords::ReadWords(const char *fname)
{   wordfile.open(fname);//open file
    if (!wordfile)
    {   cout << "Failed to open " << fname << endl;
        exit(1);
    }
    wordfile >> nextword;
    eoffound = false;
}

string ReadWords::getNextWord()
{ // should return next word in file, converted to lower case
  // empty string should be returned if next word contains no letters
  
  // uses a one-word lookahead to avoid any problems relating to when if end-of-file
  // is detected due to absence/presence of newline at end of file

  // incomplete

  string word = nextword;
  wordfile >> nextword;

  if (wordfile.eof())
     eoffound = true;

  word = removePunct(word);

  // need to check that word contains a letter, and if not return an empty string;
  // also need to convert to lower case before returning

  return word;
}

bool ReadWords::isNextWord()
{ return !eoffound;
}

void ReadWords::getPhrases()
{ // you must write this
}

void ReadWords::getWords()
{ // you must write this
}

void ReadWords::close()
{ // you must write this
}
