#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <pthread.h>
#include <windows.h>
#include <filesystem>
#include <chrono>

#include "threadcontrol.h"

using namespace std;

std::mutex consoleMutex;
std::mutex outputMutex;

std::ofstream outputFile;


int main() 
{
    string data = "data.txt";
    string output = "analize/output.txt";

    if (std::filesystem::exists(output)) 
    {
        std::cout << "File exists. Clearing content..." << std::endl;
        std::ofstream file(output, std::ios::trunc);
    }

    ifstream inputFile(data);
    if(!inputFile.is_open())
    {
        return 1;
    }

    vector<string> lines;
    string line;

    while(getline(inputFile, line))
    {
        lines.push_back(line);
    }
    inputFile.close();

    

    outputFile.open(output);
    if(!outputFile.is_open())
    {
        return 1;
    }
    string wordToFind;
    wordToFind.resize(20);
    int threadCount;

    std::cout << "threads: ";
    std::cin >> threadCount;
    std::cout << "word to find: ";
    std::cin >> wordToFind;

    int linesPerThread = lines.size() / threadCount;
    int remainingLines = lines.size() % threadCount;
    HANDLE * threads = new HANDLE[threadCount];
    ThreadData * tData = new ThreadData[threadCount];
    int currentLine = 0;

    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i < threadCount; ++i)
    {
        tData[i].startLine = currentLine;
        tData[i].endLine = currentLine + linesPerThread - 1;

        
        if (remainingLines > 0) 
        {
            tData[i].endLine++;
            remainingLines--;
        }

        tData[i].threadId = i + 1;
        currentLine = tData[i].endLine+1;
        tData[i].word = wordToFind;
        tData[i].lines = lines;
        tData[i].outMtx = &outputMutex;
        tData[i].output = &outputFile;

        threads[i] = CreateThread(NULL, 0, SearchWordInLines, &tData[i], CREATE_SUSPENDED, NULL);

    }
    auto control = chrono::high_resolution_clock::now();
    thread_control(threads, threadCount, consoleMutex);
    auto control_end = chrono::high_resolution_clock::now();

    WaitForMultipleObjects(threadCount, threads, TRUE, INFINITE);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start - (control_end - control);

    for(int i = 0; i < threadCount; ++i)
    {
        CloseHandle(threads[i]);
    }

    std::cout << "time spent: " << duration.count() << " miliseconds" << std::endl;

    delete[] threads;
    delete[] tData;
    outputFile.close();

    system("pause");



}