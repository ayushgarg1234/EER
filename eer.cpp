#include <iostream>
#include <fstream>
#include <cmath>
#include<cstdlib>
#include<sstream>
#include<cstring>
//#include <vector>

using namespace std;

/*
class EER_Data {
public:
	bool G_I;
	float score;
};*/

class histogram {
public:
	int G_cum;
	int I_cum;
	float diff;
	histogram();
};

histogram::histogram()
{
	G_cum = 0;
	I_cum = 0;
}

int main(int argc,char * argv[])
{
	ifstream myfile;
	myfile.open(argv[1]);
    //ifstream fin("EER_input.txt", ios::in);
    int no_of_buckets = 1000000;
    
    histogram *array;
	array = new histogram[no_of_buckets + 1];

    int temp1,temp2,temp3,temp4,G_I;
    float score;
    int index;
    while(!myfile.eof())
    {
      	myfile >> temp1 >> temp2 >> temp3 >> temp4;
    	myfile >> G_I >> score;
        index = score * 1000000;
	if (G_I)
	  array[index-1].G_cum++;
	else
	  array[index].I_cum++;	
        //cout << index << " " << array[index].G_cum << "\n";
    }
    if(G_I)
      array[index-1].G_cum--;
    else
      array[index].I_cum--;
    
    ofstream test_G_Hist;
    test_G_Hist.open("test_G_Hist.dat");

    ofstream test_I_Hist;
    test_I_Hist.open("test_I_Hist.dat");
    

	myfile.close();
	for (int i = 0; i <= no_of_buckets; i++)
	{
		if (array[i].G_cum != 0)
			test_G_Hist << (float(i) / 1000000) << " " << array[i].G_cum << "\n";
		if (array[i].I_cum != 0)
			test_I_Hist << (float(i) / 1000000) << " " << array[i].I_cum << "\n";
	}

	test_G_Hist.close();
	test_I_Hist.close();
	
	for (int i = 1; i <= no_of_buckets; i++)
	{
		array[i].I_cum = array[i - 1].I_cum + array[i].I_cum;
		array[no_of_buckets - i].G_cum = array[no_of_buckets - i + 1].G_cum + array[no_of_buckets - i].G_cum;
	}
	//cout << array[0].G_cum << " " << array[no_of_buckets].I_cum<<"\n";
 	
    stringstream temp_1, temp_2;
	temp_1 << (array[0].G_cum);
	temp_2 << (array[no_of_buckets].I_cum);
   
   	int total_G = array[0].G_cum, total_I = array[no_of_buckets].I_cum;
	/*
	for (int i = 0; i <= no_of_buckets; i++)
	{
	  array[i].G_cum = array[i].G_cum / total_G;
	  array[i].I_cum = array[i].I_cum / total_I;
	}
	*/
	ofstream FARvsFRR;
	FARvsFRR.open("FAR_FRR.dat");

	for (int i = 0; i <= no_of_buckets; i++)
	{
	  //FARvsFRR << i << " " << array[i].G_cum*total_G << " " << array[i].G_cum << "\t" << i << " " << array[i].I_cum*total_I << " " << array[i].I_cum << "\n";
	 FARvsFRR << i << " " << total_G-array[i].G_cum << " " << array[i].G_cum*1.0/total_G << "\t" << i << " " << array[i].I_cum << " " << array[i].I_cum*1.0/total_I << "\n";
	}

	FARvsFRR.close();

	char command[60] = "sh graph_change.sh ";
	strcat(command, temp_1.str().c_str());
	strcat(command, " ");
	strcat(command, temp_2.str().c_str());
	system(command);
	strcpy(command, "sh graph_generate.sh");
	system(command);    

    strcpy(command, "sh graph_generate1.sh");
	system(command);
	
    for (int i = 0; i <= no_of_buckets; i++)
	{
		array[i].diff = abs(array[i].G_cum*1.0/total_G - array[i].I_cum*1.0/total_I);
	}

	float min_diff = array[0].diff;
	int min_index = 0, min_count = 0;


	for (int i = 1; i <= no_of_buckets; i++)
	{
		if (array[i].diff < min_diff)
		{
			min_diff = array[i].diff;
			min_index = i;
			min_count = 1;
		}
		else if (array[i].diff == min_diff)
		{
			min_count++;
		}
		else
			break;
	}

	//cout << min_diff << " " << min_count << " " << min_index << "\n";
	float threshold = ((float(min_index) + float(min_count) / 2) / 1000000);
    ofstream eer;
    eer.open("eer.txt");
    //eer << "  (" << array[min_index].G_cum << ")  " << threshold << " with Difference = " <<array[min_index].diff; 
    eer << array[min_index].G_cum*1.0/total_G << " " << threshold << " " << "0 "; 

	cout << "Threshold = " << threshold << "\n";
    ofstream fa,fr;
    fa.open("test.FA");
    fa << "Imposters Passed (FA)\n";
    fr.open("test.FR");
    fr << "Genuine Passed (FR)\n";
    myfile.open(argv[1]);
    //float score1;
    int fa_cnt=0,fr_cnt=0;
    while(!myfile.eof())
    {
   	    myfile >> temp1 >> temp2 >> temp3 >> temp4;
       	myfile >> G_I >> score;
	    if(score<=threshold && !G_I)
        {
            fa << temp1 <<  " " << temp2 << " " << temp3 <<  " " << temp4 <<  " " << 0 <<  " " << score << "\n";
            fa_cnt++;
        }
        else if(score>threshold && G_I)
        {
            fr << temp1 <<  " " << temp2 <<  " " << temp3 <<  " " << temp4 <<  " " << 1 <<  " " << score << "\n";
            fr_cnt++;
        }
    }
    fa.close();fr.close();
    eer << fr_cnt << " " << total_G << " " << fa_cnt << " " << total_I << " " << array[min_index].I_cum*1.0/total_I<< " " << array[min_index].G_cum*1.0/total_G;
    eer.close();
    //cin >> n;
	return 0;
}
