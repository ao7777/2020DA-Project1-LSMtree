#include<fstream>
using namespace std;
int main(){
    fstream f;
    string s("./data");
    f.open(s+"/log.txt",fstream::out|fstream::binary);
    f.write("hey",3);
    f.close();
}