#include "mpi.h" 
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
	for (int i = 0; i < scount*sendExtent; i++)
		*((char *)(rbuf)+root*recvExtent*scount + i) = *((char *)(sbuf)+i);

	//filling the rest of rbuf with data except root proc

	//non-root procs
	if (trank != root) {
		MPI_Request *request = new MPI_Request[tsize - 1];
		MPI_Status *status = new MPI_Status[tsize - 1];
		for (int i = 0; i < root; i++)
			MPI_Isend(sbuf, scount, stype, root, i, MPI_COMM_WORLD, &request[i]);
		for (int i = root + 1; i < tsize; i++) 
			MPI_Isend(sbuf, scount, stype, root, i, MPI_COMM_WORLD, &request[i - 1]);
		MPI_Waitall(tsize - 1, request, status);
		delete[]request;
		delete[]status;
	}
	//root
	else {
		MPI_Request *request = new MPI_Request[tsize - 1];
		MPI_Status *status = new MPI_Status[tsize - 1];
		for (int i = 0; i < root; i++) 
			MPI_Irecv(((char *)rbuf + i*scount*recvExtent), scount, stype, i, i, MPI_COMM_WORLD, &request[i]);
		for (int i = root + 1; i < tsize; i++) 
			MPI_Irecv(((char *)rbuf + i*scount*recvExtent), scount, stype, i, i, MPI_COMM_WORLD, &request[i - 1]);
		
		MPI_Waitall(tsize - 1, request, status);
		delete[]request;
		delete[]status;
	}
	return 1;
}

int main(int argc, char * argv[]) {

	int size, rank;

	int *sbuf, *rbuf; // testing MPI_Gather with ints

	double *dsbuf, *drbuf; //testing MPI_Gather with doubles 

	float *fsbuf, *frbuf; //testing MPI_Gather with floats

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
	//doubles
	drbuf = new double[rcount*size];
	dsbuf = new double[scount];
	//floats
	frbuf = new float[rcount*size];
	fsbuf = new float[scount];

	for (int i = 0; i < scount; i++) {
		sbuf[i] = rank;
		dsbuf[i] = rank;
		fsbuf[i] = rank;
	}

	for (int i = 0; i < rcount * size; i++) {
		frbuf[i] = -1;
		rbuf[i] = -1;
		drbuf[i] = -1;
	}

	if (rank == root) {
		cout << "Array INT Before:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;
		cout << "Array FLOAT Before:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << frbuf[i] << "\t";
		}
		cout << endl;
		cout << "Array DOUBLE Before:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << drbuf[i] << "\t";
		}
		cout << endl;
	}
	MPI_Gather(dsbuf, scount, MPI_DOUBLE, drbuf, rcount, MPI_DOUBLE, root, MPI_COMM_WORLD);
	MPI_Gather(sbuf, scount, MPI_INT, rbuf, rcount, MPI_INT, root, MPI_COMM_WORLD);
	MPI_Gather(fsbuf, scount, MPI_FLOAT, frbuf, rcount, MPI_FLOAT, root, MPI_COMM_WORLD);

	if (rank == root) {

		cout << "Final INT Array:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << rbuf[i] << "\t";
		}
		cout << endl;
		cout << "Final FLOAT Array:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << frbuf[i] << "\t";
		}
		cout << endl;
		cout << "Final DOUBLE Array:" << endl;
		for (int i = 0; i < rcount * size; i++) {
			cout << drbuf[i] << "\t";
		}
		cout << endl;
	}
	delete sbuf, rbuf, drbuf, dsbuf, fsbuf, frbuf;

	MPI_Finalize();
	return 0;
}
