#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <stack>
#include <algorithm>

std::string vt;
std::string vn;
std::map<std::string, std::vector<std::string>> epsilonMap;
std::map<std::vector<std::string>, std::string> reverseEpsilonMap;
std::map<std::string, std::string> firstMap;
std::map<std::string, std::string> followMap;
std::map<std::pair<std::string, char> , std::string> LL1List;
std::string initialKey;
bool haveNew = true;//标志follow集是否加入了新的元素，true表示加入了新的元素

std::string formatStr(const std::string& str);

std::string getInput();

void showStack(std::stack<std::string> stack);

void errorInf(char ch, size_t errorLoc, const std::string &initialStr, const std::string& expectedChar);

void setEpsilon();//设置产生式

void getEpsilon();//打印产生式

void setVT();//设置终结符

void getVT();//打印终结符的信息

void setVN();//设置非终结符

void getVN();//打印非终结符的信息

void delDirectLeftCur();//消除左递归

std::string computeFirst(const std::string &M);//计算first集

void computeFollow(const std::vector<std::string> &vs);//计算follow集

std::vector<std::string> getKeySetOfEpsilon(const std::map<std::string, std::vector<std::string>> &epsilonMap1);

bool hasLeftCursive(const std::string &left);

//如果有新加入的字符那么返回true
bool noDuplicateAppend(std::string &str, const std::string &addString);//把addString中不存在与str的字符加入到str中

//如果有新加入的字符那么返回true
bool noDuplicateAppend(std::string &str, char addChar);//如果str含有addChar那么不执行任何操纵，否则加入str尾部

std::map<std::string, std::string> getFirst();

void showFirstMap(const std::map<std::string, std::string> &firstMapT);

std::map<std::vector<std::string>, std::string> reverseMap();

void showReverseMap();

void showFollowMap();

void getFollow();

void setInitialKey();

void getLL1List();

void showLL1List();

void analysisProcess();

int main() {
    setEpsilon();
    setVT();
    setVN();
    getVN();
    getVT();
    setInitialKey();
    delDirectLeftCur();
    getEpsilon();
    firstMap = getFirst();
    showFirstMap(firstMap);
    reverseEpsilonMap = reverseMap();
    getFollow();
    showFollowMap();
    getLL1List();
    showLL1List();
    analysisProcess();
    return 0;
}

void setEpsilon() {
    std::cout
            << "Please enter epsilon, enter \"quit\" to end: (epsilon be like E->E-F or F->(E), with no space, one line one epsilon)"
            << std::endl;
    std::string tempEpsilon;
    std::cin >> tempEpsilon;
    while (tempEpsilon != "quit") {
        std::string leftStr;
        for (char i: tempEpsilon) {
            if (i != '-') {
                leftStr.append(sizeof(char), i);
            } else {
                break;
            }
        }
        epsilonMap[leftStr].emplace_back(tempEpsilon.erase(0, 3));
        std::cin >> tempEpsilon;
    }
    std::cout << "Have set the epsilons successfully!" << std::endl;
}

void getEpsilon() {
    std::cout << "epsilon as follow:" << std::endl;
    for (auto &it: epsilonMap) {
        for (auto it1 = it.second.begin(); it1 != it.second.end(); it1++) {
            std::cout << it.first << "->" << *it1 << std::endl;
        }
    }
}

void setVT() {
    std::cout << "Please enter the number of VT: " << std::endl;
    size_t sizeOfVT;
    std::cin >> sizeOfVT;
    std::cout << "Please enter the vt: (one line one vt)" << std::endl;
    char tempVT;
    for (int i = 0; i < sizeOfVT; i++) {
        std::cin >> tempVT;
        vt.append(1, tempVT);
    }
    std::cout << "Have set the vt successfully!" << std::endl;
}

void getVT() {
    std::cout << "list vt as follow:" << std::endl;
    for (char i: vt) {
        std::cout << i << std::endl;
    }
}

void setVN() {
    std::cout << "Please enter the number of VN: " << std::endl;
    size_t sizeOfVN;
    std::cin >> sizeOfVN;
    std::cout << "Please enter the vn: (one line one vn)" << std::endl;
    char tempVT;
    for (int i = 0; i < sizeOfVN; i++) {
        std::cin >> tempVT;
        vn.append(1, tempVT);
    }
    std::cout << "Have set the vn successfully!" << std::endl;
}

void getVN() {
    std::cout << "list vn as follow:" << std::endl;
    for (char i: vn) {
        std::cout << i << std::endl;
    }
}

void delDirectLeftCur() {
    std::vector<std::string> keySet = getKeySetOfEpsilon(epsilonMap);
    for (auto curKey: keySet) {//遍历每一个非终结符
        std::string newState;
        if (hasLeftCursive(curKey)) {//当前非终结符产生式存在左递归
            //创建新的非终结符, 在原本的非终结符后面加上‘
            newState.append(curKey);
            newState.append(sizeof(char), '\'');
        } else {//不存在左递归，那么直接跳到下一个非终结符
            continue;
        }
        std::vector<std::string> &curVector = epsilonMap[curKey];
        for (int j = 0; j < curVector.size(); j++) {//遍历非终结符的产生式
            std::string &curString = curVector[j];//当前产生式
            //如果产生式的第一个非终结符等于curKey的第一个字符，那么这个产生式是左递归的，把后面的式子加入新的非终结符产生式中,并删除原来左递归的产生式
            if (curString[0] == curKey[0]) {
                epsilonMap[newState].emplace_back(curString.erase(0, 1) + newState);
                curVector.erase(curVector.begin() + j);//删除产生式，后面的产生式的索引都变大了
                j--;//j减小以适应之前删除产生式的操作
            } else {//如果不等于第一个字符，那么在后面加上新的非终结符，变成新产生式
                curString.append(newState);
            }
        }
        //最后在新的非终结符产生式中加入空
        epsilonMap[newState].emplace_back("e");
    }
}

std::string computeFirst(const std::string &M) {
    std::string res;
    if (vt.find(M[0]) != std::string::npos)//是终结符
    {
        noDuplicateAppend(res, M[0]);
    } else {
        //是非终结符
        for (int i = 0; i <= epsilonMap[M].size() - 1; i++) {
            int k = 0;
            while (epsilonMap[M][i][k] == 'e' && k < epsilonMap[M][i].size()) {
                k++;
            }
            if (k < epsilonMap[M][i].size()) {
                noDuplicateAppend(res, computeFirst(std::string(1, epsilonMap[M][i][k])));
            } else {
                noDuplicateAppend(res, 'e');
            }
        }
    }
    return res;
}

void computeFollow(const std::vector<std::string> &vs) {
    std::string key = reverseEpsilonMap[vs];
    for (const auto& str: vs) {
        for (auto rit = str.rbegin(); rit != str.rend(); rit++) {
            if (rit != str.rbegin() && vn.find(*rit) != std::string::npos) {//不在末尾并且是非终结符
                char lastChar = *(rit - 1);
                std::string curVN(1, *rit);
                std::string lastVN;
                if (lastChar == '\'') {
                    curVN.append(1, '\'');
                    if (rit - 2 >= str.rbegin()) {
                        lastChar = *(rit - 2);
                    } else {//非终结符是结尾
//                        printf("add follow(%s) to follow(%s)\n", key.c_str(), curVN.c_str());
                        if(noDuplicateAppend(followMap[curVN], followMap[key]))
                            haveNew = true;
//                        printf("current haveNew is: %d\n", haveNew);
                    }
                }
                if (vt.find(lastChar) != std::string::npos) {//非终结符后面有终结符
//                    printf("add %c to follow(%s)\n", lastChar, curVN.c_str());
                    if(noDuplicateAppend(followMap[curVN], lastChar))
                        haveNew = true;
//                    printf("current haveNew is: %d\n", haveNew);
                } else {//非终结符后面是非终结符
                    lastVN.append(1, lastChar);
                    if(*(rit - 2) == '\''){
                        lastVN.append(1, '\'');
                    }
                    if (firstMap[lastVN].find('e') != std::string::npos) {//first集中有空，加入左侧非终结符follow集
//                        printf("add follow(%s) to follow(%s)\n", key.c_str(), curVN.c_str());
                        if(noDuplicateAppend(followMap[curVN], followMap[key]))
                            haveNew = true;
//                        printf("current haveNew is: %d\n", haveNew);
                    }
                    //加入后续非终结符中的first集中的非空元素
                    for(auto ele : firstMap[lastVN]){
                        if(ele != 'e'){
//                            printf("add %c to follow(%s)\n",ele , curVN.c_str());
                            if(noDuplicateAppend(followMap[curVN], ele))
                                haveNew = true;
//                            printf("current haveNew is: %d\n", haveNew);
                        }
                    }
                }
            } else if (vn.find(*rit) != std::string::npos){
                if(noDuplicateAppend(followMap[std::string(1, *rit)], followMap[key]))
                    haveNew = true;
            }
        }
    }
}

std::vector<std::string> getKeySetOfEpsilon(const std::map<std::string, std::vector<std::string>> &epsilonMap1) {
    std::vector<std::string> keys;
    keys.reserve(epsilonMap1.size());
    for (auto &it: epsilonMap1) {
        keys.emplace_back(it.first);
    }
    return keys;
}

bool noDuplicateAppend(std::string &str, char addChar) {
    if (str.find(addChar) == std::string::npos) {
        str.append(1, addChar);
        return true;
    }
    return false;
}

bool noDuplicateAppend(std::string &str, const std::string &addString) {
    bool flag = false;
    for (char addChar: addString) {
        if (str.find(addChar) == std::string::npos) {
            str.append(1, addChar);
            flag = true;
        }
    }
    return flag;
}

std::map<std::string, std::string> getFirst() {
    std::vector<std::string> keySet = getKeySetOfEpsilon(epsilonMap);
    std::map<std::string, std::string> firstMapT;
    std::string tempFirst;
    for (const auto &key: keySet) {
        tempFirst = computeFirst(key);
        firstMapT[key] = tempFirst;
    }
    return firstMapT;
}

void showFirstMap(const std::map<std::string, std::string> &firstMapT) {
    std::cout << std::endl << "FIRST print as follow:" << std::endl;
    for (const auto &pair: firstMapT) {
        std::cout << pair.first << " : ";
        for (const char &ch: pair.second) {
            std::cout << ch << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

std::map<std::vector<std::string>, std::string> reverseMap() {
    std::map<std::vector<std::string>, std::string> reverseEpMap;
    for (const auto &pair: epsilonMap) {
        std::pair<std::vector<std::string>, std::string> reversePair;
        reversePair.first = pair.second;
        reversePair.second = pair.first;
        reverseEpMap.insert(reversePair);
    }
    return reverseEpMap;
}

void showReverseMap() {
    std::cout << "reverseMap show as follow:" << std::endl;
    for (const auto &pair: reverseEpsilonMap) {
        for (const auto &str: pair.first) {
            std::cout << str << "<->" << pair.second << std::endl;
        }
    }
}

void showFollowMap() {
    std::cout << "FOLLOW print as follow:" << std::endl;
    for (const auto &pair: followMap) {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
    std::cout << std::endl;
}

void getFollow() {
    noDuplicateAppend(followMap[initialKey], '$');//起始状态的follow集初始化
    while (haveNew) {
        haveNew = false;
        for (const auto &pair: epsilonMap) {
            computeFollow(pair.second);
        }
    }
}

void setInitialKey() {
    std::cout << "please enter the Initial State: " << std::endl;
    std::cin >> initialKey;
}

void getLL1List() {
    for(const auto& pair: reverseEpsilonMap){
        std::string CurKey = pair.second;
        for(auto str: pair.first){
            std::string epsilon;
            epsilon.append(CurKey);
            epsilon.append("->");
            epsilon.append(str);
            if(vt.find(str[0]) != std::string::npos){//是终结符
                LL1List[std::pair<std::string,char>(CurKey, str[0])] = epsilon;
            }
            else if(str[0] == 'e'){//是空e,把该产生式加入到follow[CurKey]
                for(auto ch: followMap[CurKey]){
                    LL1List[std::pair<std::string, char>(CurKey, ch)] = epsilon;
                }
            }
            else if(vn.find(str[0]) != std::string::npos){
                //是非终结符
                std::string lastKey(1,str[0]);
                if(str.size() > 1 && str[1] == '\''){
                    lastKey.append(1, '\'');
                }
                for(auto ch: firstMap[lastKey]){
                    LL1List[std::pair<std::string, char>(CurKey, ch)] = epsilon;
                }
                if(firstMap[lastKey].find('e') != std::string::npos){//first中含有空，把产生式加入follow[CurKey]
                    for(auto ch:followMap[CurKey]){
                        LL1List[std::pair<std::string, char>(CurKey, ch)] = epsilon;
                    }
                }
            }
        }
    }
}

void showLL1List() {
    std::cout << "LL1List print as follow: " << std::endl;
    for(const auto& pair : LL1List){
        std::cout << "<" << pair.first.first << ", " << pair.first.second << "> : " << pair.second << std::endl;
    }
}

void analysisProcess() {
    size_t count = 1;
    std::stack<std::string> stateStack;
    std::string initialStr;
    initialStr = getInput();
    std::string buffer;
    buffer = formatStr(initialStr);
    buffer.append(1, '$');
    std::cout << "format string is: " << std::endl << buffer << std::endl;
    stateStack.push(std::string(1,'$'));
    stateStack.push(initialKey);
    std::string::iterator strIt = buffer.begin();
    while(!stateStack.empty()){
        std::string stackTop = stateStack.top();
        showStack(stateStack);
        std::cout << "current char is: " << *strIt << std::endl;
        if(vt.find(stackTop) != std::string::npos || stackTop == std::string(1, '$')){
            if(stackTop == std::string(1, *strIt) ){
                std::cout << "pop stack" << std::endl << std::endl;
                stateStack.pop();
                strIt++;
            }
            else{
                size_t errorLoc = strIt - buffer.begin();
                std::cout << "error here!" << std::endl << std::endl;
                errorInf(*strIt, errorLoc, buffer, stackTop);
            }
        }
        else{
            std::string right = LL1List[std::pair<std::string, char>(stackTop, *strIt)];
            if(right.empty()){
                size_t errorLoc = strIt - buffer.begin();
                std::cout << "error here!" << std::endl << std::endl;
                errorInf(*strIt, errorLoc, buffer, stackTop);
                continue;
            }
            std::cout << "use epsilon: " << right << std::endl << std::endl;
            //获得产生式右部
            size_t rightBegin = right.find('>');
            right.erase(0, rightBegin + 1);
            //如果产生式右边空，直接弹出栈顶
            if(right[0] == 'e'){
                stateStack.pop();
                continue;
            }
            //反序产生式右部
            std::reverse(right.begin(), right.end());
            size_t swapLoc = right.find('\'');
            if(swapLoc != std::string::npos){
                std::swap(right[swapLoc], right[swapLoc + 1]);
            }
            stateStack.pop();
            for(const auto& ch: right){
                if(ch == '\''){
                    std::string top = stateStack.top();
                    stateStack.pop();
                    stateStack.push(top.append(1, ch));
                }
                else{
                    stateStack.push(std::string(1,ch));
                }
            }
        }
    }
    std::cout << "receive this str!" << std::endl;
}

bool hasLeftCursive(const std::string &left) {
    std::vector<std::string> curRight = epsilonMap[left];
    for (auto &rightStr: curRight) {
        if (rightStr[0] == left[0]) {
            return true;
        }
    }
    return false;
}

void errorInf(char ch, size_t errorLoc, const std::string &initialStr, const std::string& expectedChar) {
    std::cout << "receive an unexpected char: " << ch << std::endl;
    std::cout << "current stackTop is : " << expectedChar << std::endl;
    std::cout << "error location show as follow:" << std::endl;
    std::cout << initialStr << std::endl;
    for(int i = 0; i < errorLoc; i ++){
        std::cout << ' ';
    }
    std::cout << '^' << std::endl;
    std::cout << "analysis exit";
    exit(-1);
}

void showStack(std::stack<std::string> stack) {
    std::cout << "current stack is :";
    while(!stack.empty()){
        std::cout << stack.top();
        stack.pop();
    }
    std::cout << std::endl;
}

std::string getInput() {
    std::string buffer;
    std::cout << "please enter the str to analyse: " << std::endl;
    getchar();//读出缓冲区剩下的换行符
    std::getline(std::cin, buffer);
    return buffer;
}

std::string formatStr(const std::string& str) {
    std::string resStr;
    bool meetNum = false;
    for(auto ch: str){
        if(!isspace(ch)){
            if(!isdigit(ch)){
                if(meetNum){
                    resStr.append(1, 'n');
                    meetNum = false;
                }
                resStr.append(1, ch);
            }
            else{
                meetNum = true;
            }
        }
    }
    if(meetNum){
        resStr.append(1, 'n');
    }
    return resStr;
}
