#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

typedef struct node{
  int size;
  int col;
}node;

vector <int> compareOrder;
int compareLen;

unsigned int FileRead( istream & is, vector <char> & buff ) {
  is.read( &buff[0], buff.size() );
  return is.gcount();
}

unsigned int CountLines( const vector <char> & buff, int sz ) {
  int newlines = 0;
  const char * p = &buff[0];
  for ( int i = 0; i < sz; i++ )
    if ( p[i] == '\n' )
      newlines++;
  return newlines;
}

node rowSize(){
  ifstream metaFile;
  metaFile.open("metadata.txt");
  node data;
  if(!metaFile){
    cout << "Error : METADATA File Missing or empty";
    data.size = data.col =0;
    return data;
  }
  string s;
  int Rsize = 0,colNum=0;
  while(metaFile >> s){
    char * si = strdup(s.c_str());
    const char * delim = ",";
    char * token = strtok(si,delim);
    token = strtok(NULL,delim);
    Rsize += atoi(token);
    free(si);
    colNum++;
  }
  data.size = Rsize;
  data.col = colNum;
  return data;
}

int findRecNum(int Mb,char* fileName){
  const int SZ = 1024*1024*Mb;
  vector <char> buff(SZ);
  ifstream ifs(fileName);
  int n = 0;
  while( int cc = FileRead( ifs, buff ) )
    n += CountLines( buff, cc );
  return n;
}


//not complete
bool breakCond(int Mb,int totalRecords,int Rsize){
  int size = 1024*1024*Mb;
  return false;
  //return (size/Rsize)*()
}

bool sortAsc(const vector<string> a,const vector<string> b){
  int i;
  for(i=0;i<compareLen;i++){
    if(a[compareOrder[i]]==b[compareOrder[i]])
      i++;
    else
      return (a[compareOrder[i]] < b[compareOrder[i]]);
  }
  return true;
}

void writeToFile(vector <vector <string> > vec,int fileNum){
  string fileName;
  stringstream convert;
  convert << fileNum;
  fileName = convert.str() + ".txt";
  cout << fileName << endl;
  ofstream myfile(fileName.c_str());
  sort(vec.begin(),vec.end(),sortAsc);
  if(myfile.is_open()){
    for(int i=0;i<vec.size();i++){
      string s = vec[i][0];
      for(int j=1;j<vec[0].size()-1;j++)
        s = s + ' ' + vec[i][j];
      s = s + ' ' + vec[i][vec[0].size()] + '\n';
      myfile << s;
    }
    myfile.close();
  }
}

int main(int argc, char** argv){
  node data = rowSize();
  int Rsize = data.size;
  int totalCol = data.col;
  if(Rsize==0)
    return 0;
  if(argc<5){
    cout << "Check Input Format" << endl;
    return 0;
  }
  for(int i=5;i<argc;i++)
    compareOrder.push_back(atoi((string(argv[i]).substr(1,strlen(argv[i]))).c_str()));
  compareLen = compareOrder.size();
  int Mb = atoi(argv[3]);
  int mainMem = 1024*1024*Mb;
  int totalRecords = findRecNum(Mb,argv[1]);
  if(breakCond(Mb,totalRecords,Rsize)){
    cout << "Too big data to sort." << endl;
    return 0;
  }
  int recordPerFile = mainMem/Rsize;
  cout << recordPerFile << endl;
  ifstream inFile(argv[1]);
  string line;
  vector <vector <string> > vec(recordPerFile,vector <string>(1000));
  int i=0,fileNum=0;
  while(getline(inFile,line)){
    istringstream iss(line);
    string s;
    int j = 0;
    while(getline(iss,s,' ')){
      vec[i][j] = s.c_str();
      j++;
    }
    i++;
    if(i==recordPerFile){
      writeToFile(vec,fileNum);
      i=0;
      fileNum++;
      vector <vector <string> > vec(recordPerFile,vector <string>(totalCol));
    }
  }
  if(i!=0){
    writeToFile(vec,fileNum);
    vector <vector <string> > vec(recordPerFile,vector <string>(totalCol));
  }
  //done till here
  //Merging Remaining
  return 0;
}
