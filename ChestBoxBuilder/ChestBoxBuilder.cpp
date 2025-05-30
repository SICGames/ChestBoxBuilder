// ChestBoxBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <sstream>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <Windows.h>
#include <sys/stat.h>
#include <stdexcept>
#include "resource.h"

#include "Inc/MiniLogger/MiniLogger.h"

MiniLogger* logger;

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
    int maxlines = 4;
    int currentLine = 0;
    bool found = false;
    for (auto i = array.begin() + index; i != array.end(); i++) 
    {
        if (currentLine < maxlines - 1) {
            auto t = *i;
            if (t.find(target) != std::string::npos) {
                found = true;
                break;
            }
            currentLine++;
        }
        else {
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
    logger = new MiniLogger();
    logger->CreateLogger("ChestBoxBuilder.log");
    logger->Info("Chest Box Builder Started...");
    /*
     Exit Codes Return Values:
     0 = Success.
     -1 = Help Displayed.
     -500 = No Input File Declared
     -501 = No Output File Declared
     -404 = Input File Doesn't Exist
     -405 = Output File Can't Be Written To
     -411 = No Arguments Supplied.
     -911 = Exception Occured.
    */
    char* inputFile = nullptr;
    char* outputFile = nullptr;
    char* lang = nullptr;
    bool bSuccess = false;
    bool bBadBadBad = false;
    const char* errorMessage = nullptr;
    if (argc < 1) {
        DisplayHelp();
        return -411;
    }
    if (cmdOptionExists(argv, argv + argc, "-help")) {
        DisplayUseage();
        return -1;
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
    
    logger->Info("Loaded resource string 'Contains'");

    std::vector<std::string> lineArray;
    std::string containsStr(sz);

    if (inputFile == nullptr) 
    {
        std::cout << "500\tNo input file specified...\t0%" << std::endl;

        logger->Warn("No input file such as a cached clan chest file has been specified.");
        delete logger;

        Sleep(100);
        return -500;
    }

    if (inputFile) {
        std::string file(inputFile);
        bool exists = FileExists(file);
        if (exists == false) 
        {
            std::stringstream ss;
            ss << "404\t" << inputFile << " does not exist.\t0%";
            
            logger->Warn("Clan Chest File doesn't exist and could not be found.");
            delete logger;

            std::cout << ss.str() << std::endl;
            containsStr.clear();
            inputFile = nullptr;
            outputFile = nullptr;
            Sleep(1);
            return -404;
        }

        if (exists) 
        {
            logger->Info("Cache file exists and beginning to be processed.");
            logger->Info("Beginning to read cache file and obtain maximum of lines in file.");

            int maxLines = -1;
            try {
                maxLines = GetLineCount(inputFile);
            }
            catch (std::exception& ex) {
                std::cout << "500\tSomething terrible just happened. ChestBoxBuilder caught an exception with a reason: " << ex.what() << ".\t0%" << std::endl;
                logger->Fatal(&ex, "Issue with obtaining cached clan chest file\'s lines.");
                
                delete logger;
                logger = 0;

                inputFile = nullptr;
                outputFile = nullptr;
                lang = nullptr;
                return -911;
            }

            lineArray.resize(maxLines + 1);

            if (maxLines > 0)
            {
                logger->Info("Maximum lines of cached clan chest file obtained successfully.");

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
                    try {
                        auto lineStr = lineArray.at(lines); //-- check to see if we can do this without exception.
                        lineArray[lines] = line;
                        lines++;
                        logger->Info("Successfully processed a line from cached clan chest file.");
                        Sleep(10);
                    }
                    catch (const std::out_of_range& e) {
                        std::cout << "500\tSomething terrible just happened. ChestBoxBuilder caught an exception. Reason: Index Out Of Range. " << ".\t0%" << std::endl;
                        std::exception ex(e);

                        logger->Fatal(&ex, "There\'s an issue with processing clan chest file. Index out of range usually means there was an attempt to obtain an element outside array's max capacity.");

                        delete logger;

                        bBadBadBad = true;
                        bSuccess = false;
                        Sleep(10);
                        break;
                    }
                }

                iFile.close();
                logger->Info("Closed cached clan chest file.");

                if (bBadBadBad) {
                    if (lineArray.size() > 0) 
                    {
                        maxLines = -1;
                        lineArray.clear();
                        inputFile = nullptr;
                        outputFile = nullptr;
                        return -911;
                    }
                }
                if (!outputFile) {
                    std::cout << "500\tNo output file specified...\t0%" << std::endl;
                    logger->Info("No output file was specified. Which is unusual.");
                    delete logger;

                    Sleep(10);
                    return -501;
                }

                if (outputFile)
                {
                    logger->Info("Writing to temporary output file.");

                    std::ofstream oFile(outputFile, std::ios::out);
                    if (!oFile) 
                    {
                        std::cout << "500\tUnable To Write to Output File '" << outputFile <<"'...\t0 % " << std::endl;
                        containsStr.clear();
                        inputFile = nullptr;
                        outputFile = nullptr;
                        logger->Warn("Was not able to open temporary output file. Could be lack of permissions. Run as administrator and try again.");
                        delete logger;
                        Sleep(10);
                        return -405;
                    }

                    unsigned int inc = 0;
                    unsigned int index = 0;
                    
                    logger->Info("Beginning to populate and build chest boxes to output file.");

                    for (auto i = 0; i < lineArray.size() - 1; i++)
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
                            std::vector<std::string> tmp_array;
                            std::copy(lineArray.begin() + i, lineArray.end(), std::back_inserter(tmp_array));
                            unsigned int endPos = lineArray.size() - i;

                            if (Has(tmp_array, containsStr, endPos)) {
                                inc = 4;
                            }
                            else
                            {
                                inc = 3;
                            }

                            unsigned int linenum = i;
                            for (unsigned int x = 0; x != inc; x++) 
                            {
                                std::string line = lineArray[i + x];
                                oFile << line << "\n";
                            }

                            tmp_array.clear();
                            logger->Info("Successfully built a chest box.");
                        }
                        catch (const std::exception &ex) {
                            bSuccess = false;
                            bBadBadBad = true;
                            errorMessage = ex.what();
                            logger->Fatal((std::exception*)&ex, "Failed building a chest box. More information is provided.");
                            delete logger;
                            break;
                        }

                        i += inc - 1;
                        Sleep(5);
                    }

                    oFile << "#\n";
                    oFile.close();
                    if (bBadBadBad == false) 
                    {
                        bSuccess = true;
                    }
                    logger->Info("Everything went smooth as rain.");

                }
            }
            else {
                logger->Info("There are no lines to read from cache file. Possibly due to being empty.");
                std::cout << "100\tNo lines obtainable. Possibly empty cached clan chest file...\t0%"<< std::endl;

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

    logger->Info("Cleaning up amd shutting down...");
    delete logger;

    if (bSuccess) {
        std::cout << "200\tComplete\t100%" << std::endl;
    }
    else {
        std::cout << "500\tUnexpected Error Occured. Exception was caught for having the following reason: " << errorMessage << "\t0% " << std::endl;
        return -911;
    }
    return 0;
}