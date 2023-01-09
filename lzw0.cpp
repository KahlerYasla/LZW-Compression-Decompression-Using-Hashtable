#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

// Hash function for character sequences
struct KeyHasher
{
    std::size_t operator()(const std::vector<char> &key) const
    {
        std::size_t hash = 0;
        for (char c : key)
        {
            hash = hash * 31 + c;
        }
        return hash;
    }
};

// Dictionary entry for character sequence and its code
struct DictionaryEntry
{
    std::vector<char> sequence;
    int code;
};

// Compresses the input file and writes the output to the output file
void compress(std::ifstream &in, std::ofstream &out)
{
    // Initialize dictionary with ASCII characters
    std::unordered_map<std::vector<char>, int, KeyHasher> dictionary;
    for (int i = 0; i < 256; i++)
    {
        std::vector<char> sequence = {char(i)};
        dictionary[sequence] = i;
    }
    int code = 256;

    // Read input file one character at a time
    char c;
    std::vector<char> sequence;
    while (in.get(c))
    {
        sequence.push_back(c);
        if (!dictionary.count(sequence))
        {
            // Output code for previous sequence and add new entry to dictionary
            out.write((char *)&dictionary[sequence], sizeof(int));
            dictionary[sequence] = code++;
            sequence.pop_back();
        }
    }
    // Output code for last sequence
    out.write((char *)&dictionary[sequence], sizeof(int));
}

// Decompresses the input file and writes the output to the output file
void decompress(std::ifstream &in, std::ofstream &out)
{
    // Initialize dictionary with ASCII characters
    std::unordered_map<int, DictionaryEntry> dictionary;
    for (int i = 0; i < 256; i++)
    {
        std::vector<char> sequence = {char(i)};
        dictionary[i] = {sequence, i};
    }
    int code = 256;

    // Read dictionary size from input file
    int dictSize;
    in.read((char *)&dictSize, sizeof(int));

    // Read input file one code at a time
    int prevCode;
    in.read((char *)&prevCode, sizeof(int));
    std::vector<char> sequence = dictionary[prevCode].sequence;
    out.write(sequence.data(), sequence.size());
    
    while (in.read((char *)&code, sizeof(int)))
    {
        if (dictionary.count(code))
        {
            // Output character sequence and add new entry to dictionary
            std::vector<char> sequence = dictionary[code].sequence;
            out.write(sequence.data(), sequence.size());
            dictionary[code] = {dictionary[prevCode].sequence, code};
            dictionary[code].sequence.push_back(sequence[0]);
        }
        else
        {
            // Output character sequence for previous code and add new entry to dictionary
            std::vector<char> sequence = dictionary[prevCode].sequence;
            sequence.push_back(sequence[0]);
            out.write(sequence.data(), sequence.size());
            dictionary[code] = {sequence, code};
        }
        prevCode = code;
    }
}

int main(int argc, char *argv[])
{
    // Check number of arguments
    if (argc != 4)
    {
        std::cerr << "Usage: lzw c/d input_file output_file" << std::endl;
        return 1;
    }

    // Open input and output files
    std::ifstream in(argv[2], std::ios::binary);
    std::ofstream out(argv[3], std::ios::binary);
    if (!in || !out)
    {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    // Check first argument and call appropriate function
    if (argv[1][0] == 'c')
    {
        compress(in, out);
    }
    else if (argv[1][0] == 'd')
    {
        decompress(in, out);
    }
    else
    {
        std::cerr << "Invalid command" << std::endl;
        return 1;
    }

    return 0;
}
