/**
 * @file spell_check.cc
 * @author Moududur Rahman
 * @brief A simple spell checker that reads a document file, a dictionary file, 
 *        and displays if any words in the document are incorrect 
 * @version 0.1
 * @date 2021-11-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <fstream>
#include <iostream>
#include <string>
#include <locale>

// You can change to quadratic probing if you
// haven't implemented double hashing.
#include "double_hashing.h"
using namespace std;

// You can add more functions here.

// Creates and fills double hashing hash table with all words from
// dictionary_file
HashTableDouble<string> MakeDictionary(const string &dictionary_file)
{
  string line;
  fstream d_file(dictionary_file);
  HashTableDouble<string> dictionary_hash(73, 101);

  while (d_file >> line)
  {
    dictionary_hash.Insert(line);
  }

  // Fill dictionary_hash.
  return dictionary_hash;
}

struct AltSpelling
{
  std::string spelling;
  char type;
};

// For each word in the document_file, it checks the 3 cases for a word being
// misspelled and prints out possible corrections
void SpellChecker(HashTableDouble<string> &dictionary,
                  const string &document_file)
{
  std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
  std::string line;
  std::fstream doc_file(document_file);
  std::locale loc;
  std::vector<AltSpelling> alternate_spellings;
  while (doc_file >> line)
  {
    for (size_t x = 0; x < line.size(); x++)
    {
      line.replace(x, 1, std::string(1, std::tolower(line[x], loc)));
      if (line[x] == '.' || line[x] == ',')
      {
        line.erase(x, x + 1);
      }
    }

    try
    {
      dictionary.Get(line);
      std::cout << line << " is CORRECT" << std::endl;
    }
    catch (const std::exception &e)
    {
      std::cout << line << " is INCORRECT" << std::endl;
      string copy = line;

      for (size_t x = 0; x < 26; x++)
      {
        for (size_t y = 0; y < line.size() + 1; y++)
        {
          copy.insert(y, std::string(1, alphabet[x]));
          alternate_spellings.push_back(AltSpelling{copy, 'A'});
          copy = line;
        }
      }

      for (size_t x = 0; x < line.size(); x++)
      {
        copy.erase(x, 1);
        alternate_spellings.push_back(AltSpelling{copy, 'B'});
        copy = line;
      }

      for (size_t x = 0; x < line.size() - 1; x++)
      {
        std::swap(copy[x], copy[x + 1]);
        alternate_spellings.push_back(AltSpelling{copy, 'C'});
        copy = line;
      }

      for (auto x : alternate_spellings)
      {
        try
        {
          dictionary.Get(x.spelling);
          std::cout << "*** " << line << " -> " << x.spelling << " *** case " << x.type << std::endl;
        }
        catch (const std::exception &e)
        {
        }
      }

      alternate_spellings.clear();
    }
  }
}

// @argument_count: same as argc in main
// @argument_list: save as argv in main.
// Implements
int testSpellingWrapper(int argument_count, char **argument_list)
{
  const string document_filename(argument_list[1]);
  const string dictionary_filename(argument_list[2]);

  // Call functions implementing the assignment requirements.
  HashTableDouble<string> dictionary = MakeDictionary(dictionary_filename);
  SpellChecker(dictionary, document_filename);

  return 0;
}

// Sample main for program spell_check.
// WE WILL NOT USE YOUR MAIN IN TESTING. DO NOT CODE FUNCTIONALITY INTO THE
// MAIN. WE WILL DIRECTLY CALL testSpellingWrapper. ALL FUNCTIONALITY SHOULD BE
// THERE. This main is only here for your own testing purposes.
int main(int argc, char **argv)
{
  if (argc != 3)
  {
    cout << "Usage: " << argv[0] << " <document-file> <dictionary-file>"
         << endl;
    return 0;
  }

  testSpellingWrapper(argc, argv);

  return 0;
}
