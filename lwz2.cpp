#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

int main(int argc, char **argv)
{
    int next_code = 256;
    // Check if there are enough command line arguments
    if (argc < 4)
    {
        std::cout << "Error: not enough command line arguments" << std::endl;
        return 1;
    }

    // Check if first argument is 'c' or 'd' for compression or decompression
    std::string operation = argv[1];
    if (operation != "c" && operation != "d")
    {
        std::cout << "Error: invalid operation specified. Use 'c' for compression or 'd' for decompression." << std::endl;
        return 1;
    }

    // Read input file
    std::ifstream input_file(argv[2]);
    if (!input_file.is_open())
    {
        std::cout << "Error: unable to open input file" << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream output_file(argv[3]);
    if (!output_file.is_open())
    {
        std::cout << "Error: unable to open output file" << std::endl;
        return 1;
    }

    // Initialize dictionary with 256 ASCII characters
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < 256; i++)
    {
        dictionary[std::string(1, (char)i)] = i;
    }

    // Compress or decompress text file based on specified operation
    if (operation == "c")
    {
        // Compress text file
        std::string current_string = "";
        char c;
        while (input_file.get(c))
        {
            std::string s = current_string + c;
            if (dictionary.count(s))
            {
                current_string = s;
            }
            else
            {
                output_file << dictionary[current_string] << " ";
                dictionary[s] = next_code++;
                current_string = c;
            }
        }
        output_file << dictionary[current_string];
    }
    else
    {
        // Decompress text file
        std::unordered_map<int, std::string> reverse_dictionary;
        for (auto it : dictionary)
        {
            reverse_dictionary[it.second] = it.first;
        }

        int current_code;
        input_file >> current_code;
        std::string current_string = reverse_dictionary[current_code];
        output_file << current_string;
        int previous_code = current_code;
        while (input_file >> current_code)
        {
            std::string s;
            if (reverse_dictionary.count(current_code))
            {
                s = reverse_dictionary[current_code];
            }
            else if (current_code == next_code)
            {
                s = reverse_dictionary[previous_code] + reverse_dictionary[previous_code][0];
            }
            else
            {
                std::cout << "Error: invalid code found in compressed file" << std::endl;
                //return 1;
            }
            output_file << s;
            dictionary[reverse_dictionary[previous_code] + s[0]] = next_code++;
            previous_code = current_code;
        }
    }

    input_file.close();
    output_file.close();

    return 0;
}