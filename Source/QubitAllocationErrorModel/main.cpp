#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <sys/types.h>
#include <dirent.h>
#define infinity 1e4
#define cof 0.01

using namespace std;

class HardwareA
{
protected:
    int qubitNum;

    int edgeNum;

    bool isUniDirection;

    vector<vector<bool>> archMatrix;

    vector<vector<int>> distMatrix;

    vector<vector<int>> routeMatrix;

    vector<int> outdeg;

    vector<float> crosstalk;

    vector<int> mapArray;

public:
    HardwareA(string hwname,bool isUniDirection);

    int GetQNum();

    int GetENum();

    void GetArch(string hwname);

    void PrintArchMatrix();

    void GetCrosstalk(string ctname);

    void Floyd();

    void VerifyRouteMatrix();

    void PrintRouteMatrix();

    void PrintPath(int i,int j);

    void PrintMap();

    void InitMap(vector<vector<int>> seq);

    float Alloc(vector<vector<int>> seq);

};


HardwareA::HardwareA(string hwname,bool isUniDirection=true)
{
    this->isUniDirection=isUniDirection;

    GetArch(hwname);

    PrintArchMatrix();

    GetCrosstalk(hwname+"_ct");

    Floyd();

    cout << "Physical qubits number: " << qubitNum << endl;
    cout << "Edge number: " << edgeNum << endl;
}


void HardwareA::GetArch(string hwname)
{
    int adjIndex,i;
    ifstream is(hwname,ios::in);

    if(!is)
    {
        cout << "Cannot Open Hardware File." << endl;
        exit(1);
    }

    qubitNum=0;
    edgeNum=0;

    while(!is.eof())
    {
        is>>adjIndex;
        if(adjIndex == -1)
            qubitNum++;
    }
    qubitNum--;

    for(i=0; i<qubitNum; i++)
    {
        archMatrix.push_back(vector<bool>(qubitNum,false));
        distMatrix.push_back(vector<int>(qubitNum));
        routeMatrix.push_back(vector<int>(qubitNum));
        outdeg.push_back(0);
    }

    mapArray.resize(qubitNum);

    i=0;
    is.clear();
    is.seekg(0,ios::beg);

    while(i<qubitNum && !is.eof())
    {
        is>>adjIndex;
        if(adjIndex==-1)
        {
            archMatrix[i][i]=true;
            i++;
        }

        else
        {
            archMatrix[i][adjIndex]=true;
            outdeg[i]++;
            edgeNum++;
        }
    }

    edgeNum=edgeNum/2;

    is.close();
}


void HardwareA::PrintArchMatrix()
{
    cout << "Architecture Matrix:" << endl;
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
        {
            cout << archMatrix[i][j] << " ";
            if(j==qubitNum-1)
                cout << endl;
        }
}


void HardwareA::GetCrosstalk(string ctname)
{
    float ct;
    ifstream is(ctname,ios::in);

    if(!is)
    {
        cout << "Cannot Open Crosstalk File." << endl;
        exit(1);
    }

    for(int i=0; i<qubitNum; i++)
    {
        is >> ct;
        crosstalk.push_back(cof*ct);
    }

    is.close();

    cout << "Crosstalk:" << endl;
    for(int j=0; j<qubitNum; j++)
        cout << crosstalk[j] << " ";
    cout << endl;
}


void HardwareA::Floyd()
{
    int i,j,k;
    for(i=0; i<qubitNum; i++)
        for(j=0; j<qubitNum; j++)
        {
            if(!archMatrix[i][j] && !archMatrix[j][i])
            {
                distMatrix[i][j]=infinity;
                routeMatrix[i][j]=-1;
            }

            else
            {
                distMatrix[i][j]=1;
                routeMatrix[i][j]=j;
            }
        }

    for(k=0; k<qubitNum; k++)
        for(i=0; i<qubitNum; i++)
            for(j=0; j<qubitNum; j++)
                if(distMatrix[i][j]>distMatrix[i][k]+distMatrix[k][j])
                {
                    distMatrix[i][j]=distMatrix[i][k]+distMatrix[k][j];
                    routeMatrix[i][j]=routeMatrix[i][k];
                }

    VerifyRouteMatrix();

    PrintRouteMatrix();
}



void HardwareA::VerifyRouteMatrix()
{
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
            if(routeMatrix[i][j]==-1)
            {
                cout << "Not fully connected architecture." << endl;
                exit(1);
            }
}


void HardwareA::PrintRouteMatrix()
{
    cout << "Route Matrix:" << endl;
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
        {
            cout << routeMatrix[i][j] << " ";
            if(j==qubitNum-1)
                cout << endl;
        }
}


void HardwareA::PrintPath(int i,int j)
{
    int next=routeMatrix[i][j];
    if(next==-1)
        cout << "No Path between " << i << " and "<< j << endl;
    else
    {
        cout << "Path from " << i << " to " << j << ": " << i << " ";
        while(next!=j)
        {
            cout << next << " ";
            next=routeMatrix[next][j];
        }
        cout << j << endl;
    }
}

int HardwareA::GetQNum()
{
    return qubitNum;
}


int HardwareA::GetENum()
{
    return edgeNum;
}


void HardwareA::InitMap(vector<vector<int>> seq)
{
    int i;
    unsigned int j;
    vector<int> freq(qubitNum,0);
    vector<int> sortFreq(1,0);
    vector<int> sortOutDeg(1,0);

    for(j=0; j<seq.size(); j++)
        if(seq[j][0]!=-1)
            freq[seq[j][0]]++;

    for(i=1; i<qubitNum; i++)
        for(j=0; j<sortFreq.size(); j++)
        {
            if(freq[i]>freq[sortFreq[j]])
            {
                sortFreq.insert(sortFreq.begin()+j,i);
                break;
            }

            if(j==sortFreq.size()-1)
            {
                sortFreq.push_back(i);
                break;
            }
        }

    for(i=1; i<qubitNum; i++)
        for(j=0; j<sortOutDeg.size(); j++)
        {
            if(outdeg[i]>outdeg[sortOutDeg[j]])
            {
                sortOutDeg.insert(sortOutDeg.begin()+j,i);
                break;
            }

            if(j==sortOutDeg.size()-1)
            {
                sortOutDeg.push_back(i);
                break;
            }
        }

    for(i=0; i<qubitNum; i++)
        mapArray[sortOutDeg[i]]=sortFreq[i];

    /*
    cout << "Initial Mapping:" << endl;
    PrintMap();
    */
}


void HardwareA::PrintMap()
{
    int i;
    cout << "Physical qubits: ";
    for(i=0; i<qubitNum; i++)
        cout << i << " ";
    cout << endl;
    cout << "Pseudo   qubits: ";
    for(i=0; i<qubitNum; i++)
        cout << mapArray[i] << " ";
    cout << endl;
}


float HardwareA::Alloc(vector<vector<int>> seq)
{
    unsigned int i;
    int j,temp,current,next,dest;
    float cost=0;

    for(i=0; i<seq.size(); i++)
    {
        if(seq[i][0]==-1)
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][1])
                {
                    cost=cost+crosstalk[j];
                    break;
                }
            }

        else
        {
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][1])
                    current=j;

                if(mapArray[j]==seq[i][0])
                    dest=j;
            }

            next=routeMatrix[current][dest];

            while(next!=dest)
            {
                temp=mapArray[current];
                mapArray[current]=mapArray[next];
                mapArray[next]=temp;

                cost=cost+7;
                current=next;
                next=routeMatrix[current][dest];
            }

            if(archMatrix[current][next])
                cost++;
            else
                cost=cost+5;

        }
    }

    return cost;
}

class HardwareB:public HardwareA
{
protected:
    vector<int> sgateNum;

public:
    HardwareB(string hwname,bool isUniDirection);

    float Alloc(vector<vector<int>> seq);
};

HardwareB::HardwareB(string hwname,bool isUniDirection=true):HardwareA(hwname,isUniDirection)
{
    for(int i=0; i<qubitNum; i++)
        sgateNum.push_back(0);
}

float HardwareB::Alloc(vector<vector<int>> seq)
{
    unsigned int i;
    float minsgc,cost=0;
    int j,temp, beg,current,next,dest;

    for(i=0; i<seq.size(); i++)
    {
        if(seq[i][0]==-1)
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][1])
                {
                    sgateNum[j]++;
                    break;
                }
            }

        else
        {
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][1])
                    beg=j;

                if(mapArray[j]==seq[i][0])
                    dest=j;
            }

            if(crosstalk[dest]>crosstalk[beg])
            {
                temp=beg;
                beg=dest;
                dest=temp;
            }

            cost=cost+crosstalk[dest]*sgateNum[dest];
            sgateNum[dest]=0;

            minsgc=crosstalk[beg];

            next=routeMatrix[beg][dest];

            current=beg;

            while(next!=dest)
            {
                temp=mapArray[current];
                mapArray[current]=mapArray[next];
                mapArray[next]=temp;

                cost=cost+7;
                current=next;
                next=routeMatrix[current][dest];

                if(crosstalk[current]<minsgc)
                    minsgc=crosstalk[current];
            }

            if(archMatrix[current][next])
                cost++;
            else
                cost=cost+5;

            cost=cost+minsgc*sgateNum[beg];
            sgateNum[beg]=0;
        }
    }

    for(j=0;j<qubitNum;j++)
        if(sgateNum[j]!=0)
        {
            cost=cost+crosstalk[j]*sgateNum[j];
            sgateNum[j]=0;
        }

    return cost;
}


class HardwareC:public HardwareA
{
public:
    HardwareC(string hwname,bool isUniDirection);

    float Alloc(vector<vector<int>> seq);
};

HardwareC::HardwareC(string hwname,bool isUniDirection=true):HardwareA(hwname,isUniDirection){}

float HardwareC::Alloc(vector<vector<int>> seq)
{
    unsigned int i;
    float cost=0;
    int j,temp,current,next,dest;

    for(i=0; i<seq.size(); i++)
    {
        if(seq[i][0]==-1)
        {
            for(j=0; j<qubitNum; j++)
                if(mapArray[j]==seq[i][1])
                {
                    cost=cost+crosstalk[j];
                    break;
                }
        }

        else
        {
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][1])
                    current=j;

                if(mapArray[j]==seq[i][0])
                    dest=j;
            }

            next=routeMatrix[current][dest];

            if(next==dest)
            {
                if(archMatrix[current][next])
                    cost++;
                else
                    cost=cost+5;
            }

            else
            {
                while(routeMatrix[next][dest]!=dest)
                {
                    temp=mapArray[current];
                    mapArray[current]=mapArray[next];
                    mapArray[next]=temp;

                    cost=cost+7;
                    current=next;
                    next=routeMatrix[current][dest];
                }

                if(!archMatrix[current][next] && !archMatrix[next][dest])
                    cost=cost+4;
                else if((archMatrix[current][next] && archMatrix[next][dest]) || (!archMatrix[current][next] && archMatrix[next][dest]))
                    cost=cost+10;
                else
                {
                    temp=mapArray[current];
                    mapArray[current]=mapArray[next];
                    mapArray[next]=temp;
                    cost=cost+8;
                }
            }

        }
    }

    return cost;
}

class HardwareD:public HardwareA
{
protected:
    vector<int> sgateNum;

public:
    HardwareD(string hwname,bool isUniDirection);

    float Alloc(vector<vector<int>> seq);
};

HardwareD::HardwareD(string hwname,bool isUniDirection=true):HardwareA(hwname,isUniDirection)
{
    for(int i=0; i<qubitNum; i++)
        sgateNum.push_back(0);
}

float HardwareD::Alloc(vector<vector<int>> seq)
{
    unsigned int i;
    float minsgc,cost=0;
    int j,temp, beg,current,next,dest;

    for(i=0; i<seq.size(); i++)
    {
        if(seq[i][0]==-1)
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][1])
                {
                    sgateNum[j]++;
                    break;
                }
            }

        else
        {
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][1])
                    beg=j;

                if(mapArray[j]==seq[i][0])
                    dest=j;
            }


            if(crosstalk[dest]>crosstalk[beg])
            {
                temp=beg;
                beg=dest;
                dest=temp;
            }


            cost=cost+crosstalk[dest]*sgateNum[dest];
            sgateNum[dest]=0;

            minsgc=crosstalk[beg];

            next=routeMatrix[beg][dest];

            if(next==dest)
            {
                if(archMatrix[beg][next])
                    cost++;
                else
                    cost=cost+5;
            }

            else
            {
                current=beg;

                while(routeMatrix[next][dest]!=dest)
                {
                    temp=mapArray[current];
                    mapArray[current]=mapArray[next];
                    mapArray[next]=temp;

                    cost=cost+7;
                    current=next;
                    next=routeMatrix[current][dest];

                    if(crosstalk[current]<minsgc)
                        minsgc=crosstalk[current];
                }

                if(!archMatrix[current][next] && !archMatrix[next][dest])
                    cost=cost+4;
                else if((archMatrix[current][next] && archMatrix[next][dest]) || (!archMatrix[current][next] && archMatrix[next][dest]))
                    cost=cost+10;
                else
                {
                    temp=mapArray[current];
                    mapArray[current]=mapArray[next];
                    mapArray[next]=temp;
                    cost=cost+8;
                }
            }

            cost=cost+minsgc*sgateNum[beg];
            sgateNum[beg]=0;
        }
    }

    for(j=0;j<qubitNum;j++)
        if(sgateNum[j]!=0)
        {
            cost=cost+crosstalk[j]*sgateNum[j];
            sgateNum[j]=0;
        }

    return cost;
}


void RandSeqGen(vector<vector<int>> &seq,int qubitNum,int seqLen);

void GetSeq(vector<vector<int>> &seq,string fname);

void PrintSeq(vector<vector<int>> seq);

int GetSeqList(vector<string> &fileList, string directory);

int main()
{
    float costA,costB;
    int fcount;
    HardwareC archA("ibmqx5");
    HardwareD archB("ibmqx5");

    vector<string> fileList;
    vector<vector<int>> seq;
/*
    RandSeqGen(seq,archA.GetQNum(),1000);
    //GetSeq(seq,"seq/seq_3_17_13.qasm");
    archA.InitMap(seq);
    costA=archA.Alloc(seq);

    archB.InitMap(seq);
    costB=archB.Alloc(seq);

    cout << "The total cost of HardwareA is: " << costA << endl;
    cout << "The total cost of HardwareB is: " << costB << endl;
    cout << "costB / costA = " << costB/costA << endl;
*/
    string directory="/home/tilmto/CodeBlocks/QubitAllocationErrorModel/seq";
    fcount=GetSeqList(fileList,directory);

    ofstream os("/home/tilmto/Ericpy/QuantumComputing/errormodel/result",ios::out);

    for(int i=0; i<fcount; i++)
    {
        GetSeq(seq,"seq/"+fileList[i]);

        archA.InitMap(seq);
        costA=archA.Alloc(seq);

        archB.InitMap(seq);
        costB=archB.Alloc(seq);

        os << fileList[i] << ":" << endl;
        os << "The total cost of HardwareA is: " << costA << endl;
        os << "The total cost of HardwareB is: " << costB << endl;
        os << "costB / costA = " << costB/costA << endl;
        os << endl;
    }

    os.close();

    return 0;
}


void RandSeqGen(vector<vector<int>> &seq,int qubitNum,int seqLen)
{
    int cqubit,squbit;
    int i=0;
    srand((int)time(0));

    while(i<seqLen)
    {
        if(rand()%2==0)
            cqubit=rand()%qubitNum;
        else
            cqubit=-1;

        squbit=rand()%qubitNum;

        if(cqubit!=squbit)
        {
            seq.push_back(vector<int>(2));
            seq[i][0]=cqubit;
            seq[i][1]=squbit;
            i++;
        }
    }
}


void GetSeq(vector<vector<int>> &seq,string fname)
{
    int i=0;

    seq.clear();

    ifstream is(fname,ios::in);

    while(!is.eof())
    {
        seq.push_back(vector<int>(2));
        is >> seq[i][0];
        is >> seq[i][1];
        i++;
    }
    seq.pop_back();

    is.close();
}


void PrintSeq(vector<vector<int>> seq)
{
    cout << "Dependency Sequence:"<<endl;

    for(unsigned int i=0; i<seq.size(); i++)
        cout << "( " << seq[i][0] << " , " << seq[i][1] << " )" <<endl;
}


int GetSeqList(vector<string> &fileList, string directory)
{
    directory = directory.append("/");

    DIR *p_dir;
    const char* str = directory.c_str();

    p_dir = opendir(str);
    if( p_dir == NULL)
    {
        cout<< "can't open :" << directory << endl;
    }

    struct dirent *p_dirent;

    while ( p_dirent = readdir(p_dir))
    {
        string tmpFileName = p_dirent->d_name;
        if( tmpFileName == "." || tmpFileName == "..")
        {
            continue;
        }
        else
        {
            fileList.push_back(tmpFileName);
        }
    }
    closedir(p_dir);

    return fileList.size();
}






