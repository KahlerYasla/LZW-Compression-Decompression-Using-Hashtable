#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

template <typename T>
void mergeSort(std::vector<T> &arr)
{
    // Base case
    if (arr.size() < 2)
    {
        return;
    }

    // Recursive case
    int mid = arr.size() / 2;
    std::vector<T> left(arr.begin(), arr.begin() + mid);
    std::vector<T> right(arr.begin() + mid, arr.end());
    mergeSort(left);
    mergeSort(right);
    int i = 0, j = 0, k = 0;
    while (i < left.size() && j < right.size())
    {
        if (left[i] < right[j])
        {
            arr[k++] = left[i++];
        }
        else
        {
            arr[k++] = right[j++];
        }
    }
    while (i < left.size())
    {
        arr[k++] = left[i++];
    }
    while (j < right.size())
    {
        arr[k++] = right[j++];
    }
}

std::string bwt(const std::string &s)
{
    int n = s.size();
    // 1. Generate rotations from s
    std::vector<std::string> rotations;
    for (int i = 0; i < n; ++i)
    {
        rotations.push_back(s.substr(i) + s.substr(0, i));
    }
    // 2. Sort the rotations
    mergeSort(rotations);
    // 3. Take the last column
    std::string bwt;
    for (int i = 0; i < n; ++i)
    {
        bwt += rotations[i][n - 1];
    }
    return bwt;
}

std::string reverse_bwt(const std::string &bwt)
{
    // STEP 1: Build the suffix array from the BWT string.
    int n = bwt.size();
    std::vector<std::pair<char, int>> chars;
    for (int i = 0; i < n; ++i)
    {
        chars.push_back(std::make_pair(bwt[i], i));
    }
    mergeSort(chars);

    // STEP 2: Build the string from the suffix array.
    std::string s;
    for (int i = 0, j = chars[0].second; i < n; ++i, j = chars[j].second)
    {
        s += chars[j].first;
    }
    return s;
}

void compress(std::string ifilename, std::string ofilename)
{

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
    std::string outputData = bwt(inputData);

    outputFile << outputData;
    outputFile.close();
}

void decompress(std::string ifilename, std::string ofilename)
{
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

    std::string outputData = reverse_bwt(inputData);

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