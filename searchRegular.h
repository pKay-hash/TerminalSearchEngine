// TODO: remove and replace this file header comment
// This is a .h file you will edit and turn in.

// Remove starter comments and add your own
// comments on each function and on complex code sections.

#pragma once

#include <iostream>
#include <set>
#include <map>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fstream>
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
    return res;
}

// Takes a given text strand and pushes all of the individual tokens in given string to a set of strings, consisting of unique
// tokens, consisting of words from the given parameter.
set<string> gatherTokens(string text) {
    set<string> tokens;
    int textSize = text.length();
    int i = 0;
    string temp = "";
    while(text[i] != '\0'){
    	while(text[i] != ' '){
    		if(text[i] == '\0'){
			break;
		}
		temp += text[i];
		i++;
    	}
	temp = cleanToken(temp);
	if(temp != ""){
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
    set<string>::iterator itr;
    while(!inFile.fail()){
    	getline(inFile, tempData);//gets each line of data
	index[tempURL] = gatherTokens(cleanToken(tempData));//adds URL and set of data to inverted index.
	getline(inFile, tempURL);
    }
    
    return 0;
}

//takes a sentence that contains words and +/- operators to search for through the database that we have,
//and returns a set of strings, which contains the urls of the pages that the query sentence matches.
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {
    set<string> result;
    string tempWord = "";
    map<string, set<string>>::iterator mapItr;
    set<string>:: iterator setItr;
    int i = 0;
    sentence = cleanToken(sentence);//makes sure that there are no ending punctuation or beginning punctuation in the query and makes them all lowercase
    //Finds the webpages for the first search term
    while(sentence[i] != ' ' && sentence[i] != '\0'){//scans for the first word
    	tempWord += sentence[i++];
    }
    tempWord = cleanToken(tempWord);//makes sure that the word is formatted correctly
    for(mapItr = index.begin(); mapItr != index.end(); mapItr++){//goes through each webpage in the map
	if(mapItr->second.count(tempWord) == 1){//checks each webpage for the given first word
		result.insert(mapItr->first);//adds the webpages with the given search term to the resulting list.
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
    while(i < sentence.length()){//goes until the end of the rest of the string
    	while(sentence[i] != ' ' && sentence[i] != '\0'){//scans for the next term
        	tempWord += sentence[i++];
    	}
	if(tempWord[0] == '+'){//checks for the + operator before each term
		tempWord = cleanToken(tempWord);//makes sure that the word is formatted correctly
		for(mapItr = index.begin(); mapItr != index.end(); mapItr++){//NOT A TRIPLE LOOP, THIS IS ON THE SAME LEVEL AS THE WHILE LOOP THAT SCANS FOR THE NEXT TERM, IT IS SIMPLY INDENTED BECAUSE IT IS INSIDE OF AN IF STATEMENT.
			if(result.count(mapItr->first) == 1){//checks if the current webpage we are looking at is in our existing resulting webpages
				if(mapItr->second.count(tempWord) == 0){//checks if webpage has the current term
					result.erase(mapItr->first);//removes the webpages without the given search term from the result list.
				}
			}
        	}
	}
	else if(tempWord[0] == '-'){//checks for the - operator before each term
		tempWord = cleanToken(tempWord);//makes sure that the word is formatted correctly
                for(mapItr = index.begin(); mapItr != index.end(); mapItr++){//NOT A TRIPLE LOOP, THIS IS ON THE SAME LEVEL AS THE WHILE LOOP THAT SCANS FOR THE NEXT TERM, IT IS SIMPLY INDENTED BECAUSE IT IS INSIDE OF AN IF STATEMENT.
                        if(result.count(mapItr->first) == 1){//checks if the current webpage we are looking at is in our existing resulting webpages
                                if(mapItr->second.count(tempWord) == 1){//checks if webpage has the current term
                                        result.erase(mapItr->first);//removes the webpages with the given search term from the result list.
                                }
                        }
                }
	}
	else{
		for(mapItr = index.begin(); mapItr != index.end(); mapItr++){//goes through each webpage in the map
			if(mapItr->second.count(tempWord) == 1){//checks each webpage for the given first word
				result.insert(mapItr->first);//adds the webpages with the given search term to the resulting list.
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
	set<string> uniqueTerms;
	string input = "";
	for(mapItr = invertIndex.begin(); mapItr != invertIndex.end(); mapItr++){//this double for loop goes through each term stored and adds them to another set of strings to count how many of them are unique over the entire database.
		for(itr = mapItr->second.begin(); itr != mapItr->second.end(); itr++){
			uniqueTerms.insert(*itr);
		}
	}

 	cout << "Indexed " << invertIndex.size() << " pages containing " << uniqueTerms.size() << " unique terms" << endl << endl;
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


