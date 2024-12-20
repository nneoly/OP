#include <iostream>
#include <clocale>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <algorithm>
using namespace std;

struct FileInfo {
    string name;
    string date;
    double radius;
};

std::ostream& operator<<(std::ostream& stream, const FileInfo& fileInfo);
vector<string> extractData(const string& data, const string& regex);
string getFileName(const string& s);
string getFileDate(const string& s);
double getFileRadius(const string& s);
bool compareByDate(const FileInfo& a, const FileInfo& b);
string convertDateToSortableFormat(const string& date);
vector<FileInfo> filterByRadius(const vector<FileInfo>& files, double minRadius, double maxRadius);

int main() {
    ifstream ist("file.txt");
    if (!ist) {
        cerr << "Error: Unable to open file.\n";
        return 1;
    }

    vector<FileInfo> files;
    string s;
    while (getline(ist, s)) {
        try {
            files.push_back({ getFileName(s), getFileDate(s), getFileRadius(s) });
        }
        catch (const runtime_error& e) {
            cerr << "Error parsing line: " << s << " - " << e.what() << "\n";
        }
    }

    cout << "Unsorted File data:\n";
    for (const auto& item : files) {
        cout << item.date << " | " << item.name << " | " << item.radius << " kilometers\n";
    }

    sort(files.begin(), files.end(), compareByDate);

    cout << "\nSorted File data:\n";
    for (const auto& item : files) {
        cout << item.date << " | " << item.name << " | " << item.radius << " kilometers\n";
    }

    double minRadius, maxRadius;
    cout << "\nEnter radius range (min max): ";
    cin >> minRadius >> maxRadius;
    auto filteredFiles = filterByRadius(files, minRadius, maxRadius);
    if (filteredFiles.empty()) {
        cout << "\nNo planets found within the given radius range.\n";
    }
    else {
        cout << "\nPlanets within radius range (" << minRadius << " - " << maxRadius << "):\n";
        for (const auto& file : filteredFiles) {
            cout << file.name << " | " << file.radius << " kilometers | Opened on: " << file.date << "\n";
        }
    }

    return 0;
}

bool compareByDate(const FileInfo& a, const FileInfo& b) {
    return convertDateToSortableFormat(a.date) < convertDateToSortableFormat(b.date);
}

string convertDateToSortableFormat(const string& date) {
    return date.substr(6, 4) + date.substr(3, 2) + date.substr(0, 2);
}

vector<FileInfo> filterByRadius(const vector<FileInfo>& files, double minRadius, double maxRadius) {
    vector<FileInfo> result;
    for (const auto& file : files) {
        if (file.radius >= minRadius && file.radius <= maxRadius) {
            result.push_back(file);
        }
    }
    return result;
}

string getFileName(const string& s) {
    auto names = extractData(s, R"("[^"]*")");
    if (names.empty()) {
        throw runtime_error("File name not found");
    }
    return names[0].substr(1, names[0].size() - 2);
}

string getFileDate(const string& s) {
    auto dates = extractData(s, R"(\b\d{2}\.\d{2}\.\d{4}\b)");
    if (dates.empty()) {
        throw runtime_error("Invalid or missing date");
    }
    return dates[0];
}

double getFileRadius(const string& s) {
    auto radii = extractData(s, R"(\b\d+\.\d+|\d+\b)");
    if (radii.empty()) {
        throw runtime_error("Radius not found");
    }
    return stod(radii.back());
}

vector<string> extractData(const string& data, const string& regex) {
    std::regex pattern(regex);
    sregex_iterator begin(data.begin(), data.end(), pattern), end;
    vector<string> result;
    for (auto it = begin; it != end; ++it) {
        result.push_back(it->str());
    }
    return result;
}

std::ostream& operator<<(std::ostream& stream, const FileInfo& fileInfo) {
    return stream << fileInfo.date << ", " << fileInfo.name << ", " << fileInfo.radius << " kilometers";
}
