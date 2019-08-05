/*
 *
 * Beyazit Yalcinkaya
 * 2172138
 *
 */


#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "writeOutput.h"

#define SLEEP(time) (usleep(time - (time * 0.01) + (rand() % (int)(time * 0.02))))
#define INIT(sem, n) (sem_init(&(sem), 0, n))
#define WAIT(sem) (sem_wait(&(sem)))
#define POST(sem) (sem_post(&(sem)))
#define TIMED_WAIT(sem, ts) sem_timedwait(&(sem), &(ts))
#define LOCK(mutex) pthread_mutex_lock(&(mutex))
#define UNLOCK(mutex) pthread_mutex_unlock(&(mutex))

sem_t oreProduced;
sem_t smelterIronStorageAvail;
sem_t smelterCopperStorageAvail;
sem_t foundryIronStorageAvail;
sem_t foundryCoalStorageAvail;
sem_t smelterAndFoundryIronStorageAvail;

pthread_mutex_t mutexK = PTHREAD_MUTEX_INITIALIZER;

unsigned int K = 0;

typedef struct {
	pthread_t tid;
	unsigned int id;
	unsigned int interval;
	unsigned int capacity;
	unsigned int maxOreCount;
	unsigned int currentOreCount;
	unsigned int reservedOreCount;
	pthread_mutex_t mutexCurrentOreCount;
	pthread_mutex_t mutexMaxOreCount;
	pthread_mutex_t mutexPickedByATransporter;
	OreType type;
	sem_t canProduce;
} Miner;

typedef struct {
	pthread_t tid;
	unsigned int id;
	unsigned int interval;
	unsigned int capacity;
	unsigned int producedIngotCount;
	unsigned int waitingOreCount;
	unsigned int reservedStorageCount;
	pthread_mutex_t mutexWaitingOreCount;
	pthread_mutex_t mutexPickedByATransporter;
	OreType type;
	sem_t twoOres;
} Smelter;

typedef struct {
	pthread_t tid;
	unsigned int id;
	unsigned int interval;
	unsigned int capacity;
	unsigned int producedIngotCount;
	unsigned int waitingIronCount;
	unsigned int reservedIronStorageCount;
	unsigned int waitingCoalCount;
	unsigned int reservedCoalStorageCount;
	pthread_mutex_t mutexWaitingIronCount;
	pthread_mutex_t mutexWaitingCoalCount;
	pthread_mutex_t mutexPickedByATransporter;
	sem_t oneIronOneCoal;
} Foundry;

typedef struct {
	pthread_t tid;
	unsigned int id;
	unsigned int interval;
	unsigned int numberOfMiners;
	unsigned int numberOfSmelters;
	unsigned int numberOfFoundries;
	Miner *miners;
	Smelter *smelters;
	Foundry *foundries;
	OreType carriedOre;
} Transporter;

void *minerThreadRoutine(void *arg) {
	Miner *miner = (Miner *) arg;
	MinerInfo minerInfo;
	LOCK(miner -> mutexCurrentOreCount);
	miner -> currentOreCount = 0;
	miner -> reservedOreCount = 0;
	FillMinerInfo(&minerInfo, miner -> id, miner -> type, miner -> capacity, miner -> currentOreCount);
	WriteOutput(&minerInfo, NULL, NULL, NULL, MINER_CREATED);
	UNLOCK(miner -> mutexCurrentOreCount);
	LOCK(miner -> mutexMaxOreCount);
	while ((miner -> maxOreCount) > 0) {
		UNLOCK(miner -> mutexMaxOreCount);
		WAIT(miner -> canProduce);
		LOCK(miner -> mutexCurrentOreCount);
		FillMinerInfo(&minerInfo, miner -> id, miner -> type, miner -> capacity, miner -> currentOreCount);
		WriteOutput(&minerInfo, NULL, NULL, NULL, MINER_STARTED);
		UNLOCK(miner -> mutexCurrentOreCount);
		SLEEP(miner -> interval);
		LOCK(miner -> mutexCurrentOreCount);
		miner -> currentOreCount++;
		LOCK(miner -> mutexMaxOreCount);
		miner -> maxOreCount--;
		UNLOCK(miner -> mutexMaxOreCount);
		POST(oreProduced);
		FillMinerInfo(&minerInfo, miner -> id, miner -> type, miner -> capacity, miner -> currentOreCount);
		WriteOutput(&minerInfo, NULL, NULL, NULL, MINER_FINISHED);
		UNLOCK(miner -> mutexCurrentOreCount);
		SLEEP(miner -> interval);
		LOCK(miner -> mutexMaxOreCount);
	}
	UNLOCK(miner -> mutexMaxOreCount);
	LOCK(miner -> mutexCurrentOreCount);
	FillMinerInfo(&minerInfo, miner -> id, miner -> type, miner -> capacity, miner -> currentOreCount);
	WriteOutput(&minerInfo, NULL, NULL, NULL, MINER_STOPPED);
	UNLOCK(miner -> mutexCurrentOreCount);
	return NULL;
}

int contTransporting(Miner *miners, unsigned int numberOfMiners) {
	unsigned int i;
	for (i = 0; i < numberOfMiners; i++) {
		LOCK(miners[i].mutexCurrentOreCount);
		LOCK(miners[i].mutexMaxOreCount);
		if (miners[i].maxOreCount > 0 || (miners[i].currentOreCount > miners[i].reservedOreCount)) {
			UNLOCK(miners[i].mutexMaxOreCount);
			UNLOCK(miners[i].mutexCurrentOreCount);
			return 1;
		}
		UNLOCK(miners[i].mutexMaxOreCount);
		UNLOCK(miners[i].mutexCurrentOreCount);
	}
	return 0;
}

Miner *nextLoad(Miner *miners, unsigned int numberOfMiners) {
	WAIT(oreProduced);
	LOCK(mutexK);
	unsigned int initK = K;
	do {
		K = ((K == (numberOfMiners - 1)) ? (0) : (K + 1));
		LOCK(miners[K].mutexCurrentOreCount);
		if (miners[K].currentOreCount > miners[K].reservedOreCount) {
			miners[K].reservedOreCount++;
			UNLOCK(mutexK);
			return &miners[K];
		}
		UNLOCK(miners[K].mutexCurrentOreCount);
	} while (K != initK);
	LOCK(miners[K].mutexCurrentOreCount);
	if (miners[K].currentOreCount > miners[K].reservedOreCount) {
		miners[K].reservedOreCount++;
		UNLOCK(mutexK);
		return &miners[K];
	}
	UNLOCK(miners[K].mutexCurrentOreCount);
	UNLOCK(mutexK);
	return NULL;
}

int pickProducer(void **temp, OreType carriedOre, Smelter *smelters, unsigned int numberOfSmelters, Foundry *foundries, unsigned int numberOfFoundries) {
	unsigned int i;
	*temp = NULL;
	if (carriedOre == IRON) {
		WAIT(smelterAndFoundryIronStorageAvail);
		for (i = 0; i < numberOfSmelters; i++) {
			LOCK(smelters[i].mutexWaitingOreCount);
			if ((smelters[i].waitingOreCount < smelters[i].capacity) &&
				(smelters[i].type == IRON) &&
			   ((smelters[i].waitingOreCount % 2) == 1) &&
			   ((smelters[i].capacity - smelters[i].waitingOreCount) > smelters[i].reservedStorageCount)) {
				WAIT(smelterIronStorageAvail);
				smelters[i].reservedStorageCount++;
				*temp = (void *)&smelters[i];
				//mutexes will be unlocked after return
				return 1;
			}
			UNLOCK(smelters[i].mutexWaitingOreCount);
		}
		for (i = 0; i < numberOfFoundries; i++) {
			LOCK(foundries[i].mutexWaitingIronCount);
			if ((foundries[i].waitingIronCount < foundries[i].capacity) &&
			   (foundries[i].waitingCoalCount > foundries[i].waitingIronCount) &&
			   ((foundries[i].capacity - foundries[i].waitingIronCount) > foundries[i].reservedIronStorageCount)) {
				WAIT(foundryIronStorageAvail);
				foundries[i].reservedIronStorageCount++;
				*temp = (void *)&foundries[i];
				//mutexes will be unlocked after return
				return 0;
			}
			UNLOCK(foundries[i].mutexWaitingIronCount);
		}
		for (i = 0; i < numberOfSmelters; i++) {
			LOCK(smelters[i].mutexWaitingOreCount);
			if ((smelters[i].waitingOreCount < smelters[i].capacity) &&
				(smelters[i].type == IRON) &&
			   ((smelters[i].capacity - smelters[i].waitingOreCount) > smelters[i].reservedStorageCount)) {
				WAIT(smelterIronStorageAvail);
				smelters[i].reservedStorageCount++;
				*temp = (void *)&smelters[i];
				//mutexes will be unlocked after return
				return 1;
			}
			UNLOCK(smelters[i].mutexWaitingOreCount);
		}
		for (i = 0; i < numberOfFoundries; i++) {
			LOCK(foundries[i].mutexWaitingIronCount);
			if ((foundries[i].waitingIronCount < foundries[i].capacity) &&
			   ((foundries[i].capacity - foundries[i].waitingIronCount) > foundries[i].reservedIronStorageCount)) {
				WAIT(foundryIronStorageAvail);
				foundries[i].reservedIronStorageCount++;
				*temp = (void *)&foundries[i];
				//mutexes will be unlocked after return
				return 0;
			}
			UNLOCK(foundries[i].mutexWaitingIronCount);
		}

	} else if (carriedOre == COAL) {
		WAIT(foundryCoalStorageAvail);
		for (i = 0; i < numberOfFoundries; i++) {
			LOCK(foundries[i].mutexWaitingCoalCount);
			if ((foundries[i].waitingCoalCount < foundries[i].capacity) &&
			   (foundries[i].waitingIronCount > foundries[i].waitingCoalCount) &&
			   ((foundries[i].capacity - foundries[i].waitingCoalCount) > foundries[i].reservedCoalStorageCount)) {
			   	foundries[i].reservedCoalStorageCount++;
				*temp = (void *)&foundries[i];
				//mutexes will be unlocked after return
				return 0;
			}
			UNLOCK(foundries[i].mutexWaitingCoalCount);
		}
		for (i = 0; i < numberOfFoundries; i++) {
			LOCK(foundries[i].mutexWaitingCoalCount);
			if ((foundries[i].waitingCoalCount < foundries[i].capacity) &&
			   ((foundries[i].capacity - foundries[i].waitingCoalCount) > foundries[i].reservedCoalStorageCount)) {
			   	foundries[i].reservedCoalStorageCount++;
				*temp = (void *)&foundries[i];
				//mutexes will be unlocked after return
				return 0;
			}
			UNLOCK(foundries[i].mutexWaitingCoalCount);
		}

	} else if (carriedOre == COPPER) {
		WAIT(smelterCopperStorageAvail);
		for (i = 0; i < numberOfSmelters; i++) {
			LOCK(smelters[i].mutexWaitingOreCount);
			if ((smelters[i].waitingOreCount < smelters[i].capacity) &&
				(smelters[i].type == COPPER) &&
			   ((smelters[i].waitingOreCount % 2) == 1) &&
			   ((smelters[i].capacity - smelters[i].waitingOreCount) > smelters[i].reservedStorageCount)) {
				smelters[i].reservedStorageCount++;
				*temp = (void *)&smelters[i];
				//mutexes will be unlocked after return
				return 1;
			}
			UNLOCK(smelters[i].mutexWaitingOreCount);
		}
		for (i = 0; i < numberOfSmelters; i++) {
			LOCK(smelters[i].mutexWaitingOreCount);
			if ((smelters[i].waitingOreCount < smelters[i].capacity) &&
				(smelters[i].type == COPPER) &&
			   ((smelters[i].capacity - smelters[i].waitingOreCount) > smelters[i].reservedStorageCount)) {
				smelters[i].reservedStorageCount++;
				*temp = (void *)&smelters[i];
				//mutexes will be unlocked after return
				return 1;
			}
			UNLOCK(smelters[i].mutexWaitingOreCount);
		}
	}
	return -1;
}

void *transporterThreadRoutine(void *arg) {
	Transporter *transporter = (Transporter *) arg;
	TransporterInfo transporterInfo;
	transporter -> carriedOre = IRON;
	Miner *miner;
	MinerInfo minerInfo;
	SmelterInfo smelterInfo;
	FoundryInfo foundryInfo;
	void *temp;
	Smelter *smelter;
	Foundry *foundry;
	FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
	WriteOutput(NULL, &transporterInfo, NULL, NULL, TRANSPORTER_CREATED);
	while (contTransporting(transporter -> miners, transporter -> numberOfMiners)) {
		transporter -> carriedOre = IRON;
		miner = nextLoad(transporter -> miners, transporter -> numberOfMiners);
		if (!miner)
			break;
		//START: Transporter thread miner routine
		FillMinerInfo(&minerInfo, miner -> id, IRON, 0, 0);
		FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
		WriteOutput(&minerInfo, &transporterInfo, NULL, NULL, TRANSPORTER_TRAVEL);
		UNLOCK(miner -> mutexCurrentOreCount);
		SLEEP(transporter -> interval);
		LOCK(miner -> mutexPickedByATransporter);
		LOCK(miner -> mutexCurrentOreCount);
		miner -> currentOreCount--;
		miner -> reservedOreCount--;
		transporter -> carriedOre = miner -> type;
		FillMinerInfo(&minerInfo, miner -> id, miner -> type, miner -> capacity, miner -> currentOreCount);
		FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
		WriteOutput(&minerInfo, &transporterInfo, NULL, NULL, TRANSPORTER_TAKE_ORE);
		UNLOCK(miner -> mutexCurrentOreCount);//This is before the sleep since miners can mine while transporters are loading.
		SLEEP(transporter -> interval);
		UNLOCK(miner -> mutexPickedByATransporter);
		POST(miner -> canProduce);
		// END: Transporter thread miner routine
		if (pickProducer(&temp, transporter -> carriedOre, transporter -> smelters, transporter -> numberOfSmelters, transporter -> foundries, transporter -> numberOfFoundries)) {
			if (!temp)
				break;
			smelter = (Smelter *)temp;
			//START: Transporter thread smelter routine
			FillSmelterInfo(&smelterInfo, smelter -> id, IRON, 0, 0 ,0);
			FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
			WriteOutput(NULL, &transporterInfo, &smelterInfo, NULL, TRANSPORTER_TRAVEL);
			UNLOCK(smelter -> mutexWaitingOreCount);
			SLEEP(transporter -> interval);
			LOCK(smelter -> mutexPickedByATransporter);
			LOCK(smelter -> mutexWaitingOreCount);
			smelter -> waitingOreCount++;
			smelter -> reservedStorageCount--;
			FillSmelterInfo(&smelterInfo, smelter -> id, smelter -> type, smelter -> capacity, smelter -> waitingOreCount, smelter -> producedIngotCount);
			FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
			WriteOutput(NULL, &transporterInfo, &smelterInfo, NULL, TRANSPORTER_DROP_ORE);
			UNLOCK(smelter -> mutexWaitingOreCount);
			SLEEP(transporter -> interval);
			UNLOCK(smelter -> mutexPickedByATransporter);
			LOCK(smelter -> mutexWaitingOreCount);
			if (((smelter -> waitingOreCount) > 0) && (((smelter -> waitingOreCount) % 2) == 0))
				POST(smelter -> twoOres);
			UNLOCK(smelter -> mutexWaitingOreCount);
			//END: Transporter thread smelter routine
		} else {
			foundry = (Foundry *)temp;
			//START: Transporter thread foundry routine
			if ((transporter -> carriedOre) == IRON) {
				FillFoundryInfo(&foundryInfo, foundry -> id, 0, 0, 0, 0);
				FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
				WriteOutput(NULL, &transporterInfo, NULL, &foundryInfo, TRANSPORTER_TRAVEL);
				UNLOCK(foundry -> mutexWaitingIronCount);
				SLEEP(transporter -> interval);
				LOCK(foundry -> mutexPickedByATransporter);
				LOCK(foundry -> mutexWaitingIronCount);
				foundry -> waitingIronCount++;
				foundry -> reservedIronStorageCount--;
				FillFoundryInfo(&foundryInfo, foundry -> id, foundry -> capacity, foundry -> waitingIronCount, foundry -> waitingCoalCount, foundry -> producedIngotCount);
				FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
				WriteOutput(NULL, &transporterInfo, NULL, &foundryInfo, TRANSPORTER_DROP_ORE);
				UNLOCK(foundry -> mutexWaitingIronCount);
				SLEEP(transporter -> interval);
				UNLOCK(foundry -> mutexPickedByATransporter);
				LOCK(foundry -> mutexWaitingIronCount);LOCK(foundry -> mutexWaitingCoalCount);
				if (((foundry -> waitingIronCount) <= (foundry -> waitingCoalCount))&&foundry -> waitingIronCount)
					POST(foundry -> oneIronOneCoal);
				UNLOCK(foundry -> mutexWaitingCoalCount);UNLOCK(foundry -> mutexWaitingIronCount);
			}
			else {
				FillFoundryInfo(&foundryInfo, foundry -> id, 0, 0, 0, 0);
				FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
				WriteOutput(NULL, &transporterInfo, NULL, &foundryInfo, TRANSPORTER_TRAVEL);
				UNLOCK(foundry -> mutexWaitingCoalCount);
				SLEEP(transporter -> interval);
				LOCK(foundry -> mutexPickedByATransporter);
				LOCK(foundry -> mutexWaitingCoalCount);
				foundry -> waitingCoalCount++;
				foundry -> reservedCoalStorageCount--;
				FillFoundryInfo(&foundryInfo, foundry -> id, foundry -> capacity, foundry -> waitingIronCount, foundry -> waitingCoalCount, foundry -> producedIngotCount);
				FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
				WriteOutput(NULL, &transporterInfo, NULL, &foundryInfo, TRANSPORTER_DROP_ORE);
				UNLOCK(foundry -> mutexWaitingCoalCount);
				SLEEP(transporter -> interval);
				UNLOCK(foundry -> mutexPickedByATransporter);
				LOCK(foundry -> mutexWaitingIronCount);LOCK(foundry -> mutexWaitingCoalCount);
				if (((foundry -> waitingCoalCount) <= (foundry -> waitingIronCount))&&foundry -> waitingCoalCount)
					POST(foundry -> oneIronOneCoal);
				UNLOCK(foundry -> mutexWaitingCoalCount);UNLOCK(foundry -> mutexWaitingIronCount);
			}
			//END: Transporter thread foundry routine
		}
	}
	FillTransporterInfo(&transporterInfo, transporter -> id, &transporter -> carriedOre);
	WriteOutput(NULL, &transporterInfo, NULL, NULL, TRANSPORTER_STOPPED);
	return NULL;
}

void *smelterThreadRoutine(void *arg) {
	struct timespec ts;
	Smelter *smelter = (Smelter *) arg;
	SmelterInfo smelterInfo;
	smelter -> producedIngotCount = 0;
	LOCK(smelter -> mutexWaitingOreCount);
	smelter -> waitingOreCount = 0;
	FillSmelterInfo(&smelterInfo, smelter -> id, smelter -> type, smelter -> capacity, smelter -> waitingOreCount, smelter -> producedIngotCount);
	WriteOutput(NULL, NULL, &smelterInfo, NULL, SMELTER_CREATED);
	UNLOCK(smelter -> mutexWaitingOreCount);
	while (1) {
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += 5;
		if (TIMED_WAIT(smelter -> twoOres, ts) == -1 && errno == ETIMEDOUT)
			break;
		LOCK(smelter -> mutexWaitingOreCount);
		smelter -> waitingOreCount -= 2;
		smelter -> reservedStorageCount += 2;
		FillSmelterInfo(&smelterInfo, smelter -> id, smelter -> type, smelter -> capacity, smelter -> waitingOreCount, smelter -> producedIngotCount);
		WriteOutput(NULL, NULL, &smelterInfo, NULL, SMELTER_STARTED);
		UNLOCK(smelter -> mutexWaitingOreCount);
		SLEEP(smelter -> interval);
		LOCK(smelter -> mutexWaitingOreCount);
		smelter -> producedIngotCount++;
		smelter -> reservedStorageCount -= 2;
		if (smelter -> type == IRON) {
			POST(smelterAndFoundryIronStorageAvail);POST(smelterAndFoundryIronStorageAvail);
			POST(smelterIronStorageAvail);
			POST(smelterIronStorageAvail);
		} else {
			POST(smelterCopperStorageAvail);
			POST(smelterCopperStorageAvail);
		}
		FillSmelterInfo(&smelterInfo, smelter -> id, smelter -> type, smelter -> capacity, smelter -> waitingOreCount, smelter -> producedIngotCount);
		WriteOutput(NULL, NULL, &smelterInfo, NULL, SMELTER_FINISHED);
		UNLOCK(smelter -> mutexWaitingOreCount);
	}
	LOCK(smelter -> mutexWaitingOreCount);
	FillSmelterInfo(&smelterInfo, smelter -> id, smelter -> type, smelter -> capacity, smelter -> waitingOreCount, smelter -> producedIngotCount);
	WriteOutput(NULL, NULL, &smelterInfo, NULL, SMELTER_STOPPED);
	UNLOCK(smelter -> mutexWaitingOreCount);
	return NULL;
}

void *foundryThreadRoutine(void *arg) {
	struct timespec ts;
	Foundry *foundry = (Foundry *) arg;
	FoundryInfo foundryInfo;
	foundry -> producedIngotCount = 0;
	LOCK(foundry -> mutexWaitingIronCount);
	LOCK(foundry -> mutexWaitingCoalCount);
	foundry -> waitingIronCount = 0;
	foundry -> waitingCoalCount = 0;
	FillFoundryInfo(&foundryInfo, foundry -> id, foundry -> capacity, foundry -> waitingIronCount, foundry -> waitingCoalCount, foundry -> producedIngotCount);
	WriteOutput(NULL, NULL, NULL, &foundryInfo, FOUNDRY_CREATED);
	UNLOCK(foundry -> mutexWaitingCoalCount);
	UNLOCK(foundry -> mutexWaitingIronCount);
	while (1) {
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += 5;
		if (TIMED_WAIT(foundry -> oneIronOneCoal, ts) == -1 && errno == ETIMEDOUT)
			break;
		LOCK(foundry -> mutexWaitingIronCount);
		LOCK(foundry -> mutexWaitingCoalCount);
		foundry -> waitingIronCount--;
		foundry -> waitingCoalCount--;
		foundry -> reservedIronStorageCount++;
		foundry -> reservedCoalStorageCount++;
		FillFoundryInfo(&foundryInfo, foundry -> id, foundry -> capacity, foundry -> waitingIronCount, foundry -> waitingCoalCount, foundry -> producedIngotCount);
		WriteOutput(NULL, NULL, NULL, &foundryInfo, FOUNDRY_STARTED);
		UNLOCK(foundry -> mutexWaitingCoalCount);
		UNLOCK(foundry -> mutexWaitingIronCount);
		SLEEP(foundry -> interval);
		LOCK(foundry -> mutexWaitingIronCount);
		LOCK(foundry -> mutexWaitingCoalCount);
		foundry -> producedIngotCount++;
		foundry -> reservedIronStorageCount--;
		foundry -> reservedCoalStorageCount--;
		POST(smelterAndFoundryIronStorageAvail);
		POST(foundryIronStorageAvail);
		POST(foundryCoalStorageAvail);
		FillFoundryInfo(&foundryInfo, foundry -> id, foundry -> capacity, foundry -> waitingIronCount, foundry -> waitingCoalCount, foundry -> producedIngotCount);
		WriteOutput(NULL, NULL, NULL, &foundryInfo, FOUNDRY_FINISHED);
		UNLOCK(foundry -> mutexWaitingCoalCount);
		UNLOCK(foundry -> mutexWaitingIronCount);
	}
	LOCK(foundry -> mutexWaitingIronCount);
	LOCK(foundry -> mutexWaitingCoalCount);
	FillFoundryInfo(&foundryInfo, foundry -> id, foundry -> capacity, foundry -> waitingIronCount, foundry -> waitingCoalCount, foundry -> producedIngotCount);
	WriteOutput(NULL, NULL, NULL, &foundryInfo, FOUNDRY_STOPPED);
	UNLOCK(foundry -> mutexWaitingCoalCount);
	UNLOCK(foundry -> mutexWaitingIronCount);
	return NULL;
}

int main() {
	// Declarations
	unsigned int numberOfMiners;
	unsigned int numberOfTransporters;
	unsigned int numberOfSmelters;
	unsigned int numberOfFoundries;
	unsigned int totalSmelterIronStorage = 0;
	unsigned int totalSmelterCopperStorage = 0;
	unsigned int totalFoundryStorage = 0;
	unsigned int i;
	Miner *miners;
	Transporter *transporters;
	Smelter *smelters;
	Foundry *foundries;
	// Scan miners
	scanf("%u", &numberOfMiners);
	miners = (Miner *)malloc(sizeof(Miner) * numberOfMiners);
	for (i = 1; i <= numberOfMiners; i++) {
		miners[i - 1].id = i;
		scanf("%u %u %u %u", &(miners[i - 1].interval), &(miners[i - 1].capacity), &(miners[i - 1].type), &(miners[i - 1].maxOreCount));
		INIT(miners[i - 1].canProduce, miners[i - 1].capacity);
		pthread_mutex_init(&miners[i - 1].mutexCurrentOreCount, NULL);
		pthread_mutex_init(&miners[i - 1].mutexMaxOreCount, NULL);
		pthread_mutex_init(&miners[i - 1].mutexPickedByATransporter, NULL);
	}
	// Scan transporters
	scanf("%u", &numberOfTransporters);
	transporters = (Transporter *)malloc(sizeof(Transporter) * numberOfTransporters);
	for (i = 1; i <= numberOfTransporters; i++) {
		transporters[i - 1].id = i;
		scanf("%u", &(transporters[i - 1].interval));
	}
	// Scan smelters
	scanf("%u", &numberOfSmelters);
	smelters = (Smelter *)malloc(sizeof(Smelter) * numberOfSmelters);
	for (i = 1; i <= numberOfSmelters; i++) {
		smelters[i - 1].id = i;
		scanf("%u %u %u", &(smelters[i - 1].interval), &(smelters[i - 1].capacity), &(smelters[i - 1].type));
		INIT(smelters[i - 1].twoOres, 0);
		pthread_mutex_init(&smelters[i - 1].mutexWaitingOreCount, NULL);
		pthread_mutex_init(&smelters[i - 1].mutexPickedByATransporter, NULL);
		if (smelters[i - 1].type == IRON) totalSmelterIronStorage += smelters[i - 1].capacity;
		else totalSmelterCopperStorage += smelters[i - 1].capacity;

	}
	// Scan foundry
	scanf("%u", &numberOfFoundries);
	foundries = (Foundry *)malloc(sizeof(Foundry) * numberOfFoundries);
	for (i = 1; i <= numberOfFoundries; i++) {
		foundries[i - 1].id = i;
		scanf("%u %u", &(foundries[i - 1].interval), &(foundries[i - 1].capacity));
		INIT(foundries[i - 1].oneIronOneCoal, 0);
		pthread_mutex_init(&foundries[i - 1].mutexWaitingIronCount, NULL);
		pthread_mutex_init(&foundries[i - 1].mutexWaitingCoalCount, NULL);
		pthread_mutex_init(&foundries[i - 1].mutexPickedByATransporter, NULL);
		totalFoundryStorage += foundries[i - 1].capacity;
	}
	// Initialize semaphores
	INIT(oreProduced, 0);
	INIT(smelterAndFoundryIronStorageAvail, totalSmelterIronStorage + totalFoundryStorage);
	INIT(smelterIronStorageAvail, totalSmelterIronStorage);
	INIT(smelterCopperStorageAvail, totalSmelterCopperStorage);
	INIT(foundryIronStorageAvail, totalFoundryStorage);
	INIT(foundryCoalStorageAvail, totalFoundryStorage);
	// Create threads
	InitWriteOutput();	
	for (i = 0; i < numberOfMiners; i++)
		pthread_create(&(miners[i].tid), NULL, minerThreadRoutine, (void *)&miners[i]);
	for (i = 0; i < numberOfTransporters; i++) {
		transporters[i].miners = miners;
		transporters[i].smelters = smelters;
		transporters[i].foundries = foundries;
		transporters[i].numberOfMiners = numberOfMiners;
		transporters[i].numberOfSmelters = numberOfSmelters;
		transporters[i].numberOfFoundries = numberOfFoundries;
		pthread_create(&(transporters[i].tid), NULL, transporterThreadRoutine, (void *)&transporters[i]);
	}
	for (i = 0; i < numberOfSmelters; i++)
		pthread_create(&(smelters[i].tid), NULL, smelterThreadRoutine, (void *)&smelters[i]);
	for (i = 0; i < numberOfFoundries; i++)
		pthread_create(&(foundries[i].tid), NULL, foundryThreadRoutine, (void *)&foundries[i]);
	// Wait for threads
	for (i = 0; i < numberOfMiners; i++)
		pthread_join(miners[i].tid, NULL);
	for (i = 0; i < numberOfTransporters; i++)
		pthread_join(transporters[i].tid, NULL);
	for (i = 0; i < numberOfSmelters; i++)
		pthread_join(smelters[i].tid, NULL);
	for (i = 0; i < numberOfFoundries; i++)
		pthread_join(foundries[i].tid, NULL);
	// Free allocated memory
	free(miners);
	free(transporters);
	free(smelters);
	free(foundries);
	return 0;
}

