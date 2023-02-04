// Author: Syed Shaban
// System: Ubuntu/Linux
// Program Overview: Takes in database of pages and their data, and can take a query consisting of search terms
// 		and different modifiers in order to narrow down the search. Returns the pages that match the
// 		search conditions provided and some other statistics.
//
// Creative component: Stop words - The program will stop certain stopwords from entering the inverted index
// 			which should allow for slightly better runtimes and a better use of computer resources.
// 			Can be found at the end of the CleanToken function.

#include <iostream>
#include "search.h"
using namespace std;

int main() {
    searchEngine("cplusplus.txt");	
    return 0;
}

