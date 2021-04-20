#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <string.h>
using namespace std;

int** fileread(char *filename, int &rows, int &cols);

int main()
{
	int r1, c1, r2, c2;
	char filename1[] = {"Mat1.txt"};
	char filename2[] = {"Mat2.txt"};
	int** mat1 = fileread(filename1,r1,c1);//reading first matrix from Mat1.txt
	int** mat2 = fileread(filename2, r2, c2);//reading first matrix from Mat2.txt
	if (c1 != r2) {
		cout<<"Multiplication not possible "<<endl;
	}
	else {
		int res[r1][c2];
		for (int i=0;i<r1;i++) {
			for (int j=0;j<c2;j++) {
				res[i][j]=0;
			}
		}
		int pipes_and_childs = r1*(r2*c2);
		int pipes[pipes_and_childs][2];
		int read_pipes[pipes_and_childs][2];
		pid_t p[pipes_and_childs];
		for (int i=0;i<pipes_and_childs;i++) {//creating pipes
			pipe(pipes[i]);
			pipe(read_pipes[i]);
		}
		int x=0,y=0,z=0,k=0;
		for (int i=0;i<pipes_and_childs;i++) {//creating childs process
			p[i] =fork();
			if (p[i] < 0) {
				cout<<"Child failed"<<endl;
			}
			else if (p[i] > 0) { //parent process
				int mul=0;
				close(pipes[i][0]);
				write(pipes[i][1],&mat1[x][z], sizeof(mat1[x][z]));
				write(pipes[i][1],&mat2[z][y], sizeof(mat2[z][y]));
				close(pipes[i][1]);
				wait(NULL);
				close(read_pipes[i][1]); 
				read(read_pipes[i][0],&mul,sizeof(mul));
				res[x][y] = res[x][y] + mul;
				close(read_pipes[i][0]);
				if (z==c1-1) {
					y++;
					z=0;
				}
				else {
					z++;
				}
				if (y==c2) {
					x++;
					y=0;
					z=0;
				}
			}
			else { // chlid process
				int a=0,b=0;
				close(pipes[i][1]);
				read(pipes[i][0],&a, sizeof(a));
				read(pipes[i][0],&b, sizeof(b));
				int mul = a*b;
				close(pipes[i][0]); 
				close(read_pipes[i][0]); 
				write(read_pipes[i][1],&mul,sizeof(mul)); 
				close(read_pipes[i][1]); 
				exit(0);
			}
		}
		ofstream fout("resultant");//writing the output to the file resultant
		for (int i=0;i<r1;i++) {
			for (int j=0;j<c2;j++) {
				fout<<res[i][j];
				if (j<c2-1)
					fout<<",";
			}
			if (i<r1-1)
				fout<<"\n";
		}
	}
	for (int i=0;i<r1;i++) {//deallocation
		delete []mat1[i];
	}
	delete []mat1;
	for (int i=0;i<r1;i++) {
		delete []mat2[i];
	}
	delete []mat2;
	return 0; 
}

int** fileread(char* filename, int& rows, int& cols)
{
	ifstream fin(filename);
	char  buff[10][100];
	int** p;
	int count = 0, a = 0, c = 0, b = 0;
	bool flag = false;
	if (fin.is_open()) {
		while (!fin.eof()) {
			fin.getline(buff[b], 100, '\n');
			if (flag == false) {
				for (int i = 0; buff[b][i] != '\0'; i++) {
					if (buff[b][i] == ',') {
						a++;
					}
				}
				a += 1;
				flag = true;
			}
			b++;
		}
	}
	p = new int* [b];
	for (int i = 0; i < b; i++) {
		p[i] = new int[a];
	}
	for (int i = 0; i < b; i++) {
		for (int j = 0; j < a; j++) {
			p[i][j] = 0;
		}
	}
	for (int i = 0; i < b; i++) {
		int cols = 0;
		for (int j = 0; buff[i][j] != '\0'; j++) {
			if (buff[i][j] != ',') {
				p[i][cols] += (buff[i][j] - 48);
				p[i][cols] *= 10;
			}
			else if (buff[i][j] == ',') {
				p[i][cols] /= 10;
				cols++;
			}
			if (buff[i][j + 1] == '\0') {
				p[i][cols] /= 10;
			}
		}
	}
	rows = b-1;
	cols = a;
	return p;
}
