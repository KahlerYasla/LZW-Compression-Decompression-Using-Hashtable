#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>

const int DICT_SIZE = 65536; // size of hash table

void compress(std::ifstream &in_file, std::ofstream &out_file)
{
    // initialize hash table with single characters
    std::unordered_map<std::string, int> dict;
    for (int i = 0; i < 256; i++)
    {
        dict[std::string(1, i)] = i;
    }
    int next_code = 256; // next code to be added to dictionary

    std::string s; // current string
    char c;
    while (in_file.get(c))
    {
        std::string sc = s + c; // new string formed by appending c to s
        if (dict.count(sc))
        { // sc is in the dictionary
            s = sc;
        }
        else
        {                        // sc is not in the dictionary
            out_file << dict[s]; // output code for s
            // add sc to the dictionary
            dict[sc] = next_code++;
            s = c;
        }
    }
    // output code for remaining string
    if (!s.empty())
    {
        out_file << dict[s];
    }
}

void decompress(std::ifstream &in_file, std::ofstream &out_file)
{
    // initialize hash table with single characters
    std::unordered_map<int, std::string> dict;
    for (int i = 0; i < 256; i++)
    {
        dict[i] = std::string(1, i);
    }
    int next_code = 256; // next code to be added to dictionary

    int prev_code = in_file.get(); // previous code
    if (prev_code == EOF)
    { // empty file
        return;
    }
    out_file << dict[prev_code]; // output first character

    int curr_code;

    while (in_file.get(curr_code))
        ;
    {
        if (dict.count(curr_code))
        { // curr_code is in the dictionary
            std::string s = dict[curr_code];
            out_file << s;
            // add new string to the dictionary
            dict[next_code++] = dict[prev_code] + s[0];
        }
        else
        { // curr_code is not in the dictionary
            std::string s = dict[prev_code];
            s += s[0];
            out_file << s;
            // add new string to the dictionary
            dict[next_code++] = s;
        }
        prev_code = curr_code;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: lzw c/d input_file output_file" << std::endl;
        return 1;
    }

    char mode = argv[1][0];
    std::ifstream in_file(argv[2], std::ios::binary);
    if (!in_file)
    {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }
    std::ofstream out_file(argv[3], std::ios::binary);
    if (!out_file)
    {
        std::cerr << "Error opening output file" << std::endl;
        return 1;
    }

    // compress or decompress
    if (mode == 'c')
    {
        compress(in_file, out_file);
    }
    else if (mode == 'd')
    {
        decompress(in_file, out_file);
    }
    else
    {
        std::cerr << "Invalid mode" << std::endl;
        return 1;
    }

    return 0;
}