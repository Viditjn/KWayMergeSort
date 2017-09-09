#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

typedef struct node{
  long long int size;
  long long int col;
}node;

vector <int> compareOrder;
int compareLen;

unsigned int FileRead( istream & is, vector <char> & buff ) {
  is.read( &buff[0], buff.size() );
  return is.gcount();
}

unsigned long long int CountLines( const vector <char> & buff, int sz ) {
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

long long int findRecNum(long long int Mb,char* fileName){
  const long long int SZ = 1024*1024*Mb;
  vector <char> buff(SZ);
  ifstream ifs(fileName);
  long long int n = 0;
  while( int cc = FileRead( ifs, buff ) )
    n += CountLines( buff, cc );
  return n;
}


//not complete
bool breakCond(long long int Mb,long long int totalRecords,long long int Rsize){
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
  // for(int i = 0;i<vec.size();i++){
  //   for(int j=0;j<vec[0].size();j++)
  //     cout << vec[i][j] << " ";
  //   cout << endl;
  // }
  sort(vec.begin(),vec.end(),sortAsc);
  // cout << "sorting done !" << endl;
  // for(int i = 0;i<vec.size();i++){
  //   for(int j=0;j<vec[0].size();j++)
  //     cout << vec[i][j] << " ";
  //   cout << endl;
  // }
  //}
  if(myfile.is_open()){
    for(int i=0;i<vec.size();i++){
      string s = vec[i][0];
      for(int j=1;j<vec[0].size()-1;j++)
        s = s + " " + vec[i][j];
      s = s + " " + vec[i][vec[0].size()-1] + '\n';
      myfile << s;
    }
    myfile.close();
  }
}

int main(int argc, char** argv){
  node data = rowSize();
  long long int Rsize = data.size;
  long long int totalCol = data.col;
  if(Rsize==0)
    return 0;
  if(argc<5){
    cout << "Check Input Format" << endl;
    return 0;
  }
  for(int i=5;i<argc;i++)
    compareOrder.push_back(atoi((string(argv[i]).substr(1,strlen(argv[i]))).c_str()));
  compareLen = compareOrder.size();
  long long int Mb = atoi(argv[3]);
  long long int mainMem = 1024*1024*Mb;
  long long int totalRecords = findRecNum(Mb,argv[1]);
  if(breakCond(Mb,totalRecords,Rsize)){
    cout << "Too big data to sort." << endl;
    return 0;
  }
  long long int recordPerFile = mainMem/Rsize;
  long long int remainingRecords = totalRecords;
  ifstream inFile(argv[1]);
  string line;
  vector <vector <string> > vec(min(remainingRecords,recordPerFile),vector <string>(totalCol));
  long long int i=0,fileNum=0;
  cout << recordPerFile << " " << totalRecords << " " << vec[0].size()<< endl;
  while(getline(inFile,line)){
    cout << i << endl;
    if(line.empty())
      break;
    long long int j = 0;
    istringstream iss(line);
    while(iss && j<totalCol){
      iss >> vec[i][j];
      j++;
    }
    i++;
    if(i==recordPerFile){
      remainingRecords -= recordPerFile;
      cout << "wrong loop" << endl;
      writeToFile(vec,fileNum);
      i=0;
      fileNum++;
      vector <vector <string> > vec(min(remainingRecords,recordPerFile),vector <string>(totalCol));
    }
  }
  if(i!=0){
    cout << "finally" << endl;
    writeToFile(vec,fileNum);
  }
  //}
  //done till here
  //Merging Remaining
  return 0;
}
