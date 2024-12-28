// ChestBoxBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <Windows.h>
#include <sys/stat.h>

#include "resource.h"

void DisplayUseage() {
    std::cout << "Program Usage: ChestBoxBuilder.exe -i [input file] -o [out file] -L [language display name] en-US" << std::endl;
}

bool FileExists(const std::string &filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}
char* getCmdOption(char** begin, char** end, const std::string& option)
{
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

unsigned int GetLineCount(char* filename) {
    unsigned int lines = 0;
    FILE* inFile = nullptr;
    fopen_s(&inFile, filename, "r");
    char c;
    if (inFile == nullptr) {
        return 0;
    }

    while (EOF != (c = getc(inFile))) {
        if ('\n' == c)
            lines++;
    }
    fclose(inFile);
    return lines;
}

bool Has(std::vector<std::string> array, std::string target, unsigned int index) {

    bool found = false;
    for (auto i = array.begin() + index; i != array.end(); ++i) {
        auto t = *i;
        if (t.find(target) != std::string::npos) {
            found = true;
            break;
        }
    }
    return found;
}

void DisplayHelp() {
    std::cout << "Use -help for understanding program usage." << std::endl;
}
int main(int argc, char *argv[])
{
    char* inputFile = nullptr;
    char* outputFile = nullptr;
    char* lang = nullptr;
    bool bSuccess = false;
    bool bBadBadBad = false;

    if (argc < 1) {
        DisplayHelp();
        return 0;
    }
    if (cmdOptionExists(argv, argv + argc, "-help")) {
        DisplayUseage();
    }
    inputFile = getCmdOption(argv, argv + argc, "-i");
    outputFile = getCmdOption(argv, argv + argc, "-o");
    lang = getCmdOption(argv, argv + argc, "-L");

    /* localization 
       - load specified .DLL 
       - load string from loaded .DLL
    */

    //-- we'll just default to en-US
    char sz[255] = { 0 };
    LoadStringA(GetModuleHandle(0),IDS_CONTAINS, sz, sizeof(sz)/sizeof(char));
    
    std::vector<std::string> lineArray;
    std::string containsStr(sz);

    if (inputFile == nullptr) 
    {
        std::cout << "500\tNo input file specified...\t0%" << std::endl;
        return EXIT_FAILURE;
    }

    if (inputFile) {
        std::string file(inputFile);
        bool exists = FileExists(file);
        if (exists == false) 
        {
            char msg[2048] = { 0 };

            sprintf_s(msg,sizeof(msg)/sizeof(char),"404\t %s does not exist.\t0%", inputFile);

            std::cout << msg << std::endl;
            containsStr.clear();
            inputFile = nullptr;
            outputFile = nullptr;
            return EXIT_FAILURE;
        }

        if (exists) 
        {
            int maxLines = -1;
            try {
                maxLines = GetLineCount(inputFile);
            }
            catch (std::exception ex) {

            }

            lineArray.resize(maxLines);

            if (maxLines > 0)
            {
                unsigned int lines = 0;
                std::string line;
                std::ifstream iFile(inputFile);
                double percentage;
                while (std::getline(iFile, line))
                {
                    double l = lines * 1.0;
                    double ml = maxLines * 1.0;
                    percentage = ((double)(l / ml) * 100.0);
                    std::cout << "100\tProcessing Cached Clan Chest Data (" << lines << "/" << maxLines << ")\t" << round(percentage) << "%" << std::endl;
                    lineArray[lines] = line;
                    lines++;
                    Sleep(1);
                }
                iFile.close();

                if (outputFile)
                {
                    std::ofstream oFile(outputFile, std::ios::out);
                    if (!oFile) 
                    {
                        std::cout << "500\tUnable To Write to Output File '" << outputFile <<"'...\t0 % " << std::endl;
                        containsStr.clear();
                        inputFile = nullptr;
                        outputFile = nullptr;
                        return EXIT_FAILURE;
                    }

                    unsigned int inc = 0;
                    unsigned int index = 0;
                    for (auto i = 0; i < lineArray.size(); ++i)
                    {

                        double _i = i * 1.0;
                        double _lSz = lineArray.size() * 1.0;
                        double percentage = ((double)_i / _lSz * 100.0);
                        std::cout << "100\tPopulating Chest Boxes...\t" << round(percentage) << "%" << std::endl;
                        oFile << "#\n";

                        //-- 0 Gnorme Workshop Chest
                        //-- 1 From: AwesomeDude
                        //-- 2 Source: Level 5 Crypt
                        //-- 3 Contains: Gold

                        try
                        {
                            std::vector<std::string> tmp_array(lineArray);
                            if (Has(tmp_array, containsStr, i)) {
                                inc = 4;
                            }
                            else
                            {
                                inc = 3;
                            }

                            for (unsigned int x = 0; x != inc; x++) {
                                oFile << lineArray[i + x] << "\n";
                            }
                            tmp_array.clear();
                        }
                        catch (std::exception ex) {
                            bSuccess = false;
                            bBadBadBad = true;
                            break;
                        }

                        i += inc - 1;
                        Sleep(1);
                    }

                    oFile << "#\n";
                    oFile.close();
                    if (bBadBadBad == false) 
                    {
                        bSuccess = true;
                    }
                }
            }
        }
    }

    //-- Clean Up On Isle 5
    containsStr.clear();

    if (lineArray.size() > 0) {
        lineArray.clear();
    }

    ZeroMemory(&sz, sizeof(sz) / sizeof(char));

    outputFile = nullptr;
    inputFile = nullptr;
    if (bSuccess) {
        std::cout << "200\tComplete\t100%" << std::endl;
    }
    else {
        std::cout << "500\tUnexpected Error Occured.\t0%" << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}