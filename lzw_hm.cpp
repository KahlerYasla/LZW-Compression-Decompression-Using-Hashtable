#include <iostream>
#include <fstream>
#include <string>

#define MAX_DEF 65536

class hashtable
{
private:
    std::string *table;
    uint64_t length;
    uint64_t size;

public:
    hashtable(uint64_t size)
    {
        this->size = size;
        length = 0;
        table = new std::string[size];

        for (uint64_t i = 0; i < size; i++)
        {
            table[i] = "";
        }
    }

    ~hashtable()
    {
        delete[] table;
    }

    uint64_t hash(std::string Word)
    {
        uint64_t hash = 0;
        for (char &c : Word)
        {
            hash = (hash * 7 + uint64_t(c)) % size;
        }
        return hash;
    }

    void insert(std::string Word)
    {
        if (float(length) / float(size) > 0.8)
        {
            rehash();
        }

        uint64_t index = hash(Word) % size;

        while (table[index] != "")
        {
            if (table[index] == Word)
            {
                return;
            }
            index = (index + 1) % size;
        }
        table[index] = Word;
        length++;
    }

    std::string get(uint64_t index)
    {
        return table[index];
    }

    uint64_t getSize()
    {
        return size;
    }

    uint64_t find(std::string Word)
    {
        uint64_t index = hash(Word) % size;

        while (table[index] != "")
        {
            if (table[index] == Word)
            {
                return index;
            }
            index = (index + 1) % size;
        }
        return -1;
    }

    void remove(std::string Word)
    {
        uint64_t index = find(Word);
        table[index] = "";
        length--;
    }

    void rehash()
    {
        hashtable newTable(size * 2);

        for (uint64_t i = 0; i < size; i++)
        {
            if (table[i] != "")
            {
                newTable.insert(table[i]);
            }
        }

        delete[] table;
        table = newTable.table;
        size = size * 2;
    }
};

void compress(std::string ifilename, std::string ofilename)
{
    // Dictionary initializing with ASCII
    hashtable dictionary(MAX_DEF);
    for (uint64_t i = 0; i < 256; i++)
    {
        dictionary.insert(std::string(1, char(i)));
    }

    // Read input file into a string
    std::ifstream inputFile(ifilename);

    // Check if file exists
    if (!inputFile)
    {
        std::cout << "File does not exist" << std::endl;
        return;
    }

    std::string inputData((std::istreambuf_iterator<char>(inputFile)),
                          std::istreambuf_iterator<char>());
    inputFile.close();

    // Output file for compressed data
    std::ofstream outputFile(ofilename);

    // Check if file is empty
    if (inputData.empty())
    {
        outputFile.close();
        return;
    }

    std::string outputData = "";

    // Write the compressed data to the output file
    std::string P = inputData.substr(0, 1);
    std::string C = "";
    for (std::size_t i = 1; i < inputData.length(); i++)
    {
        C = inputData.substr(i, 1);
        if (dictionary.find(P + C) != -1)
        {
            P = P + C;
        }
        else
        {
            uint64_t index = dictionary.find(P);
            outputData += std::to_string(index) + " ";
            dictionary.insert(P + C);
            P = C;
        }
    }
    // Write the last entry
    uint64_t index = dictionary.find(P);
    outputData += std::to_string(index) + " ";

    outputFile << outputData;
    outputFile.close();
}

void decompress(std::string ifilename, std::string ofilename)
{
    // Dictionary initializing with ASCII
    hashtable dictionary(MAX_DEF);
    for (uint64_t i = 0; i < 256; i++)
    {
        dictionary.insert(std::string(1, char(i)));
    }

    // Read input file into a string
    std::ifstream inputFile(ifilename);

    // Check if file exists
    if (!inputFile)
    {
        std::cout << "File does not exist" << std::endl;
        return;
    }

    std::string inputData((std::istreambuf_iterator<char>(inputFile)),
                          std::istreambuf_iterator<char>());
    inputFile.close();

    // Output file for compressed data
    std::ofstream outputFile(ofilename);

    // Check if file is empty
    if (inputData.empty())
    {
        outputFile.close();
        return;
    }

    std::string outputData = "";

    // Parse the input string and write the decompressed data to the output file

    uint64_t old_cursor = 0;
    uint64_t cursor = inputData.find(" ");
    uint64_t indexLength = cursor - old_cursor;
    uint64_t index = std::stoi(inputData.substr(old_cursor, indexLength));
    std::string P = dictionary.get(index);
    outputData += P;
    std::string C = "";
    while (cursor != std::string::npos)
    {
        old_cursor = cursor + 1;
        cursor = inputData.find(" ", old_cursor);
        if (cursor != std::string::npos)
        {
            indexLength = cursor - old_cursor;
            if (indexLength > 0)
            {
                index = std::stoi(inputData.substr(old_cursor, indexLength));
                C = dictionary.get(index);
                if (index < dictionary.getSize())
                {
                    outputData += C;
                    dictionary.insert(P + C.substr(0, 1));
                }
                else
                {
                    C = P + P.substr(0, 1);
                    outputData += C;
                    dictionary.insert(C);
                }
                P = C;
            }
        }
    }

    outputFile << outputData;
    outputFile.close();
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Invalid number of arguments" << std::endl;
        return -1;
    }
    std::string operation = argv[1];
    std::string ifilename = argv[2];
    std::string ofilename = argv[3];

    if (operation == "c" || operation == "C")
    {
        compress(ifilename, ofilename);
    }
    else if (operation == "d" || operation == "D")
    {
        decompress(ifilename, ofilename);
    }
    else
    {
        std::cout << "Invalid operation" << std::endl;
    }

    return 0;
}