#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <sstream>

using namespace std;

const int HASH_SIZE = 65536;

// Hash table class to store mapping between character sequences and codes
class HashTable
{
private:
    // Inner class for hash table entries
    class Entry
    {
    public:
        string str;  // character sequence
        int code;    // code for character sequence
        Entry *next; // next entry in hash table

        // Constructor
        Entry(string str, int code) : str(str), code(code), next(nullptr) {}
    };

    Entry **entries; // array of hash table entries

public:
    // Constructor
    HashTable()
    {
        entries = new Entry *[HASH_SIZE];
        for (int i = 0; i < HASH_SIZE; i++)
        {
            entries[i] = nullptr;
        }
    }

    // Destructor
    ~HashTable()
    {
        for (int i = 0; i < HASH_SIZE; i++)
        {
            Entry *entry = entries[i];
            while (entry != nullptr)
            {
                Entry *next = entry->next;
                delete entry;
                entry = next;
            }
        }
        delete[] entries;
    }

    // Find code for given character sequence, return -1 if not found
    int find(string str)
    {
        // Compute hash value for character sequence
        int hash = 0;
        for (char c : str)
        {
            hash = (hash * 31 + c) % HASH_SIZE;
        }

        // Search for character sequence in hash table
        Entry *entry = entries[hash];
        while (entry != nullptr)
        {
            if (entry->str == str)
            {
                return entry->code;
            }
            entry = entry->next;
        }
        return -1;
    }

    // Add character sequence and code to hash table
    void add(string str, int code)
    {
        // Compute hash value for character sequence
        int hash = 0;
        for (char c : str)
        {
            hash = (hash * 31 + c) % HASH_SIZE;
        }

        // Add character sequence and code to hash table
        Entry *entry = new Entry(str, code);
        entry->next = entries[hash];
        entries[hash] = entry;
    }
};

// Compress text file using LZW method
void compress(string input_file, string output_file)
{
    // Open input and output files
    ifstream fin(input_file, ios::binary);
    if (!fin.is_open())
    {
        cerr << "Error: Cannot open input file" << endl;
        return;
    }
    ofstream fout(output_file, ios::binary);
    if (!fout.is_open())
    {
        cerr << "Error: Cannot open output file" << endl;
        fin.close();
        return;
    }

    // Initialize hash table with single characters
    HashTable table;
    for (int i = 0; i < 256; i++)
    {
        string str(1, (char)i);
        table.add(str, i);
    }

    // Compress file
    string str;
    char c;
    while (fin.get(c))
    {
        str += c;
        int code = table.find(str);
        if (code != -1)
        {
            // character sequence found in hash table
            continue;
        }
        else
        {
            // character sequence not found, output code for previous sequence
            fout.write((char *)&code, 2);
            str.erase(str.size() - 1);
            table.add(str + c, table.find(str) + 256);
            str = c;
        }
    }

    // Output code for last character sequence
    int code = table.find(str);
    fout.write((char *)&code, 2);

    // Close input and output files
    fin.close();
    fout.close();
}

// Decompress file
void decompress(string input_file, string output_file)
{
    // Open input and output files
    ifstream fin(input_file, ios::binary);
    if (!fin.is_open())
    {
        cerr << "Error: Cannot open input file" << endl;
        return;
    }
    ofstream fout(output_file, ios::binary);
    if (!fout.is_open())
    {
        cerr << "Error: Cannot open output file" << endl;
        fin.close();
        return;
    }

    // Initialize hash table with single characters
    HashTable table;
    for (int i = 0; i < 256; i++)
    {
        string str(1, (char)i);
        table.add(str, i);
    }

    // Decompress file
    string str;
    int code;
    fin.read((char *)&code, 2);
    string str2 = to_string(table.find(std::to_string(code)));
    fout << str2;
    while (fin.read((char *)&code, 2))
    {
        str = to_string(table.find(std::to_string(code)));
        if (str == "")
        {
            str = str2 + str2[0];
        }
        fout << str;
        table.add(str2 + str[0], table.find(str2) + 256);
        str2 = str;
    }

    // Close input and output files
    fin.close();
    fout.close();
}


int main(int argc, char **argv)
{
    if (argc < 4)
    {
        cerr << "Error: Invalid number of arguments" << endl;
        cerr << "Usage: lzw c/d input_file output_file" << endl;
        return 1;
    }

    string mode = argv[1];
    string input_file = argv[2];
    string output_file = argv[3];

    if (mode == "c")
    {
        compress(input_file, output_file);
    }
    else if (mode == "d")
    {
        decompress(input_file, output_file);
    }
    else
    {
        cerr << "Error: Invalid mode" << endl;
        cerr << "Usage: lzw c/d input_file output_file" << endl;
        return 1;
    }

    return 0;
}