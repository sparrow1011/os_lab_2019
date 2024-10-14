#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int *child_pids = NULL;
int pnum;

void handle_timeout(int sig) {
    for (int i = 0; i < pnum; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGKILL);
        }
    }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;
  int timeout = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
              printf("seed is a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
              printf("array_size is a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
              printf("pnum is a positive number\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            if (timeout <= 0) {
              printf("timeout should be a positive number\n");
              return 1;
            }
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  child_pids = malloc(sizeof(int) * pnum);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int chunk_size = array_size / pnum;
  int **pipes = malloc(pnum * sizeof(int *));
  for (int i = 0; i < pnum; i++) {
    pipes[i] = malloc(2 * sizeof(int));
    pipe(pipes[i]);
  }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        int begin = i * chunk_size;
        int end = (i == pnum - 1) ? array_size : begin + chunk_size;
        struct MinMax min_max = GetMinMax(array, begin, end);

        if (with_files) {
          // use files here
          char filename[20];
          sprintf(filename, "result_%d.txt", i);
          FILE *file = fopen(filename, "w");
          fprintf(file, "%d %d\n", min_max.min, min_max.max);
          fclose(file);
        } else {
          // use pipe here
          close(pipes[i][0]); // close read end
          write(pipes[i][1], &min_max, sizeof(min_max));
          close(pipes[i][1]); // close write end
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  // Обработчик для SIGALRM
  if (timeout > 0) {
      signal(SIGALRM, handle_timeout);
      alarm(timeout); 
  }
  while (active_child_processes > 0) {
      int status;
      pid_t finished_pid = waitpid(-1, &status, WNOHANG);  // Неблокирующий wait
      if (finished_pid > 0) {
          active_child_processes--;
      }
  }
  
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
      char filename[20];
      sprintf(filename, "result_%d.txt", i);
      FILE *file = fopen(filename, "r");
      fscanf(file, "%d %d", &min, &max);
      fclose(file);
      remove(filename); 
    } else {
      // read from pipes
      close(pipes[i][1]); // close write end
      struct MinMax temp_min_max;
      read(pipes[i][0], &temp_min_max, sizeof(temp_min_max));
      close(pipes[i][0]); // close read end
      min = temp_min_max.min;
      max = temp_min_max.max;
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  for (int i = 0; i < pnum; i++) {
    free(pipes[i]);
  }
  free(pipes);

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  free(child_pids);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
