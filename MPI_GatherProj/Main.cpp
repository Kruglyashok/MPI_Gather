#include "mpi.h"
#include "stdio.h" 
#include "stdlib.h"
#include <ctime> 
#include <iostream>


using namespace std;


int  MPI_Gather(void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm) {
	
	int  tsize, trank;
	
	MPI_Comm_size(MPI_COMM_WORLD, &tsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &trank);
	
	if (root < 0 || root >= tsize
		|| scount != rcount || stype != rtype) {
		return 0;
	}
	MPI_Aint sendExtent; // extent of MPI_ types of data 
	MPI_Aint recvExtent; // extent of MPI_ types of data 

	MPI_Type_extent(stype, &sendExtent); //getting extent
	MPI_Type_extent(rtype, &recvExtent); //getting extent

	
	//filling the rbuf of root proc with data
	for (int i = 0; i < scount*sendExtent; i++) {
		*((char *)(rbuf)+root*recvExtent*scount + i) = *((char *)(sbuf)  + i);
	}

	//filling the rest of rbuf with data except root proc
	if (trank != root) {
		for (int i = 0; i < tsize; i++) {
			if (i == root) {
				continue;
			}
			else {
				MPI_Send(sbuf, scount, stype, root, 0, MPI_COMM_WORLD);
			}
		}
	}
	else {
			for (int i = 0; i < tsize; i++) { 
				if (i == root) continue;
		MPI_Recv(((char *)rbuf + i*scount*recvExtent), scount, stype, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		}
	}
	return 1;
}



int main(int argc, char * argv[]) {
	
	int size, rank;
	
	int *sbuf, *rbuf; // testing MPI_Gather with ints
	double *dsbuf, *drbuf; // testing MPI_Gather wint doubles
	float *fsbuf, *frbuf; //testing MPI_Gather wint floats
	
	int rcount, scount;
	int root;

	MPI_Init(&argc, &argv);
	MPI_Comm_size( MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    root = atoi(argv[1]);
	
	if (root < 0 || root >= size) {
		return 0;
	}


	rcount = 2; 
	scount = 2;

	//ints
	rbuf = new int[rcount*size];
	sbuf = new int[scount];
	for (int i = 0; i < scount; i++) sbuf[i] = rank;
	
	//doubles
	drbuf = new double[rcount*size];
	dsbuf = new double[scount];
	for (int i = 0; i < scount; i++) dsbuf[i] = rank;
	
	//floats
	frbuf = new float[rcount*size];
	fsbuf = new float[scount];
	for (int i = 0; i < scount; i++) fsbuf[i] = rank;


	for (int i = 0; i < rcount * size; i++) rbuf[i] =  -1;

	for (int i = 0; i < rcount * size; i++) drbuf[i] = -1;	

	for (int i = 0; i < rcount * size; i++) frbuf[i] = -1;
	


	if (rank == root) {
		cout << "INT Array Before:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;

		cout << "DOUBLE Array Before:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;
		cout << "FLOAT Array Before:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	//MPI_Gather(dsbuf, scount, MPI_DOUBLE, drbuf, rcount, MPI_DOUBLE, root, MPI_COMM_WORLD);
	
    MPI_Gather(sbuf, scount, MPI_INT, rbuf, rcount, MPI_INT, root, MPI_COMM_WORLD);

	if (rank == root) {

		cout << "Final INT Array:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;
	}

	
	if (rank == root) {
		cout << "Final DOUBLE Array:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << drbuf[i] << "\t";
		}
		cout << endl;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(fsbuf, scount, MPI_FLOAT, frbuf, rcount, MPI_FLOAT, root, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == root) {
		cout << "Final FLOAT Array:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << frbuf[i] << "\t";
		}
	}
	MPI_Finalize();
	return 0;
}