#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include "util.h"
class ProcessParser {
    public:
        static std::string getCmd(std::string pid);
        static std::vector<std::string> getPidList();
        static std::string getVmSize(std::string pid);
        static std::string getCpuPercent(std::string pid);
        static long int getSysUpTime();
        static std::string getProcUpTime(std::string pid);
        static std::string getProcUser(std::string pid);
        static std::vector<std::string> getSysCpuPercent(std::string coreNumber = "");
        static float getSysRamPercent();
        static std::string getSysKernelVersion();
        static int getTotalThreads();
        static int getTotalNumberOfProcesses();
        static int getNumberOfRunningProcesses();
        static string getOsName();
        static std::string printCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
        static int getNumberOfCores();
        static float getSysActiveCpuTime(vector<string> values);
        static float getSysIdleCpuTime(vector<string>values);
};
 
std::string ProcessParser::getVmSize(std::string pid)
{
    std::string line;
    std::string name= "VmData";
    std::string value;
    float result;
    //Opening steam for specific file
    ifstream stream = Util::getStream((Path::basePath() + "/" + pid + Path::statusPath()));
    while(std::getline(stream,line)){
        //Search line by line
        if(line.compare(0,name.size(),name)==0){
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf),end;
            vector<std::string>values(beg,end);
            //conversion kB ->GB
            result = (stof(values[1])/float(1024*1024));
            break;
        }
    };
    return to_string(result);

}

std::string ProcessParser::getCpuPercent(std::string pid)
{  
    std::string line;
    ifstream stream = Util::getStream((Path::basePath() + "/" + pid + Path::statPath()));
    std::getline(stream,line);
    std::istringstream buf(line);
    std::istream_iterator<std::string> beg(buf),end;
    std::vector<std::string> values(beg, end);
    float utime = stof(ProcessParser::getProcUpTime(pid));

    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);

    float uptime = ProcessParser::getSysUpTime();

    float freq = sysconf(_SC_CLK_TCK);

    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/freq);
    float result = 100.0 * ((total_time/freq)/seconds);

    return to_string(result);

}

std::string ProcessParser::getProcUpTime(std::string pid)
{
    std::string line;
    ifstream stream = Util::getStream((Path::basePath() + "/"+ pid + Path::statPath()));
    getline(stream,line);
    std::istringstream buf(line);
    std::istream_iterator<std::string> beg(buf),end;
    std::vector<std::string> values(beg,end);
    return to_string(float(stof(values[13])/sysconf(_SC_CLK_TCK)));
    
}

long int ProcessParser::getSysUpTime()
{
    std::string line;
    ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
    std::getline(stream,line);

    std::istringstream buf(line);
    std::istream_iterator<string> beg(buf), end;
    std::vector<string> values(beg, end);
    return stoi(values[0]);
}

std::string ProcessParser::getProcUser(string pid)
{
    std::string line;
    std::string name = "Uid:";
    std::string result ="";
    std::ifstream stream = Util::getStream((Path::basePath() + "/" +pid + Path::statusPath()));
    // Getting UID for user
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            std::vector<string> values(beg, end);
            result =  values[1];
            break;
        }
    }
    stream = Util::getStream("/etc/passwd");
    name =("x:" + result);
    // Searching for name of the user with selected UID
    while (std::getline(stream, line)) {
        if (line.find(name) != std::string::npos) {// line.find() will EITHER return the position of the first appearance of 'a' in some_string if 'a' appears at least once in some_string, OR it will return a value that is equal to std::string::npos if 'a' does not appear at all in some_string.
            result = line.substr(0, line.find(":")); 
            return result;
        }
    }
    return "";
}

vector<string> ProcessParser::getPidList()
{
    DIR* dir;
    // Basically, we are scanning /proc dir for all directories with numbers as their names
    // If we get valid check we store dir names in vector as list of machine pids
    vector<std::string> container;
    if(!(dir=opendir("/proc")))
        throw std::runtime_error(std::strerror(errno));
    
    while(dirent* dirp = readdir(dir)){
        // is this a directory?
        if(dirp->d_type !=DT_DIR)
            continue;
        // IS every character of the name a digit?
        if(all_of(dirp->d_name,dirp->d_name + std::strlen(dirp->d_name),[](char c){return std::isdigit(c);})){
            container.push_back(dirp->d_name);// dirent and DIR struct ?????????
        }
    }

    //Validating process of directory closing
    if(closedir(dir))
        throw std::runtime_error(std::strerror(errno));
    return container;
}

std::string ProcessParser::getCmd(std::string pid){
    std::string line;
    std::ifstream stream = Util::getStream((Path::basePath() + "/" + pid + "/" + Path::cmdPath()));
    std::getline(stream, line);
    return line;
}

int ProcessParser::getNumberOfCores(){
    //Get the number of host cpu cores
    std::string line;
    std::string name = "cpu cores";
    std::ifstream stream = Util::getStream((Path::basePath() + "/cpuinfo"));
    while(std::getline(stream, line)){
        if(line.compare(0,name.size(),name)==0){
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf),end;
            std::vector<std::string> values(beg,end);
            return stoi(values[3]);

        }
    }
    return 0;
}

std::vector<std::string> ProcessParser::getSysCpuPercent(string coreNumber)
{
    std::string line;
    std::string name = "cpu"+coreNumber;
    
    std::ifstream stream = Util:getStream((Path::basePath()+Path::statPath()));
    while(std::getline(stream,line)){
        if(line.compare(0,name.size(),name)==0){
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf),end;
            std::vector<std::string> values(beg,end);
            return values;
        }
    }
}

std::string ProcessParser::printCpuStats(std::vector<std::string> values1, std::vector<std::string> values2)
{
/*
Because CPU stats can be calculated only if you take measures in two different time,
this function has two parameters: two vectors of relevant values.
We use a formula to calculate overall activity of processor.
*/
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}

float ProcessParser::getSysActiveCpuTime(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float ProcessParser::getSysIdleCpuTime(vector<string>values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}


float ProcessParser::getSysRamPercent()
{
    std::string line;
    std::string name1 = "MemAviable:";
    std::string name2 = "MemFree:";
    std::string name3 = "Buffers";

    std::ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
    float total_mem = 0;
    float free_mem = 0;
    float buffers = 0;
    while(std::getline(stream,line)){
        if (total_mem != 0 && free_mem != 0)
            break;
        if(line.compare(0,name1.size(),name1)==0){
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf),end;
            vector<string> values(beg,end);
            total_mem = stof(values[1]);
        }
        if(line.compare(0,name2.size(),name2)==0){
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf),end;
            vector<string> values(beg,end);
            free_mem = stof(values[1]);
        }
        if(line.compare(0,name3.size(),name3)==0){
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf),end;
            vector<string> values(beg,end);
            buffers = stof(values[1]);
        }

    }
    // calculating usage:
    return float(100.0*(1-(free_mem/(total_mem-buffers))));

}

std::string ProcessParser::getSysKernelVersion()
{
    std::string line;
    std::string name = "Linus version ";
    std::ifstream = Util::getStream((Path::basePath() + Path::versionPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return values[2];
        }
    }
    return "";
}

std::string ProcessParser::getOsName()
{
    string line;
    string name = "PRETTY_NAME=";

    ifstream stream = Util::getStream(("/etc/os-release"));

    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
              std::size_t found = line.find("=");
              found++;
              string result = line.substr(found);
              result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
              return result;
        }
    }
    return "";

}

int ProcessParser::getTotalThreads()
{
    string line;
    int result = 0;
    string name = "Threads:";
    vector<string>_list = ProcessParser::getPidList();
    for (int i=0 ; i<_list.size();i++) {
    string pid = _list[i];
    //getting every process and reading their number of their threads
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}

int ProcessParser::getTotalNumberOfProcesses()
{
    string line;
    int result = 0;
    string name = "processes";
    ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}

int ProcessParser::getNumberOfRunningProcesses()
{
    string line;
    int result = 0;
    string name = "procs_running";
    ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}