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
	//MPI_Aint sendExtent; // extent of MPI_ types of data 
	//MPI_Aint recvExtent; // extent of MPI_ types of data 
	//
	//MPI_Aint s;
	int sendExtent;
	int recvExtent;
	//MPI_Type_extent(stype, &sendExtent); //getting extent
	//MPI_Type_extent(rtype, &recvExtent); //getting extent
	MPI_Type_size(rtype, &recvExtent);
	MPI_Type_size(stype, &sendExtent);

	
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
	

	for (int i = 0; i < rcount * size; i++) rbuf[i] =  -1;




	if (rank == root) {
		cout << "INT Array Before:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;


	}
	




	MPI_Gather(sbuf, scount, MPI_INT, rbuf, rcount, MPI_INT, root, MPI_COMM_WORLD);

	if (rank == root) {

		cout << "Final INT Array:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;
	}


	delete sbuf, rbuf;
	MPI_Finalize();
	return 0;
}