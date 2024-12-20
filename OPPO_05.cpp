#include <iostream>
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
void sortFilesByDate(vector<FileInfo>& files);
void filterByRadius(const vector<FileInfo>& files, double minRadius, double maxRadius);
string convertDateToSortableFormat(const string& date);

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

    cout << "File data (unsorted):\n";
    for (const auto& item : files) {
        cout << item.date << " | " << item.name << " | " << item.radius << " kilometers\n";
    }

    try {
        sortFilesByDate(files);
    }
    catch (const runtime_error& e) {
        cerr << "Error during date sorting: " << e.what() << "\n";
        return 1;
    }


    cout << "\nFile data (sorted by date):\n";
    for (const auto& item : files) {
        cout << item.date << " | " << item.name << " | " << item.radius << " kilometers\n";
    }

    double minRadius, maxRadius;
    cout << "\nEnter radius range (min max): ";
    cin >> minRadius >> maxRadius;
    filterByRadius(files, minRadius, maxRadius);

    return 0;
}

void sortFilesByDate(vector<FileInfo>& files) {
    sort(files.begin(), files.end(), [](const FileInfo& a, const FileInfo& b) {
        string dateA = convertDateToSortableFormat(a.date);
        string dateB = convertDateToSortableFormat(b.date);
        if (dateA.empty() || dateB.empty()) {
            throw runtime_error("Invalid date format encountered.");
        }
        return dateA < dateB;
        });
}

string convertDateToSortableFormat(const string& date) {
    if (date.size() != 10 || date[2] != '.' || date[5] != '.') {
        cerr << "Invalid date format: " << date << "\n";
        return "";
    }
    return date.substr(6, 4) + date.substr(3, 2) + date.substr(0, 2);
}

void filterByRadius(const vector<FileInfo>& files, double minRadius, double maxRadius) {
    cout << "\nPlanets within radius range (" << minRadius << " - " << maxRadius << " kilometers):\n";
    for (const auto& file : files) {
        if (file.radius >= minRadius && file.radius <= maxRadius) {
            cout << file.name << " | " << file.radius << " kilometers\n";
        }
    }
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
    auto radii = extractData(s, R"(\b\d+\.\d+\b)");
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