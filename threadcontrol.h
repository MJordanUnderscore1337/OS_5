#ifndef THREAD_CONTROL 
#define THREAD_CONTROL

#include <windows.h>
#include <vector>
#include <map>
#include <mutex>
#include <string>

class ThreadData 
{
public:
    int startLine;
    int endLine;
    int threadId;
    std::string word; 
    std::vector<std::string> lines;
    std::mutex *outMtx;
    std::ofstream *output;
    ThreadData();
    ~ThreadData();
};

DWORD WINAPI SearchWordInLines(LPVOID param);
void thread_control(const HANDLE *threads, const int size, std::mutex &mtx);


#endif