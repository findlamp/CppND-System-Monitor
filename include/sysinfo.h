#include "ProcessParser.h"
#include <string>
#include <iostream>
#include <vector>
using std::string;
using std::vector;
class SysInfo {
    private:
        vector<string> lastCpuStats;
        vector<string> currentCpuStats;
        vector<string> coresStats;
        vector<vector<string>> lastCpuCoresStats;
        vector<vector<string>> currentCpuCoresStats;
        string cpuPercent;
        float memPercent;
        string osName;
        string kernelVer;
        long upTime;
        int totalProc;
        int runningProc;
        int threads;

    public:
        SysInfo() {
            /*
            Getting initial info about system
            Initial data for individual cores is set
            System data is set
            */
            this->getOtherCores(ProcessParser::getNumberOfCores());
            this->setLastCpuMeasures();
            this->setAttributes();
            this->osName = ProcessParser::getOsName();
            this->kernelVer = ProcessParser::getSysKernelVersion();
        }
        void setAttributes();
        void setLastCpuMeasures();
        string getMemPercent() const;
        long getUpTime() const;
        string getThreads() const;
        string getTotalProc() const;
        string getRunningProc() const;
        string getKernelVersion() const;
        string getOsName() const;
        string getCpuPercent() const;
        void getOtherCores(int _size);
        void setCpuCoresStats();
        vector<string> getCoresStats() const;
};

string SysInfo::getCpuPercent() const 
{
    return this->cpuPercent;
}

string SysInfo::getMemPercent() const 
{
    return std::to_string(this->memPercent);
}

long SysInfo::getUpTime() const 
{
    return this->upTime;
}

string SysInfo::getKernelVersion() const 
{
    return this->kernelVer;
}

string SysInfo::getTotalProc() const 
{
    return std::to_string(this->totalProc);
}

string SysInfo::getRunningProc() const 
{
    return std::to_string(this->runningProc);
}

string SysInfo::getThreads() const 
{
    return std::to_string(this->threads);
}

string SysInfo::getOsName() const 
{
    return this->osName;
}

void SysInfo::setLastCpuMeasures()
{
    this->lastCpuStats = ProcessParser::getSysCpuPercent();
}

void SysInfo::getOtherCores(int _size)
{
    //when number of cores is detected, vectors are modified to fit incoming data
    this->coresStats = vector<string>();
    this->coresStats.resize(_size);
    this->lastCpuCoresStats = vector<vector<string>>();
    this->lastCpuCoresStats.resize(_size);
    this->currentCpuCoresStats = vector<vector<string>>();
    this->currentCpuCoresStats.resize(_size);
    for (int i = 0; i < _size; i++) {
        this->lastCpuCoresStats[i] = ProcessParser::getSysCpuPercent(std::to_string(i));
    }
}

void SysInfo::setCpuCoresStats()
{
    // Getting data from files (previous data is required)
    for(int i = 0; i < this->currentCpuCoresStats.size(); i++) {
        this->currentCpuCoresStats[i] = ProcessParser::getSysCpuPercent(std::to_string(i));
    }
    for(int i = 0; i < this->currentCpuCoresStats.size(); i++) {
        // after acquirement of data we are calculating every core percentage of usage
        this->coresStats[i] = ProcessParser::printCpuStats(this->lastCpuCoresStats[i],this->currentCpuCoresStats[i]);
    }
    this->lastCpuCoresStats = this->currentCpuCoresStats;
}

void SysInfo::setAttributes() 
{
    // getting parsed data
    this->memPercent = ProcessParser::getSysRamPercent();
    this->upTime = ProcessParser::getSysUpTime();
    this->totalProc = ProcessParser::getTotalNumberOfProcesses();
    this->runningProc = ProcessParser::getNumberOfRunningProcesses();
    this->threads = ProcessParser::getTotalThreads();
    this->currentCpuStats = ProcessParser::getSysCpuPercent();
    this->cpuPercent = ProcessParser::printCpuStats(this->lastCpuStats,this->currentCpuStats);
    this->lastCpuStats = this->currentCpuStats;
    this->setCpuCoresStats();
}

// Constructing string for every core data display
vector<string> SysInfo::getCoresStats() const
{
    vector<string> result = vector<string>();
    for (int i = 0; i < this->coresStats.size() ;i++) {
        string temp = ("cpu" + std::to_string(i) +": ");
        float check = stof(this->coresStats[i]);
        if (!check || this->coresStats[i] == "nan") {
            return vector<string>();
        }
        temp += Util::getProgressBar(this->coresStats[i]);
        result.push_back(temp);
    }
    return result;
}
