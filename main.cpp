/******INCLUDE*****/
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <regex>
#include <bits/stdc++.h>
#include <limits>
/******************/

#define DEBUG false

class Step{
    public:
        std::string route;
        std::vector <int> shortestPaths;
        std::vector <std::string> previousNodes;
        int selectedNode;

        Step(const int nodeCount, const std::string& firstNode){
            this->shortestPaths.resize(nodeCount);
            this->previousNodes.resize(nodeCount,"");
            this->route = firstNode;
            std::fill(this->shortestPaths.begin(), this->shortestPaths.end(), INT_MAX);
        }

        Step(const Step &oldStep, const std::string& newNode){
            this->shortestPaths = oldStep.shortestPaths;
            this->previousNodes = oldStep.previousNodes;
            this->shortestPaths.at(oldStep.selectedNode) = -1;
            this->previousNodes.at(oldStep.selectedNode) = "\u221E";
            this->route = oldStep.route + newNode;
        }

        void updateDistance(int destNode, const std::string& srcNode, int distance){
            this->shortestPaths.at(destNode) = distance;
            this->previousNodes.at(destNode) = srcNode;
        }

        void print(int stepOrder, int tL, const std::vector <int>& allL, int maxLen, const std::vector <std::string> &nodes) const {
            int spaceCount = int(tL) + 4;
            std::cout << stepOrder <<
                        std::string(spaceCount-route.size(), ' ') <<
                        route;
            int usedSpace=0;
            for (int i = 1; i < shortestPaths.size(); i++) {
                int currentL = allL[i];
                std::string writingStr;
                if( shortestPaths[i] == INT_MAX){
                    writingStr = "\u221E";
                    writingStr.insert(0, "  ");
                }else if(shortestPaths[i] != -1){
                    writingStr = std::to_string(this->shortestPaths[i]) + "," + this->previousNodes[i];
                }else{
                    writingStr = "   ";
                }
                writingStr.insert(0, std::string(maxLen + 5, ' '));
                std::cout << writingStr;
            }
            if(stepOrder != nodes.size()-1) std::cout << "\t\tSELECTED NODE: " << nodes[selectedNode];
            std::cout << std::endl;
        }
};

/*****************PROTOTYPE*****************/
std::vector <std::string> readLines(std::ifstream &, std::vector <int> &);
bool isSyntaxCorrect(const std::string&, const std::vector <std::string>&);
std::string deleteDuplicateWhiteSpaces(std::string);
int wordCount(std::string);
std::string extractFromString(const std::string&, int);
bool isNumber(std::string);
int digitCountOf(int);
std::vector <std::string> findNodes(const std::vector <std::string>&);
bool isNodeExists(const std::string&, std::vector <std::string>);
void freeMatrix(int, int **);
void drawTmpGraph(int, int **, std::vector <std::string>&);
void updateNeighbors(std::vector <bool>&, std::vector <int>&, int, int**, std::vector <int>&, Step&, const std::string&);
std::vector <Step> dijkstra(int **, const std::vector <std::string>&);
/********************************************/

int digitCountOf(int n){
    if (n == 0) return 0;
    return 1 + digitCountOf(n / 10);
}

bool isNumber(std::string str){
    return !str.empty() && std::find_if(str.begin(),
                                      str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
}

std::string extractFromString(const std::string& str, int order){
    if (order == 0) {
        std::string firstWord = str.substr(0, str.find(' '));
        return firstWord;
    } else if (order == 1) {
        std::string remainder = str.substr(str.find(' ') + 1, str.size() - 1);
        std::string secondWord = remainder.substr(0, remainder.find(' '));
        return secondWord;
    }
    return str.substr(str.find_last_of(' ') + 1);
}

int wordCount(std::string str){
    std::regex pattern("(\\w+)(' ')*");
    auto words_begin = std::sregex_iterator(str.begin(), str.end(), pattern);
    auto words_end = std::sregex_iterator();
    return std::distance(words_begin, words_end);
}

bool isThereSamePath(const std::string& line, const std::vector <std::string>& lines){
    std::string firstNode = extractFromString(line, 0);
    std::string secondNode = extractFromString(line, 1);
    for(const auto& curLine:lines){
        std::vector <std::string> tmp{
            extractFromString(curLine, 0),
            extractFromString(curLine, 1)
        };
        if((std::find(tmp.begin(), tmp.end(), firstNode) != tmp.end()) &&
        std::find(tmp.begin(), tmp.end(), secondNode) != tmp.end()){
            tmp.clear();
            return true;
        }
    }
    return false;
}

bool isSyntaxCorrect(const std::string& line, const std::vector <std::string>& lines){
    return !(wordCount(line) != 3 ||
             !isNumber(extractFromString(line, 2)) ||
             extractFromString(line, 2).length() > digitCountOf(INT_MAX) ||
             std::stoi(extractFromString(line, 2)) < 0 ||
             extractFromString(line, 0) == extractFromString(line, 1) ||
             isThereSamePath(line, lines));
}

std::string deleteDuplicateWhiteSpaces(std::string str) {
    std::regex pattern("(\\w+)(' ')*");
    auto words_begin = std::sregex_iterator(str.begin(), str.end(), pattern);
    auto words_end = std::sregex_iterator();

    std::string tempStr;
    for (auto i = words_begin; i != words_end; ++i) {
        auto match = *i;
        tempStr.append(match[1]);
        tempStr.append(" ");
    }
    return tempStr.substr(0, tempStr.length() - 1);
}

std::vector <std::string> readLines(std::ifstream &infile, std::vector <int> &brokenLines){
    int c = 0;
    std::vector <std::string> lines;
    std::string tmpString;
    while(getline(infile, tmpString)){
        c++;
        if (tmpString.find_first_not_of(' ') != std::string::npos) {
            tmpString = deleteDuplicateWhiteSpaces(tmpString);
            if(isSyntaxCorrect(tmpString, lines)) lines.push_back(tmpString);
            else brokenLines.push_back(c);
        }
    }
    return lines;
}

bool isNodeExists(const std::string& tmpNode, std::vector <std::string> nodes){
    return std::find(nodes.begin(), nodes.end(), tmpNode) != nodes.end();
}

std::vector <std::string> findNodes(const std::vector <std::string>& lines){
    std::vector <std::string> nodes;
    for (const std::string& line : lines){
        for (int i = 0; i <= 1; i++) {
            std::string tmpNode = extractFromString(line, i);
            if (!isNodeExists(tmpNode, nodes)) {
                nodes.push_back(tmpNode);
            }
        }
    }
    return nodes;
}

int **initializeMatrix(int nodeCount, const std::vector <std::string>& lines, const std::vector <std::string>& nodes){
    int** matrix = new int*[nodeCount];
    for (int i = 0; i < nodeCount; ++i)
        matrix[i] = new int[nodeCount];

    for (int j = 0; j < nodeCount; j++) {
        for (int i = 0; i < nodeCount; i++) {
            if(j==i) matrix[j][i] = 0;
            else matrix[j][i] = -1;
        }
    }
    for (int k = 0; k < nodeCount; k++) {
        for (int i = 0; i < nodeCount; i++) {
            if(k!=i){
                for (const std::string& line : lines){
                    for (int l = 0; l < 2; l++) {
                        if (extractFromString(line, l) == nodes[k] && extractFromString(line, 1 - l) == nodes[i]){
//                            std::cout << k << ' ' << i << ' ' << std::stoi(extractFromString(line, 2)) << '\n';
                            matrix[k][i] = std::stoi(extractFromString(line, 2));
                        }
                    }
                }
            }
        }
    }
    return matrix;
}

void freeMatrix(int nodeCount, int **matrix){
    for (int i = 0; i < nodeCount; ++i)
        delete [] matrix[i];
    delete [] matrix;
}

void drawTmpGraph(int nodeCount, int **matrix, std::vector <std::string>& nodes){
    std::cout << "\n\n  ";
    for (const auto & node : nodes) {
        std::cout << "D(" << node << ") ";
    }
    std::cout << '\n';
    for (int i = 0; i < nodeCount; ++i) {
        std::cout << nodes[i] << ' ';
        for (int j = 0; j < nodeCount; ++j) {
            if(matrix[i][j] == -1) std::cout << ' ' << "\u221E" << "   ";
            else std::cout << ' ' << matrix[i][j] << "   ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

void updateNeighbors(std::vector <bool>& unvisitedSet, std::vector <int>& distance, int currentNode,
        int** adjacencyMatrix, std::vector <int>& previousVertex, Step& currentStep, const std::string& currentName){
    int sizeOfNode = unvisitedSet.size();
    for (int j = 0; j < sizeOfNode; j++) {
        if (adjacencyMatrix[currentNode][j] != -1){
            if (unvisitedSet[j] != false){
                if (distance[currentNode] + adjacencyMatrix[currentNode][j] < distance[j]){
                    distance[j] = distance[currentNode] + adjacencyMatrix[currentNode][j];
                    previousVertex[j] = currentNode;
                    currentStep.updateDistance(j, currentName, distance[j]);
                }
            }
        }
    }
}

int cheapestNode(const std::vector <int>& distance, std::vector <bool> unvisitedSet){
    std::vector <std::pair <int, int>> temp;
    int sizeOf = distance.size();
    for (int i = 0; i <sizeOf; ++i) {
        temp.push_back(std::make_pair(distance[i], i));
    }
    std::sort(temp.begin(), temp.end());
    for (int j = 0; j <sizeOf; ++j) {
        if (temp[j].first != INT_MAX)
            if(unvisitedSet[temp[j].second])
                return temp[j].second;
    }
    std::cout << "ERROR WHILE FINDING BEST WAY";
    return -1;
}

std::vector <Step> dijkstra(int **paths, const std::vector <std::string>& nodes){
    const int sizeOfNodes = nodes.size();
    std::vector <int> distance(sizeOfNodes, INT_MAX);
    std::vector <bool> unvisitedSet(sizeOfNodes, true);
    std::vector <int> previousVertex(nodes.size());

    int initialNode = 0, currentNode;
    distance[initialNode] = 0;
    std::vector <Step> steps;
    for (; !(std::find(begin(unvisitedSet), end(unvisitedSet), true) == end(unvisitedSet)) ; unvisitedSet[currentNode] = false){
        currentNode = cheapestNode(distance, unvisitedSet);
        //std::cout << "Current Node: " << currentNode << std::endl;
        if(currentNode == initialNode){
            steps.emplace_back(sizeOfNodes, nodes[initialNode]);
        }else{
            steps.back().selectedNode = currentNode;
            steps.emplace_back(steps.back(), nodes[currentNode]);
        }
        updateNeighbors(unvisitedSet, distance, currentNode, paths, previousVertex, steps.back(), nodes[currentNode]);
    }
    return steps;
}

std::vector <int> getLengths(const std::vector <std::string> nodes){
    std::vector <int> lengths;
    for (const auto & node : nodes) {
        lengths.push_back(node.size());
    }
    return lengths;
}

void printSteps(const std::vector <Step>& steps, const std::vector <std::string>& nodes){
    std::string opt;
    std::cout << "Enter step count, 'q' then Enter to quit.\nPress Enter to start";
    std::cin.ignore();
    int j = 0;
    std::vector <int> lengths = getLengths(nodes);
    int totalLength = 0;
    int maxLength = 0;
    do{
        std::cout << "Total step count is " << nodes.size() << std::endl;
        std::cout << "Enter step number to see: ";
        std::cin >> opt;
        system("clear");
        while(!isNumber(opt) && opt != "q"){
            std::cout << "Enter integer: ";
            std::cin >> opt;
        }
        if(opt == "q"){
            std::cout << "Exiting...";
            break;
        }
        if(std::stoi(opt) > 0 && std::stoi(opt) < nodes.size()){
            j = std::stoi(opt);
        }
        std::cout << "Step";
        totalLength = 0;
        for (const auto & node : nodes) {
            int curLen = node.size();
            totalLength += curLen;
            if(curLen > maxLength) maxLength = curLen;
            for (int i = 0; i < curLen; ++i) {
                std::cout << ' ';
            }
        }
        std::cout << "N'";

        for (int k = 1; k < steps.size(); ++k) {
            std::cout << std::string(maxLength+4, ' ')<< "p(" << nodes[k] << ')';
        }
        int lineSize = 6 + totalLength*2 + ((maxLength+7)*int(nodes.size()));
        std::cout << std::endl << std::string(lineSize, '-') << std::endl;

        for (int i = 0; i < j; ++i) {
            steps[i].print(i, totalLength, lengths, maxLength, nodes);
            std::cout << std::string(lineSize, '-') << std::endl;
        }
    }while(opt != "q");

}

int main() {
    std::cout << "Input Validation Control Starting...";
    std::ifstream fileStream;
    fileStream.open("dijkstra.txt");

    if(fileStream.fail()){
        std::cerr << "\nError: " << strerror(errno);
        return -1;
    }

    std::vector <int> brokenLines;
    std::vector <std::string> lines = readLines(fileStream, brokenLines);
    fileStream.close();

    if(!brokenLines.empty()){
        std::cout << "\nFollowing lines are have unexpected format, if you continue those lines will ignore!";
        std::cout << " Lines are: {";
        for (int brokenLine : brokenLines) {
            if (brokenLine != brokenLines.back()) std::cout << brokenLine << ", ";
            else std::cout << brokenLine;
        }
        std::cout << '}';
        char c = 0;
        while(c != 'Y' && c != 'y' && c != 'N' && c != 'n'){
            std::cout << "\nPress Y/y to continue or N/n to determine!";
            std::cin >> c;
        }
        if(c == 'N' || c == 'n') return -1;
    }

    std::vector <int> ().swap(brokenLines);
    if(!lines.empty() ){
        std::cout << "\nSyntax analysis is over, initializing dependencies...";
        std::vector <std::string> nodes = findNodes(lines);
        int **matrix = initializeMatrix(nodes.size(), lines, nodes);

        std::vector <std::string> ().swap(lines); // Delete lines vector

        if(DEBUG){
            std::cout << "Adjecency matrix is:\n";
            drawTmpGraph(nodes.size(), matrix, nodes);
        }
        std::vector <Step> steps = dijkstra(matrix, nodes);

        printSteps(steps, nodes);

        freeMatrix(nodes.size(), matrix);
        std::vector <std::string> ().swap(nodes);
    }else{
        std::cout << "There is no line";
    }
    return 0;
}