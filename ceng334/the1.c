#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int map(int N, char* map_proc) {
	int i;
	int j;
	char id[2];
	int status;
	pid_t pids[N];
	int fds[N][2];
	char buffer[1024];
	for (i = 0; i < N; i++)
		if (pipe(fds[i]) < 0)
			perror("pipe error");
	for (i = 0; i < N; i++) {
		if ((pids[i] = fork()) != 0) {
			;
		} else {
			for (j = 0; j < N; j++) {
				if (i != j) {
					close(fds[j][0]);
					close(fds[j][1]);
				}
			}
			close(fds[i][1]);
			dup2(fds[i][0], 0);
			close(fds[i][0]);
			sprintf(id, "%d", i);
			execlp(map_proc, map_proc, id, NULL);
		}
	}
	// Only parent will come here.
	for (i = 0; fgets(buffer, 1024, stdin); i++)
		write(fds[i % N][1], buffer, strlen(buffer));
	for (i = 0; i < N; i++) {
		close(fds[i][0]);
		close(fds[i][1]);
	}
	for (i = 0; i < N; i++)
		waitpid(pids[i], &status, 0);
	return 0;
}

int map_reduce(int N, char* map_proc, char* reduce_proc) {
	int i;
	int j;
	int status;
	char id[2];
	pid_t pids_mapper[N];
	pid_t pids_reducer[N];
	int fds_pm[N][2];
	int fds_mr[N][2];
	int fds_rr[N - 1][2];
	char buffer[1024];
	for (i = 0; i < N; i++) {
		if (pipe(fds_pm[i]) < 0)
			perror("pipe error");
		if (pipe(fds_mr[i]) < 0)
			perror("pipe error");
		if (i != N - 1)
			if (pipe(fds_rr[i]) < 0)
				perror("pipe error");
	}
	for (i = 0; i < N; i++) {
		if ((pids_mapper[i] = fork()) != 0) {
			;
			if ((pids_reducer[i] = fork()) != 0) {
				;
			} else {
				// Reducer
				for (j = 0; j < N; j++) {
					if (i != j) {
						close(fds_mr[j][0]);
						close(fds_mr[j][1]);
					}
					close(fds_pm[j][0]);
					close(fds_pm[j][1]);
				}
				close(fds_mr[i][1]);
				dup2(fds_mr[i][0], 0);
				close(fds_mr[i][0]);
				if (i != 0 && i != N - 1) {
					close(fds_rr[i - 1][1]);
					dup2(fds_rr[i - 1][0], 2);
					close(fds_rr[i - 1][0]);
					close(fds_rr[i][0]);
					dup2(fds_rr[i][1], 1);
					close(fds_rr[i][1]);
					for (j = 0; j < N - 1; j++) {
						if (j != i - 1 && j != i) {
							close(fds_rr[j][0]);
							close(fds_rr[j][1]);
						}
					}
				} else if (i == 0) {
					close(fds_rr[0][0]);
					dup2(fds_rr[0][1], 1);
					close(fds_rr[0][1]);
					for (j = 1; j < N - 1; j++) {
						close(fds_rr[j][0]);
						close(fds_rr[j][1]);
					}
				} else {
					close(fds_rr[N - 2][1]);
					dup2(fds_rr[N - 2][0], 2);
					close(fds_rr[N - 2][0]);
					for (j = 0; j < N - 2; j++) {
						close(fds_rr[j][0]);
						close(fds_rr[j][1]);
					}
				}
				sprintf(id, "%d", i);
				execlp(reduce_proc, reduce_proc, id, NULL);
			}
		} else {
			// Mapper
			for (j = 0; j < N; j++) {
				if (i != j) {
					close(fds_pm[j][0]);
					close(fds_pm[j][1]);
					close(fds_mr[j][0]);
					close(fds_mr[j][1]);
				}
				if (j != N - 1) {
					close(fds_rr[j][0]);
					close(fds_rr[j][1]);
				}
			}
			close(fds_pm[i][1]);
			dup2(fds_pm[i][0], 0);
			close(fds_pm[i][0]);
			close(fds_mr[i][0]);
			dup2(fds_mr[i][1], 1);
			close(fds_mr[i][1]);
			sprintf(id, "%d", i);
			execlp(map_proc, map_proc, id, NULL);
		}
	}
	// Only parent will come here.
	for (i = 0; fgets(buffer, 1024, stdin); i++)
		write(fds_pm[i % N][1], buffer, strlen(buffer));
	for (i = 0; i < N; i++) {
		close(fds_pm[i][0]);
		close(fds_pm[i][1]);
		close(fds_mr[i][0]);
		close(fds_mr[i][1]);
		close(fds_rr[i][0]);
		close(fds_rr[i][1]);
	}
	for (i = 0; i < N; i++) {
		waitpid(pids_mapper[i], &status, 0);
		waitpid(pids_reducer[i], &status, 0);
	}
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc == 3) {
		char pre[2];
		pre[0] = '.'; pre[1] = '/';
		char* map_proc = (char*)malloc(2 + strlen(argv[2]));
		strncpy(map_proc, "./", 2);
		strncat(map_proc, argv[2], strlen(argv[2]));
		map(atoi(argv[1]), map_proc);
		free(map_proc);
	} else if (argc == 4) {
		char pre[2];
		pre[0] = '.'; pre[1] = '/';
		char* map_proc = (char*)malloc(2 + strlen(argv[2]));
		char* reduce_proc = (char*)malloc(2 + strlen(argv[3]));
		strncpy(map_proc, "./", 2);
		strncpy(reduce_proc, "./", 2);
		strncat(map_proc, argv[2], strlen(argv[2]));
		strncat(reduce_proc, argv[3], strlen(argv[3]));
		map_reduce(atoi(argv[1]), map_proc, reduce_proc);
		free(map_proc);
		free(reduce_proc);
	}
	return 0;
}
