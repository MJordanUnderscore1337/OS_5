#include "threadcontrol.h"
#include <windows.h>
#include <mutex>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <pthread.h>

#define ERROR_MARGIN 50


ThreadData::ThreadData(){}
ThreadData::~ThreadData(){}



void gotoxy(int x, int y) 
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void gotoxy(COORD coord) 
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearArea(int x, int y, int width, int height) 
{

    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    
    for (int i = 0; i < height; ++i) 
    {
        for (int j = 0; j < width; ++j) 
        {
            std::cout << ' '; 
        }
        std::cout << '\n'; 
    }
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

COORD GetCursorPosition() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.dwCursorPosition;
}


void thread_control(const HANDLE *threads, const int size, std::mutex &mtx)
{
    COORD prev_pos;
    int key_code = 10;
    int n = 0;
    int error_count = 0;
    int priorityClass[6] = {THREAD_PRIORITY_LOWEST, 
                              THREAD_PRIORITY_BELOW_NORMAL, 
                              THREAD_PRIORITY_NORMAL, 
                              THREAD_PRIORITY_ABOVE_NORMAL, 
                              THREAD_PRIORITY_HIGHEST, 
                              THREAD_PRIORITY_TIME_CRITICAL};
 
    while(key_code)
    {
        mtx.lock();
        clearArea(0, 0, ERROR_MARGIN-1, 22);
        gotoxy(0, 0);
        
std::cout << R"(Options:
1-> kill process
2-> Renice process
3-> Suspend process
4-> Resume process
5-> Resume all
0-> exit
)";     
mtx.unlock();
        std::cin >> key_code;

        switch(key_code)
        {
            //kill
            case(1): 
                mtx.lock();
                std::cout << std::endl << "Enter thread ID from 1 to " << size << ": " <<  std::endl;
                mtx.unlock();
                std::cin >> n; 
                
                // Завершуємо thread
                if (!TerminateThread(threads[n-1], 1)) 
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not to terminate the thread" << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                }else 
                {
                    mtx.lock();
                    clearArea(0, 25+n, ERROR_MARGIN-1, 1);
                    gotoxy(0, 25+n);
                    std::cout << "thread with id " << n << " was killed" << std::endl;
                    mtx.unlock();
                }
            break;

            //nice-renice
            case(2): 
                mtx.lock();
                std::cout << "choose the thread from 1 to " << size << ": "; 
                mtx.unlock();
                std::cin >> n;
                mtx.lock();
                std::cout << std::endl << R"(choose priority to set up)" << std::endl;
                std::cout << 
R"(options:
[1] - THREAD_PRIORITY_LOWEST
[2] - THREAD_PRIORITY_BELOW_NORMAL
[3] - THREAD_PRIORITY_NORMAL
[4] - THREAD_PRIORITY_ABOVE_NORMAL
[5] - THREAD_PRIORITY_HIGHEST
[6] - THREAD_PRIORITY_TIME_CRITICAL       ya nenavydju windows X_X
)"              << std::endl;
                mtx.unlock();
                
                std::cin >> key_code; 

                // Встановлюємо пріоритет
                if (!SetThreadPriority(threads[n-1], priorityClass[key_code-1])) 
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not set priority " << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                } else 
                {
                    mtx.lock();
                    clearArea(0, 25+n, ERROR_MARGIN-1, 1);
                    gotoxy(0, 25+n);
                    std::cout << "thread with ID " << n << " priority was changed to " << GetThreadPriority(threads[n-1]) << std::endl;
                    mtx.unlock();
                }
                
            break;

            //suspend
            case (3): 
                mtx.lock();
                std::cout << "choose the process from 1 to " << size << " to suspend: "; 
                mtx.unlock();
                std::cin >> n;
                

                if(SuspendThread(threads[n-1]))
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cout << "error: can not suspend thread " << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                } else
                {
                    mtx.lock();
                    clearArea(0, 25+n, ERROR_MARGIN-1, 1);
                    gotoxy(0, 25+n);
                    std::cout << "thread with ID " << n << " suspended" << std::endl;
                    mtx.unlock();
                }
            break;

            //release
            case(4):
                mtx.lock();
                std::cout << "choose the process from 1 to " << size << " to resume: "; 
                mtx.unlock();
                std::cin >> n;
                
               
                if(!ResumeThread(threads[n-1]))
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not resume thread  " << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                } else
                {
                    mtx.lock();
                    clearArea(0, 25+n, ERROR_MARGIN-1, 1);
                    gotoxy(0, 25+n);
                    std::cout << "thread with ID " << n << " resumed " << std::endl;
                    mtx.unlock();
                } 
            break;
            case(5):
                for(int i = 0; i < size; ++i)
                {
                    ResumeThread(threads[i]);
                }
            break;
            case(0):
                clearArea(0, 0, ERROR_MARGIN-1, 22);
                gotoxy(0, 0);
                std::cout << R"(
                      :::!~!!!!!:.
                  .xUHWH!! !!?M88WHX:.
                .X*#M@$!!  !X!M$$$$$$WWx:.
               :!!!!!!?H! :!$!$$$$$$$$$$8X:
              !!~  ~:~!! :~!$!#$$$$$$$$$$8X:
             :!~::!H!<   ~.U$X!?R$$$$$$$$MM!
             ~!~!!!!~~ .:XW$$$U!!?$M8L8TXMM!
               !:~~~ .:!M"T#$$$$WX??#MRRMMM!
               ~?WuxiW*`   `"#$$$$8!!!!??!!!
             :X- M$$$$       `"T#$T~!8$WUXU~
            :%`  ~#$$$m:        ~!~ ?$$$$$$
          :!`.-   ~T$$$$8xx.  .xWW- ~""##*"
.....   -~~:<` !    ~?T#$$@@W@*?$$      /`
W$@@M!!! .!~~ !!     .:XUW$W!~ `"~:    :
#"~~`.:x%`!!  !H:   !WM$$$$Ti.: .!WUn+!`
:::~:!!`:X~ .: ?H.!u "$$$B$$$!W:U!T$$M~
.~~   :X@!.-~   ?@WTWo("*$$$W$TH$! `
Wi.~!X$?!-~    : ?$$$B$Wu("**$RM!
$R@i.~~ !     :   ~$$$$$B$$en:``
?MXT@Wx.~    :     ~"##*$$$$M~
)";
    gotoxy(0, 0);
            break;
        }

}
}


DWORD WINAPI SearchWordInLines(LPVOID param)
{
    ThreadData *  data = (ThreadData *)param;

    int start = data->startLine;
    int end = data->endLine;
    int id = data->threadId;
    std::mutex * mtx = data->outMtx;
    std::ofstream * outputFile = data->output;
    std::vector<std::string> lines = data->lines;
    std::string word = data->word;

    for(int i = start; i <= end; ++i)
    {
         if (lines[i].find(word) != std::string::npos) 
         {
            mtx->lock();
            *outputFile << "Thread " << id << ": Line " << i + 1 << ": " << lines[i] << std::endl;
            mtx->unlock();
        }
    }
    return 0;

}

