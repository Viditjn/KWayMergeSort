#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

//debug data

vector <long long int > countPerFile;

vector <int > colData;

typedef struct node{
  long long int size;
  long long int col;
}node;

struct record{
  vector <string> data;
  int fiNum;
};

vector <int> compareOrder;
int compareLen;
int sortOrder;

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
    colData.push_back(atoi(token));
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

long long int BlockSize;
//not complete
bool breakCond(long long int Mb,long long int totalRecords,long long int Rsize){
  long long int size = 1024*1024*Mb;
  long long int rperPage = size/Rsize;
  long long int totalPage = ceil(totalRecords/rperPage);
  if(size/((totalPage+1)*Rsize) < 1)
    return true;
  BlockSize = size/((totalPage+1)*Rsize);
  return false;
  //return (size/Rsize)*()
}

bool sortAsc(const vector<string> a,const vector<string> b){
  int i;
  for(i=0;i<compareLen;i++){
    if(a[compareOrder[i]]==b[compareOrder[i]])
      continue;
    else{
      if(sortOrder == 0)
        return (a[compareOrder[i]] < b[compareOrder[i]]);
      else
        return (a[compareOrder[i]] > b[compareOrder[i]]);
    }
  }
  return true;
}

class mycomp {
public:
int operator() (const record& a,const record& b) const {
  int i;
  for(i=0;i<compareLen;i++){
    if(a.data[compareOrder[i]]==b.data[compareOrder[i]])
      continue;
    else{
      if(sortOrder == 1)
        return (a.data[compareOrder[i]] < b.data[compareOrder[i]]);
      else
        return (a.data[compareOrder[i]] > b.data[compareOrder[i]]);
    }
  }
  return 1;
}
};

void writeToFile(vector <vector <string> > vec,int fileNum,long long int sizeL){
  string fileName = to_string(fileNum) + ".txt";
  //cout << fileName << endl;
  ofstream myfile(fileName.c_str());
  sort(vec.begin(),vec.begin()+sizeL,sortAsc);
  if(myfile.is_open()){
    for(int i=0;i<sizeL;i++){
      string s = vec[i][0];
      for(int j=1;j<vec[0].size()-1;j++)
        s = s + "  " + vec[i][j];
      s = s + "  " + vec[i][vec[0].size()-1] + '\r'+'\n';
      myfile << s;
    }
    myfile.close();
  }
}
long long int totalLines = 0;
void writeData(ofstream& out, vector <vector <string> > outBuf){
  //cout << "writing data iter" << endl;
  totalLines = 0;
  if(out.is_open()){
    for(long long int i=0;i<outBuf.size();i++){
      string s = outBuf[i][0];
      totalLines++;
      // if(s.empty()==true)
      //   break;
      for(long long int j=1;j<outBuf[0].size()-1;j++)
        s = s + "  " + outBuf[i][j];
      s = s + "  " + outBuf[i][outBuf[0].size()-1] + '\r'+'\n';
      out << s;
    }
  }
  //cout << totalLines << endl;
  return;
}

int iterNum = 0;

void merge(string outFile,int totalFiles,long long int blockSize,long long int totalCol){
  vector <vector <record> > vecR(totalFiles);
  vector < ifstream* > filePointers;
  vector <int> vecCount(totalFiles);
  vector <long long int> vTotal(totalFiles);
  vector <long long int> vRotal(totalFiles);
  for(int i=0;i<totalFiles;i++)
    vecCount[i] = vTotal[i] = vRotal[i] = 0;
  //vector <vector <string> > outBuffer(blockSize,vector <string>(totalCol));
  vector <vector <string> > outBuffer;
  for(int i = 0;i<totalFiles;i++){
    ifstream *ftmp = new ifstream((to_string(i)+ ".txt").c_str());
    filePointers.push_back(ftmp);
  }
  long long int index;
  for(int i=0;i<totalFiles;i++){
    long long int co = 0;
    vector <record> ans;
    string line;
    while(co<blockSize && getline(*filePointers[i],line)){
      //vTotal[i]++;
      countPerFile[i]--;
      vector <string> tempStr;
      index = 0;
      for(int q=0;q<colData.size();q++){
        tempStr.push_back(line.substr(index,colData[q]));
        index += colData[q] + 2;
      }
      record temp;
      temp.data = tempStr;
      temp.fiNum = i;
      ans.push_back(temp);
      co++;
    }
    vecR[i] = ans;
    ans.clear();
    iterNum++;
  }

  ofstream outPointer(outFile.c_str());
  long long int totalCo = 0;
  long long int totalPush = 0;
  priority_queue <record, vector <record>, mycomp > pq;
  for(int i=0;i<totalFiles;i++){
    if(vecR[i].size()>0){
      pq.push(vecR[i][0]);
      vTotal[i]++;
      totalPush++;
    }
  }
  record tmp = pq.top();
  long long int i = 0;
  while(pq.empty()==false){
    if(i == blockSize){
      writeData(outPointer,outBuffer);
      i = 0;
      outBuffer.clear();
    }
    record tmp = pq.top();
    pq.pop();
    totalCo++;
    outBuffer.push_back(tmp.data);
    vRotal[tmp.fiNum]++;
    i++;
    vecCount[tmp.fiNum]++;
    if(vecCount[tmp.fiNum]<vecR[tmp.fiNum].size()){
      vTotal[tmp.fiNum]++;
      pq.push(vecR[tmp.fiNum][vecCount[tmp.fiNum]]);
      totalPush++;
    }
    else{
      long long int co = 0;
      vector <record> ans;
      string line;
      while(co<blockSize && getline(*filePointers[tmp.fiNum],line)){
        countPerFile[tmp.fiNum]--;
        vector <string> tempStr;
        index = 0;
        for(int q=0;q<colData.size();q++){
          tempStr.push_back(line.substr(index,colData[q]));
          index += colData[q] + 2;
        }
        record temp;
        temp.data = tempStr;
        temp.fiNum = tmp.fiNum;
        ans.push_back(temp);
        co++;
      }
      iterNum++;
      vecR[tmp.fiNum] = ans;
      vecCount[tmp.fiNum] = 0;
      if(vecR[tmp.fiNum].size()>0){
        pq.push(vecR[tmp.fiNum][0]);
        totalPush++;
      }
    }
  }
  writeData(outPointer,outBuffer);
  outPointer.close();
  return;
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
  if(string(argv[4])=="asc")
    sortOrder = 0;
  else if(string(argv[4])=="desc")
    sortOrder = 1;
  else{
    cout << "Sort Not defined Correctly" << endl;
    return 0;
  }
  for(int i=5;i<argc;i++)
    compareOrder.push_back(atoi((string(argv[i]).substr(1,strlen(argv[i]))).c_str()));
  compareLen = compareOrder.size();
  for(int i=0;i<compareLen;i++){
    if(compareOrder[i]>totalCol){
      cout << "Wrong Column Entry for sorting" << endl;
      return 0;
    }
  }
  long long int Mb = atoi(argv[3]);
  long long int mainMem = 1024*1024*Mb;
  long long int totalRecords = findRecNum(Mb,argv[1]);
  if(breakCond(Mb,totalRecords,Rsize)){
    cout << "Too big data to sort." << endl;
    return 0;
  }
  long long int recordPerFile = mainMem/Rsize;
  long long int remainingRecords = totalRecords;
  ifstream inFile;
  inFile.open(argv[1]);
  if(!inFile.is_open()){
    cout << "Error : Check Filename " << endl;
    return 0;
  }

  string line;
  vector <vector <string> > vec(recordPerFile,vector <string>(totalCol));
  long long int i=0,fileNum=0,totalL=0;
  int index = 0;
  while(getline(inFile,line)){
    totalL++;
    vector <string> tempStr;
    index = 0;
    for(int q=0;q<colData.size();q++){
      vec[i][q] = line.substr(index,colData[q]);
      index += colData[q] + 2;
    }
    i++;
    if(i==recordPerFile){
      writeToFile(vec,fileNum,min(remainingRecords,recordPerFile));
      countPerFile.push_back(min(remainingRecords,recordPerFile));
      remainingRecords -= recordPerFile;
      i=0;
      fileNum++;
      vector <vector <string> > vec(recordPerFile,vector <string>(totalCol));
    }
  }
  if(i!=0){
    writeToFile(vec,fileNum,min(remainingRecords,recordPerFile));
    countPerFile.push_back(min(remainingRecords,recordPerFile));
  }
  merge(argv[2],fileNum+1,BlockSize,totalCol);
  for(int i=0;i<fileNum+1;i++)
    if (remove((to_string(i)+ ".txt").c_str()) !=0)
      cout<<"Remove operation failed"<<endl;
  return 0;
}
