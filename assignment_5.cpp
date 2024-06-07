#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

struct Process {
    string Pro_ID;
    int Arr_Time;
    int Ser_Time;
    int Remain_Time;

    Process() : Arr_Time(0), Ser_Time(0), Remain_Time(0) {}

    Process(string id, int arrival, int service)
            : Pro_ID(std::move(id)),
              Arr_Time(arrival),
              Ser_Time(service),
              Remain_Time(service) {}
};



struct Remain_TimeComp {
    bool operator()(const Process& p1, const Process& p2) const {
        return p1.Remain_Time > p2.Remain_Time;
    }
};


struct ShortSer_TimeComp {
    bool operator()(const Process& p1, const Process& p2) const {
        return p1.Ser_Time > p2.Ser_Time;
    }
};


void fcfs(const vector<Process>& processes) {
    ofstream outFile("fcfs.out");
    if (!outFile) {
        cerr << "Error opening output file.\n";
        return;
    }

    int currentTime = 0;

    for (const Process& process : processes) {

        while (currentTime < process.Arr_Time) {
            outFile << "IDLE" << endl;
            currentTime += 10;
        }

        // this would execute the process
        for (int x = 0; x < process.Ser_Time; x += 10) {
            outFile << process.Pro_ID << endl;
            currentTime += 10;
        }
    }

    outFile.close();
}



void rr(vector<Process>& processes, int quantum, const string& Out_FilePx) {
    ofstream outFile(Out_FilePx + ".out");
    if (!outFile) {
        cerr << "Error opening output file.\n";
        return;
    }

    queue<Process> readyQu;
    int current_Time = 0;
    size_t index = 0;

    while (!readyQu.empty() || index < processes.size()) {
        if (readyQu.empty() && index < processes.size() && processes[index].Arr_Time > current_Time) {
            current_Time = processes[index].Arr_Time; // this would advance time to the next process arrival if the cpu was idle
        }

        while (index < processes.size() && processes[index].Arr_Time <= current_Time) {
            readyQu.push(processes[index++]);
        }

        if (!readyQu.empty()) {
            Process current_Process = readyQu.front();
            readyQu.pop(); // Then would pop here to remove the process being executed

            int executionTime = min(quantum, current_Process.Remain_Time);
            for (int x = 0; x < executionTime; x += 10) {
                outFile << current_Process.Pro_ID << endl;
                current_Process.Remain_Time -= 10;
                current_Time += 10;


                while (index < processes.size() && processes[index].Arr_Time <= current_Time) {
                    readyQu.push(processes[index++]);
                }
            }

            if (current_Process.Remain_Time > 0) {
                readyQu.push(current_Process); // Then run it back again in the queue
            }
        }
    }

    outFile.close();
}





void spn(const vector<Process>& processes) {
    ofstream outFile("spn.out");
    if (!outFile) {
        cerr << "Error opening output file.\n";
        return;
    }

    priority_queue<Process, vector<Process>, ShortSer_TimeComp> readyQu;
    int current_Time = 0;
    int index = 0;

    while (!readyQu.empty() || index < processes.size()) {
        while (index < processes.size() && processes[index].Arr_Time <= current_Time) {
            readyQu.push(processes[index++]);
        }

        if (!readyQu.empty()) {
            Process current_Process = readyQu.top();
            readyQu.pop();

            for (int x = 0; x < current_Process.Ser_Time; x += 10) {
                outFile << current_Process.Pro_ID << endl;
                current_Time += 10;
            }
        } else {
            current_Time = processes[index].Arr_Time;
        }
    }

    outFile.close();
}



void srt(vector<Process>& processes) {
    ofstream outFile("srt.out");
    if (!outFile) {
        cerr << "Error opening output file.\n";
        return;
    }

    auto cmp = [](const Process& left, const Process& right) {
        return left.Remain_Time > right.Remain_Time ||
               (left.Remain_Time == right.Remain_Time && left.Arr_Time > right.Arr_Time);
    };
    priority_queue<Process, vector<Process>, decltype(cmp)> readyQu(cmp);

    int current_Time = 0;
    size_t index = 0;

    while (index < processes.size() || !readyQu.empty()) {
        while (index < processes.size() && processes[index].Arr_Time <= current_Time) {
            readyQu.push(processes[index]);
            index++;
        }

        if (!readyQu.empty()) {
            Process current_Process = readyQu.top();
            readyQu.pop();

            outFile << current_Process.Pro_ID << endl;
            current_Process.Remain_Time -= 10;
            current_Time += 10;

            if (current_Process.Remain_Time > 0) {
                readyQu.push(current_Process);
            }
        } else {
            if(index < processes.size()) {
                current_Time = processes[index].Arr_Time;
            }
        }
    }

    outFile.close();
}

void hrrn(vector<Process>& processes) {
    ofstream outFile("hrrn.out");
    if (!outFile) {
        cerr << "Error opening output file.\n";
        return;
    }

    vector<Process> readyProcesses;
    int current_Time = 0;
    size_t index = 0;

    while (index < processes.size() || !readyProcesses.empty()) {
        while (index < processes.size() && processes[index].Arr_Time <= current_Time) {
            readyProcesses.push_back(processes[index]);
            index++;
        }

        if (!readyProcesses.empty()) {
            auto it = max_element(readyProcesses.begin(), readyProcesses.end(),
                                  [&current_Time](const Process& a, const Process& b) {
                                      double waitTimeA = current_Time - a.Arr_Time;
                                      double waitTimeB = current_Time - b.Arr_Time;
                                      double rrA = (waitTimeA + a.Ser_Time) / double(a.Ser_Time);
                                      double rrB = (waitTimeB + b.Ser_Time) / double(b.Ser_Time);
                                      return rrA < rrB;
                                  });

            Process current_Process = *it;
            readyProcesses.erase(it);

            for (int x = 0; x < current_Process.Ser_Time; x += 10) {
                outFile << current_Process.Pro_ID << endl;
                current_Time += 10;
            }
        } else {
            if(index < processes.size()) {
                current_Time = processes[index].Arr_Time;
            }
        }
    }

    outFile.close();
}

void feedback(vector<Process>& processes) {
    ofstream outFile("feedback.out");
    if (!outFile) {
        cerr << "Error opening output file.\n";
        return;
    }

    vector<queue<Process>> queues(6);
    int current_Time = 0, nextArrivalIndex = 0;
    bool wasIdle = true;

    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.Arr_Time < b.Arr_Time;
    });

    while (nextArrivalIndex < processes.size() || any_of(queues.begin(), queues.end(), [](const queue<Process>& q) { return !q.empty(); })) {
        while (nextArrivalIndex < processes.size() && processes[nextArrivalIndex].Arr_Time <= current_Time) {
            queues[0].push(processes[nextArrivalIndex++]);
        }

        bool foundProcess = false;
        for (size_t i = 0; i < queues.size() && !foundProcess; ++i) {
            if (!queues[i].empty()) {
                Process process = queues[i].front();
                queues[i].pop();

                int actualTimeSlice = min(process.Remain_Time, 10);

                outFile << process.Pro_ID << endl;

                process.Remain_Time -= actualTimeSlice;
                current_Time += actualTimeSlice;

                wasIdle = false;

                if (process.Remain_Time > 0) {

                    queues[min(i + 1, queues.size() - 1)].push(process);
                }

                foundProcess = true;
            }
        }

        if (!foundProcess) {
            if (wasIdle) {
                outFile << "IDLE" << endl; // This would just log an IDLE state if it was already in IDLE
            }
            current_Time += 10;
            wasIdle = true;
        }
    }

    outFile.close();
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <path_to_source_file>\n";
        return 1;
    }

    string filePath = argv[1];
    ifstream inFile(filePath);
    if (!inFile) {
        cerr << "Error opening input file.\n";
        return 1;
    }

    vector<Process> processes;
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        Process process;
        ss >> process.Pro_ID >> process.Arr_Time >> process.Ser_Time;
        process.Remain_Time = process.Ser_Time; // This would Initialize time for the srt and feedback function
        processes.push_back(process);
    }
    inFile.close();

    // This calls the scheduling function
    fcfs(processes);
    rr(processes, 10, "rr_10");
    rr(processes, 40, "rr_40");
    spn(processes);
    srt(processes);
    hrrn(processes);
    feedback(processes);

    return 0;
}
