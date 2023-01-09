#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

const int HASH_SIZE = 65536; // size of hash table

// Hash table class to store mappings between character sequences and codes
class HashTable
{
public:
    // Entry in the hash table
    struct Entry
    {
        string str; // character sequence
        int code;   // code for the sequence
    };

    Entry table[HASH_SIZE]; // the hash table

public:
    // Constructor
    HashTable()
    {
        // Initially, each entry in the table is empty
        for (int i = 0; i < HASH_SIZE; i++)
        {
            table[i].str = "";
            table[i].code = -1;
        }
    }

    // Insert a new entry into the hash table
    void insert(string str, int code)
    {
        // Calculate the hash value for the string
        int hash = 0;
        for (int i = 0; i < str.length(); i++)
        {
            hash = (hash + str[i]) % HASH_SIZE;
        }

        // Insert the new entry into the hash table
        table[hash].str = str;
        table[hash].code = code;
    }

    // Find an entry in the hash table
    int find(string str)
    {
        // Calculate the hash value for the string
        int hash = 0;
        for (int i = 0; i < str.length(); i++)
        {
            hash = (hash + str[i]) % HASH_SIZE;
        }

        // Return the code for the entry
        return table[hash].code;
    }
};
//-------------------------------------------------------------------------------------------------------
void compress(string input_file, string output_file)
{
    // Open the input file
    ifstream fin(input_file, ios::binary);
    if (!fin)
    {
        cout << "Error: Cannot open input file" << endl;
        return;
    }

    // Open the output file
    ofstream fout(output_file, ios::binary);
    if (!fout)
    {
        cout << "Error: Cannot open output file" << endl;
        return;
    }

    // Initialize the hash table
    HashTable table;
    for (int i = 0; i < 256; i++)
    {
        string str;
        str += (char)i;
        table.insert(str, i);
    }

    // Initialize the current character and string
    char c = fin.get();
    string str = "";
    str += c;

    // Initialize the code
    int code = 256;

    // Compress the file
    while (!fin.eof())
    {
        // Read the next character
        char next = fin.get();

        // Check if the string + next character is in the table
        if (table.find(str + next) != -1)
        {
            // The string is in the table, so add the next character to the string
            str += next;
        }
        else
        {
            // The string is not in the table, so output the code for the string
            fout << table.find(str) << " ";

            // Add the new string (str + next character) to the table
            table.insert(str + next, code);
            code++;

            // Reset the current string to the next character
            str = "";
            str += next;
        }
    }

    // Output the code for the last string
    fout << table.find(str) << " ";

    // Close the files
    fin.close();
    fout.close();
}
//-------------------------------------------------------------------------------------------------------
void decompress(string input_file, string output_file)
{
    // Open the input file
    ifstream fin(input_file, ios::binary);
    if (!fin)
    {
        cout << "Error: Cannot open input file" << endl;
        return;
    }

    // Open the output file
    ofstream fout(output_file, ios::binary);
    if (!fout)
    {
        cout << "Error: Cannot open output file" << endl;
        return;
    }

    // Initialize the hash table
    HashTable table;
    for (int i = 0; i < 256; i++)
    {
        string str;
        str += (char)i;
        table.insert(str, i);
    }

    // Initialize the old code and current code
    int old_code = 0;
    int current_code = 0;

    // Read the first code
    fin >> current_code;

    // Initialize the current string to the character corresponding to the code
    string current_str = "";
    current_str += (char)current_code;

    // Initialize the code
    int code = 256;

    // Decompress the file
    while (!fin.eof())
    {
        // Output the current string
        fout << current_str;

        // Read the next code
        fin >> old_code;

        // Check if the old code is in the table
        if (table.find(string(1, (char)old_code)) == -1)
        {
            // The old code is not in the table, so add a new entry to the table
            string str = table.table[current_code].str;
            str += current_str[0];
            table.insert(str, code);
            code++;
        }
        else
        {
            // The old code is in the table, so get the string for the old code
            string str = table.table[old_code].str;

            // Add a new entry to the table
            str = table.table[current_code].str;
            str += str[0];
            table.insert(str, code);
            code++;
        }

        // Set the current code and current string
        current_code = old_code;
        current_str = table.table[current_code].str;
    }

    // Close the files
    fin.close();
    fout.close();
}
//-------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Check if the correct number of command line arguments is provided
    if (argc != 4)
    {
        cout << "Usage: lzw c/d input_file output_file" << endl;
        return 1;
    }

    // Get the command line arguments
    string mode = argv[1];
    string input_file = argv[2];
    string output_file = argv[3];

    // Check if the mode is "c" for compression or "d" for decompression
    if (mode == "c")
    {
        // Compress the input file
        compress(input_file, output_file);
    }
    else if (mode == "d")
    {
        // Decompress the input file
        decompress(input_file, output_file);
    }
    else
    {
        // Invalid mode
        cout << "Error: Invalid mode" << endl;
        return 1;
    }

    return 0;
}
