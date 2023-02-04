// Author: Syed Shaban
// System: Ubuntu/Linux
// Program Overview: Takes in database of pages and their data, and can take a query consisting of search terms
// 		and different modifiers in order to narrow down the search. Returns the pages that match the
// 		search conditions provided and some other statistics.
//
// Creative component: Stop words - The program will stop certain stopwords from entering the inverted index
// 			which should allow for slightly better runtimes and a better use of computer resources.
// 			Can be found at the end of the CleanToken function.
//

#pragma once

#include <iostream>
#include <set>
#include <map>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fstream>
#include <sstream>
using namespace std;


// Takes a string and returns the same string without the punctuation 
// from the beginning or the end of the string, and converts all of the 
string cleanToken(string s) {
    
    string res = "";
    int size = s.length();
    int firstLetter = 0;
    int lastLetter = size-1;
    if(size == 0){//checks if the token is empty, if so, returns empty string
    	return "";
    }
    while(firstLetter < size && !isalpha(s[firstLetter])){
        firstLetter++;
    }
    if(firstLetter == size){//checks if there is only punctuation in the entire token, meaning it makes it to the end of the string.
        return "";
    }
    firstLetter = 0;
    lastLetter = size-1;
    while(firstLetter < size && ispunct(s[firstLetter])){//a while loop that finds the first character in the string that isn't punctuation
    	firstLetter++;
    }
    if(firstLetter == size){//checks if there is only punctuation in the entire token, meaning it makes it to the end of the string.
    	return "";
    }
    while(ispunct(s[lastLetter])){//a while loop that finds the last character in the string that isn't punctuation
    	lastLetter--;
    }
    res = s.substr(firstLetter, lastLetter-firstLetter+1);//substring to get the correct portion of existing string.
    int resLength = res.length();
    for(int i = 0; i < resLength; i++){
    	if(isupper(res[i])){//checks if character is an alphanumeric character that is uppercase
		res[i] = tolower(res[i]);//if it is uppercase, makes the character lowercase.
	}
    }
    //Creative component (explained in the header file) - Stop Words, checks if the word that is in the data is a stop word, if it is, then it is discarded from the index, and considered not important
    //Uses to save space in our inverted index, reducing runtime in certain circumstances.
    set<string> stopWords;
    stopWords.insert("for");
    stopWords.insert("the");
    stopWords.insert("a");
    stopWords.insert("if");
    stopWords.insert("in");
    stopWords.insert("an");
    stopWords.insert("nor");
    stopWords.insert("but");
    stopWords.insert("or");
    stopWords.insert("so");
    stopWords.insert("what");
    stopWords.insert("who");
    stopWords.insert("this");
    stopWords.insert("that");
    stopWords.insert("is");
    stopWords.insert("am");
    
    if(stopWords.count(res) == 1){//checks if the token is a stop word, and makes sure that it is not.
	res = "";
    }
    return res;
}

// Takes a given text strand and pushes all of the individual tokens in given string to a set of strings, consisting of unique
// tokens, consisting of words from the given parameter.
set<string> gatherTokens(string text) {
    set<string> tokens;
    int i = 0;
    string temp = "";
    while(text[i] != '\0'){//goes until end of the string
    	while(text[i] != ' '){//finds first space
    		if(text[i] == '\0'){//checks if we are at the end of the string
			break;
		}
		temp += text[i];
		i++;
    	}
	temp = cleanToken(temp);//makes sure the token is in correct format before pushing it to set
	if(temp != ""){//makes sure the token is not empty
		tokens.insert(temp);
	}
	temp = "";
	if(text[i] != '\0'){
		i++;
	}
    }
    
    return tokens;
}


//Takes a filename as a string, which serves as the database for web page data, and a map of strings and a sets of strings,
//which will be filled with data serving as an inverted index after the function is completed. The function
//returns the number of documents processed by the database.
int buildIndex(string filename, map<string, set<string>>& index) {
    ifstream inFile;
    inFile.open(filename);
    string tempURL = "";
    string tempData = "";
    set<string> tempSet;
    set<string>::iterator itr;
    int urls = 0;
    if(!inFile.is_open()){//checks if file has opened properly
    	cout << "Error: unable to open '" << filename << "'" << endl;
	return 0;
    }
    getline(inFile, tempURL);//gets first full word of the file, which is always a URL.
    if(tempURL[0] != 'w' && tempURL[0] != 'h'){//checks if the first word is a URL.
        cout << "Not a database file" << endl;
    }
    if(index.size() > 0){//checks if there was a previous database of pages loaded, and clears the old data if so
    	index.clear();
    }
    while(!inFile.fail()){
    	getline(inFile, tempData);//gets each line of data
	urls++;
	tempSet = gatherTokens(cleanToken(tempData));//puts each word of data into a set of strings for the current URL
	for(itr = tempSet.begin(); itr != tempSet.end(); itr++){//puts the current URL into the set of strings associated with each word from the current URL in the inverted index
		index[*itr].insert(tempURL);
	}
	getline(inFile, tempURL);
    }
    

    return urls;
}

//a function used to clean any punctuation prior to the operator in case there is any errors given in the queue.
string queryCleaner(string tempWord){
	int i = 0;
	while(!isalpha(tempWord[i]) && tempWord[i] != '\0'){//finds the first letter in the term given in the query
		i++;
	}
	if (i > 0){//if the first character is not a letter
		if(tempWord[i-1] == '+' || tempWord[i-1] == '-'){//check if the character before the first letter is a + or -.
			return tempWord.substr(i-1);//goes from the plus or minus sign until the end of the string.
		}
	}
	else{//if the first character is a letter (should be treated as an OR operator in the main function)
		return tempWord;
	}
	return tempWord;
}

//takes a sentence that contains words and +/- operators to search for through the database that we have,
//and returns a set of strings, which contains the urls of the pages that the query sentence matches.
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {
    set<string> result;
    string tempWord = "";
    map<string, set<string>>::iterator mapItr;
    set<string>:: iterator setItr;
    int i = 0;
    int sentenceLength = sentence.length();
    sentence = cleanToken(sentence);//makes sure that there are no ending punctuation or beginning punctuation in the query and makes them all lowercase
    //Finds the webpages for the first search term
    while(sentence[i] != ' ' && sentence[i] != '\0'){//scans for the first word
    	tempWord += sentence[i++];
    }
    tempWord = cleanToken(tempWord);//makes sure that the word is formatted correctly
    if(index.count(tempWord) == 1){
    	for(setItr = index.at(tempWord).begin(); setItr != index.at(tempWord).end(); setItr++){
		result.insert(*setItr);
	}
    }
    if(sentence[i] != '\0'){//if it is not the end of the string, iterate to the next token
        i++;
    }
    else{
    	return result;//if it is the end of the query, return what you have currently.
    }
    tempWord = "";
    //finds the webpages using the rest of the conditions for the rest of the terms.
    while(i < sentenceLength){//goes until the end of the rest of the string
    	while(sentence[i] != ' ' && sentence[i] != '\0'){//scans for the next term
        	tempWord += sentence[i++];
    	}
	tempWord = queryCleaner(tempWord);//function cleans any punctuation prior to any operators before the term.
	if(tempWord[0] == '+'){//checks for the + operator before each term
		tempWord = cleanToken(tempWord);//makes sure that the word is formatted correctly
		if(index.count(tempWord) == 1){//checks if the word entered is in the entire database
			for(setItr = result.begin(); setItr != result.end(); setItr++){//NOT A TRIPLE LOOP, THIS IS ON THE SAME LEVEL AS THE WHILE LOOP FROM EARLIER, IT IS JUST INDENTED BECAUSE OF IF STATEMENTS
				if(index[tempWord].count(*setItr) == 0){//checks if each URL from existing result is a part of the URLs from the word, finds the ones that are not shared
					result.erase(*setItr);//removes the unshared ones.
				}
			}
		}
	}
	else if(tempWord[0] == '-'){//checks for the - operator before each term
		tempWord = cleanToken(tempWord);//makes sure that the word is formatted correctly
		if(index.count(tempWord) == 1){
                        for(setItr = result.begin(); setItr != result.end(); setItr++){//NOT A TRIPLE LOOP, THIS IS ON THE SAME LEVEL AS THE WHILE LOOP FROM EARLIER, IT IS JUST INDENTED BECAUSE OF IF STATEMENTS
                                if(index[tempWord].count(*setItr) == 1){//checks if the URLs from result are in the URLs matched to the word, erases the ones that are shared.
                                        result.erase(*setItr);
                                }
                        }
                }
	}
	else{
		tempWord = cleanToken(tempWord);//makes sure that the word is formatted correctly
    		if(index.count(tempWord) == 1){
        		for(setItr = index.at(tempWord).begin(); setItr != index.at(tempWord).end(); setItr++){//checks and adds any webpages that contain the new word, NOT A TRIPLE LOOP, THIS IS ON THE SAME LEVEL AS THE WHILE LOOP FROM EARLIER, IT IS JUST INDENTED BECAUSE OF IF STATEMENTS
                		result.insert(*setItr);
        		}
    		}
	}
	i++;
	tempWord = "";
    } 
    return result;
}

//Takes in a filename, denoting the database of which the program should pull webpages and data from, 
//and provides a menu from which to enter query sentences and find webpages that match your query
void searchEngine(string filename) {
	cout << "Stand by while building index..." << endl;
	map<string, set<string>> invertIndex;//inverted index that stores all of the data
    	buildIndex(filename, invertIndex);//function that builds index using the filename given as a parameter and changes the invertIndex map.
	map<string, set<string>>::iterator mapItr;//used to calculate the number of unique terms
	set<string>::iterator itr;//used to iterate through set provided by the findQueryMatches function which performs the actual searching
	set<string> uniqueURLs;
	string input = "";
	for(mapItr = invertIndex.begin(); mapItr != invertIndex.end(); mapItr++){//this double for loop goes through each term stored and adds them to another set of strings to count how many of them are unique over the entire database.
		for(itr = mapItr->second.begin(); itr != mapItr->second.end(); itr++){
			uniqueURLs.insert(*itr);
		}
	}

 	cout << "Indexed " << uniqueURLs.size() << " pages containing " << invertIndex.size() << " unique terms" << endl << endl;
	set<string> result;//the string to store the webpages that are found after running a query
	while(true){	
		cout << "Enter query sentence (press enter to quit): ";
		getline(cin, input);
		if(input == ""){
			cout << "Thank you for searching!" << endl;
			break;
		}
		cout << endl;
		result = findQueryMatches(invertIndex, input);
		cout << "Found " << result.size() << " matching pages" << endl;
		for(itr = result.begin(); itr != result.end(); itr++){
                	cout << *itr << endl;
        	}
		cout << endl;

	}
        return;	
}


