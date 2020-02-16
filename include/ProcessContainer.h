#include "process.h"
#include <vector>
using std::string;
using std::vector;

class ProcessContainer {
    public:
        ProcessContainer()
        {
            this->refreshList();
        }
        void refreshList();
        string printList();
        vector<string> getList();

    private:
        vector<Process>_list;
};

void ProcessContainer::refreshList()
{
    vector<string> pids = ProcessParser::getPidList();
    this->_list.clear();
    for (auto pid : pids) {
        Process proc(pid);
        this->_list.push_back(proc);
    }
}

string ProcessContainer::printList()
{
    std::string result="";
    for (auto i : this->_list) {
        result += i.getProcess();
    }
    return result;
}

vector<string> ProcessContainer::getList() 
{
    vector<string> values;
    for (int i = (this->_list.size()-10); i < this->_list.size(); i++){
        values.push_back(this->_list[i].getProcess());
    }
    return values;
}