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

	int sendExtent;
	int recvExtent;

	MPI_Type_size(rtype, &recvExtent); //extent for rtype		
	MPI_Type_size(stype, &sendExtent); // extent for stype


	//filling the rbuf of root proc with data
	for (int i = 0; i < scount*sendExtent; i++) {
		*((char *)(rbuf)+root*recvExtent*scount + i) = *((char *)(sbuf)+i);
	}
	
	//filling the rest of rbuf with data except root proc
	
	//non-root procs
	if (trank != root) {
		MPI_Request request;
		MPI_Status status;
		for (int i = 0; i < root; i++) {
			MPI_Isend(sbuf, scount, stype, root, i, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		}
		for (int i = root + 1; i < tsize; i++) {
			MPI_Isend(sbuf, scount, stype, root, i, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		}
	}
	//root
	else {
		MPI_Request request;
		MPI_Status status;
		for (int i = 0; i < root; i++) {
			MPI_Irecv(((char *)rbuf + i*scount*recvExtent), scount, stype, i, i, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		}
		for (int i = root + 1; i < tsize; i++) {
			MPI_Irecv(((char *)rbuf + i*scount*recvExtent), scount, stype, i, i, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
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
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	root = atoi(argv[1]);

	if (root < 0 || root >= size) {
		return 0;
	}

	rcount = atoi(argv[2]);
	scount = atoi(argv[3]);

	//ints
	rbuf = new int[rcount*size];
	sbuf = new int[scount];
	for (int i = 0; i < scount; i++) sbuf[i] = rank;

	for (int i = 0; i < rcount * size; i++) rbuf[i] = -1;

	if (rank == root) {
		cout << "Array Before:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;
	}
	MPI_Gather(sbuf, scount, MPI_INT, rbuf, rcount, MPI_INT, root, MPI_COMM_WORLD);

	if (rank == root) {

		cout << "Final Array:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;
	}

	delete sbuf, rbuf;
	MPI_Finalize();
	return 0;
}