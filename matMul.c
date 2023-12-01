// Rasheed Alqobbaj - 1202474

// Libraries
#include <stdio.h> // standard input/output library
#include <stdlib.h> // standard library 
#include <unistd.h> // standard symbolic constants and types for POSIX includes (fork(), pipe(), I/O primitives (read(), write(), close(), etc.))
#include <sys/wait.h> // declarations for waiting (waitpid())
#include <pthread.h> // POSIX threads library includes (pthread_create(), pthread_join(), etc.)
#include <string.h> // string operations (memcpy())
#include <time.h> // time functions (clock(), time(), etc.)

// funtions to fill matrices
void fillMatrix1(int matrix[100][100]){
    // fill matrix with one digit a time of id repeated
    int counter = 0;
    int id[] = {1,2,0,2,4,7,4};
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            matrix[i][j] = id[counter % 7];
            counter++;
        }
    }
}

void fillMatrix2(int matrix[100][100]){
    // fill matrix with one digit a time of id * year of birth repeated
    // id * year of birth = 1202474 * 2002 = 2405948948
    int counter = 0;
    int idyob[] = {2,4,0,7,3,5,2,9,4,8};
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            matrix[i][j] = idyob[counter % 10];
            counter++;
        }
    }
}

void fillResult(int result[100][100]){
    // fill matrix with 0 to reset/initialize
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            result[i][j] = 0;
        }
    }
}

// function to print matrix
void printMatrix(int matrix[100][100]){
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// functions to multiply matrices
double naiveMultiplyMatrix(int matrix1[100][100], int matrix2[100][100], int result[100][100]) {
    // function to naively multiply matrices
    
    clock_t start = clock(); // Record the start time

    // Multiply matrix1 by matrix2, store the result in result
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            for(int k = 0; k < 100; k++){
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    clock_t end = clock(); // Record the end time

    double time = ((double)end - start) / CLOCKS_PER_SEC; // Calculate the execution time

    return time;
}

// functions to multiply matrices using processes
double procMultiplyMatrix1(int matrix1[100][100], int matrix2[100][100], int result[100][100]) {
    // This function creats a new process for each element in the result matrix
    // The process calculates the result for that element and writes it to a pipe
    // The parent process reads the result from the pipe and stores it in the result matrix

    clock_t start = clock(); // Record the start time

    // Multiply matrix1 by matrix2, store the result in result
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            
            // File descriptors for the pipe
            int fd[2]; // fd[0] is the read end of the pipe, fd[1] is the write end of the pipe

            // Create the pipe
            if (pipe(fd) == -1) {
                perror("pipe"); // perror is a function that prints an error message to stderr
                exit(EXIT_FAILURE); // Exit with failure
            }

            pid_t pid = fork(); // Create a new process

            if (pid < 0) {
                // Fork failed
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {
                // Child process
                close(fd[0]); // Close the read end of the pipe

                // Calculate the result for this element
                int sum = 0;
                for(int k = 0; k < 100; k++){
                    sum += matrix1[i][k] * matrix2[k][j];
                }

                // Write the result to the pipe
                write(fd[1], &sum, sizeof(sum)); // Write function is used to write to a file descriptor
                close(fd[1]); // Close the write end of the pipe

                exit(EXIT_SUCCESS); // End the child process
            } else {
                // Parent process
                close(fd[1]); // Close the write end of the pipe

                // Read the result from the pipe
                read(fd[0], &result[i][j], sizeof(result[i][j]));
                close(fd[0]); // Close the read end of the pipe

                // Wait for the child process to end
                wait(NULL);
            }
        }
    }

    clock_t end = clock(); // Record the end time

    double time = ((double)end - start) / CLOCKS_PER_SEC; // Calculate the execution time

    return time;
}


double procMultiplyMatrix2(int matrix1[100][100], int matrix2[100][100], int result[100][100]) {
    // This function creats a new process for each row in the result matrix
    // The process calculates the result for that row and writes it to a pipe
    // The parent process reads the result from the pipe and stores it in the result matrix

    clock_t start = clock(); // Record the start time

    for(int i = 0; i < 100; i++){
        int fd[2]; // File descriptors for the pipe

        // Create the pipe
        if (pipe(fd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork(); // Create a new process

        if (pid < 0) {
            // Fork failed
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            close(fd[0]); // Close the read end of the pipe

            // Calculate the result for this row
            for(int j = 0; j < 100; j++){
                int sum = 0;
                for(int k = 0; k < 100; k++){
                    sum += matrix1[i][k] * matrix2[k][j];
                }
                // Write the result to the pipe
                write(fd[1], &sum, sizeof(sum));
            }
            close(fd[1]); // Close the write end of the pipe

            exit(EXIT_SUCCESS); // End the child process
        } else {
            // Parent process
            close(fd[1]); // Close the write end of the pipe

            // Read the result from the pipe
            for(int j = 0; j < 100; j++){
                read(fd[0], &result[i][j], sizeof(result[i][j]));
            }
            close(fd[0]); // Close the read end of the pipe

            // Wait for the child process to end
            wait(NULL);
        }
    }

    clock_t end = clock(); // Record the end time

    double time = ((double)end - start) / CLOCKS_PER_SEC; // Calculate the execution time

    return time;
}

double procMultiplyMatrixOptimized(int matrix1[100][100], int matrix2[100][100], int result[100][100]) {
    // This function creates a fixed number of child processes
    // Each child process computes a portion of the result matrix and writes it to a pipe
    // The parent process reads the results from the pipes and stores them in the result matrix

    clock_t start = clock(); // Record the start time

    int num_processes = 2; // Number of child processes to create; I chose 2 because I have 2 CPU cores on my VM
    int rows_per_process = 100 / num_processes; // Number of rows of the result matrix each child process will compute

    // File descriptors for the pipes
    int fd[num_processes][2];

    for (int p = 0; p < num_processes; p++) {
        // Create the pipe
        if (pipe(fd[p]) == -1) {
            perror("pipe"); // perror is a function that prints an error message to stderr (standard error)
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork(); // Create a new process

        if (pid < 0) {
            // Fork failed
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            close(fd[p][0]); // Close the read end of the pipe

            // Compute a portion of the result matrix
            int start = rows_per_process * p; // The first row this child process will compute
            int end = p == num_processes - 1 ? 100 : start + rows_per_process; // The last row this child process will compute (the ? : is a ternary operator which is a shorthand for if-else)
            for (int i = start; i < end; i++) {
                for (int j = 0; j < 100; j++) {
                    int sum = 0;
                    for (int k = 0; k < 100; k++) {
                        sum += matrix1[i][k] * matrix2[k][j];
                    }
                    // Write the result to the pipe
                    write(fd[p][1], &sum, sizeof(sum));
                }
            }

            close(fd[p][1]); // Close the write end of the pipe
            exit(EXIT_SUCCESS); // End the child process
        }
    }

    // Parent process
    for (int p = 0; p < num_processes; p++) {
        close(fd[p][1]); // Close the write end of the pipe

        // Read the result from the pipe
        int start = rows_per_process * p;
        int end = p == num_processes - 1 ? 100 : start + rows_per_process;
        for (int i = start; i < end; i++) {
            for (int j = 0; j < 100; j++) {
                read(fd[p][0], &result[i][j], sizeof(result[i][j]));
            }
        }

        close(fd[p][0]); // Close the read end of the pipe

        // Wait for the child process to end
        wait(NULL);
    }

    clock_t end = clock(); // Record the end time

    double time = ((double)end - start) / CLOCKS_PER_SEC; // Calculate the execution time

    return time;
}


// functions to multiply matrices using threads

// Struct for passing data to threads
typedef struct {
    int i;
    int j;
    int matrix1[100][100];
    int matrix2[100][100];
    int (*result)[100];
} ThreadData;

// Mutex for synchronizing access to the result matrix
// Mutex "mutual exclusion" is a lock that prevents multiple threads from accessing the same resource at the same time
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function for each thread to run
// void pointers are required by pthread_create
void* multiplyElement(void* arg) {
    ThreadData* data = (ThreadData*)arg; // Cast the void pointer to ThreadData pointer

    // Calculate the result for this element
    int sum = 0;
    for(int k = 0; k < 100; k++){
        sum += data->matrix1[data->i][k] * data->matrix2[k][data->j];
    }

    // Lock the mutex before writing to the result matrix
    pthread_mutex_lock(&mutex);

    // Write the result to the result matrix
    data->result[data->i][data->j] = sum;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex);

    // Free the memory for the thread data
    free(data);

    // returns null since void pointer is required by pthread_create and we don't need to return anything 
    // because we already wrote the result to the result matrix
    return NULL;
}

#define MATRIX_SIZE 100*100*sizeof(int) // Size of the matrix in bytes for memcpy

double thredMultiplyMatrix1(int matrix1[100][100], int matrix2[100][100], int result[100][100]) {
    // This function creats a new thread for each element in the result matrix
    // The thread calculates the result for that element and writes it to the result matrix

    clock_t start = clock(); // Record the start time

    pthread_t threads[100][100]; // Array of threads

    // Create a thread for each element of the result matrix
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            ThreadData* data = malloc(sizeof(ThreadData));
            data->i = i;
            data->j = j;
            // memcpy is a function that copies data from one memory location to another
            memcpy(data->matrix1, matrix1, MATRIX_SIZE); 
            memcpy(data->matrix2, matrix2, MATRIX_SIZE);
            data->result = result;

            pthread_create(&threads[i][j], NULL, multiplyElement, data); // Create a new thread
            // pthread_create takes a pointer to a pthread_t variable (threads[i][j])
            //                      a pointer to a pthread_attr_t variable (NULL for default attributes)
            //                      a pointer to a function (multiplyElement)
            //                      and a pointer to the data to pass to the function (data)
        }
    }

    // Wait for all threads to finish
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            pthread_join(threads[i][j], NULL); // Join a thread
            // pthread_join takes a pthread_t variable (threads[i][j])
            //                    and a pointer to a void pointer (NULL since we don't need to return anything)
        }
    }

    clock_t end = clock(); // Record the end time

    double time = ((double)end - start) / CLOCKS_PER_SEC; // Calculate the execution time

    return time;
}


// Function for each thread to run
void* multiplyRow(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    // Calculate the result for this row
    for(int j = 0; j < 100; j++){
        int sum = 0;
        for(int k = 0; k < 100; k++){
            sum += data->matrix1[data->i][k] * data->matrix2[k][j];
        }

        // Lock the mutex before writing to the result matrix
        pthread_mutex_lock(&mutex);

        // Write the result to the result matrix
        data->result[data->i][j] = sum;

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    // Free the memory for the thread data
    free(data);

    return NULL;
}

double thredMultiplyMatrix2(int matrix1[100][100], int matrix2[100][100], int result[100][100]) {
    // This function creats a new thread for each row in the result matrix
    // The thread calculates the result for that row and writes it to the result matrix

    clock_t start = clock(); // Record the start time

    pthread_t threads[100];

    // Create a thread for each row of the result matrix
    for(int i = 0; i < 100; i++){
        ThreadData* data = malloc(sizeof(ThreadData));
        data->i = i;
        memcpy(data->matrix1, matrix1, MATRIX_SIZE);
        memcpy(data->matrix2, matrix2, MATRIX_SIZE);
        data->result = result;

        pthread_create(&threads[i], NULL, multiplyRow, data);
    }

    // Wait for all threads to finish
    for(int i = 0; i < 100; i++){
        pthread_join(threads[i], NULL);
    }

    clock_t end = clock(); // Record the end time

    double time = ((double)end - start) / CLOCKS_PER_SEC; // Calculate the execution time

    return time;
}

pthread_mutex_t lock;
pthread_cond_t cond;
int finished_tasks = 0;

// Function for each deatched thread to run
void* deMultiplyElement(void* arg) {
    ThreadData* data = (ThreadData*)arg; // Cast the void pointer to ThreadData pointer

    // Calculate the result for this element
    int sum = 0;
    for(int k = 0; k < 100; k++){
        sum += data->matrix1[data->i][k] * data->matrix2[k][data->j];
    }

    // Lock the mutex before writing to the result matrix
    pthread_mutex_lock(&mutex);

    // Write the result to the result matrix
    data->result[data->i][data->j] = sum;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex);

    // Free the memory for the thread data
    free(data);

    // Increment the counter and signal the condition variable
    pthread_mutex_lock(&lock);
    finished_tasks++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);

    // returns null since void pointer is required by pthread_create and we don't need to return anything 
    // because we already wrote the result to the result matrix
    return NULL;
}

double thredMultiplyMatrix3(int matrix1[100][100], int matrix2[100][100], int result[100][100]) {
    // This function creats a new detached thread for each element in the result matrix
    // The thread calculates the result for that element and writes it to the result matrix

    // Initialize the mutex and condition variable
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    // Record the start time
    clock_t start = clock();

    pthread_t threads[100][100];
    pthread_attr_t attr;

    // Initialize the thread attribute
    pthread_attr_init(&attr);
    // Set the thread attribute to detached
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Create a detached thread for each element of the result matrix
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){
            ThreadData* data = malloc(sizeof(ThreadData));
            data->i = i;
            data->j = j;
            memcpy(data->matrix1, matrix1, MATRIX_SIZE);
            memcpy(data->matrix2, matrix2, MATRIX_SIZE);
            data->result = result;

            pthread_create(&threads[i][j], &attr, deMultiplyElement, data);
        }
    }

    // Destroy the thread attribute
    pthread_attr_destroy(&attr);

    // Wait for all threads to finish
    pthread_mutex_lock(&lock);
    while(finished_tasks < 100*100) {
        pthread_cond_wait(&cond, &lock);
    }
    pthread_mutex_unlock(&lock);

    // Record the end time after all threads have completed
    clock_t end = clock();

    // Calculate the time taken in seconds
    double time = ((double)end - start) / CLOCKS_PER_SEC;

    // Destroy the mutex and condition variable
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return time;
}

void* deMultiplyRow(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    // Calculate the result for the row
    for(int j = 0; j < 100; j++){
        int sum = 0;
        for(int k = 0; k < 100; k++){
            sum += data->matrix1[data->i][k] * data->matrix2[k][j];
        }

        // Lock the mutex before writing to the result matrix
        pthread_mutex_lock(&mutex);

        // Write the result to the result matrix
        data->result[data->i][j] = sum;

        // Unlock the mutex
        pthread_mutex_unlock(&mutex);
    }

    // Free the memory for the thread data
    free(data);

    // Increment the counter and signal the condition variable
    pthread_mutex_lock(&lock);
    finished_tasks++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);

    return NULL;
}

double thredMultiplyMatrix4(int matrix1[100][100], int matrix2[100][100], int result[100][100]) {
    // This function creats a new detached thread for each row in the result matrix
    // The thread calculates the result for that row and writes it to the result matrix

    // Initialize the mutex and condition variable
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
    finished_tasks = 0;

    clock_t start = clock(); // Record the start time

    pthread_t threads[100];
    pthread_attr_t attr;

    // Initialize the thread attribute
    pthread_attr_init(&attr);
    // Set the thread attribute to detached
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


    // Create a detached thread for each row of the result matrix
    for(int i = 0; i < 100; i++){
        ThreadData* data = malloc(sizeof(ThreadData));
        data->i = i;
        memcpy(data->matrix1, matrix1, MATRIX_SIZE);
        memcpy(data->matrix2, matrix2, MATRIX_SIZE);
        data->result = result;
        pthread_create(&threads[i], &attr, deMultiplyRow, data);
    }

    // Destroy the thread attribute
    pthread_attr_destroy(&attr);

    // Wait for all threads to finish
    pthread_mutex_lock(&lock);
    while(finished_tasks < 100) {
        pthread_cond_wait(&cond, &lock);
    }
    pthread_mutex_unlock(&lock);

    // Record the end time after all threads have completed
    clock_t end = clock();

    // Calculate the time taken in seconds
    double time = ((double)end - start) / CLOCKS_PER_SEC;

    // Destroy the mutex and condition variable
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return time;
}


// Function to print a table row
void printRow(const char* name, double time) {
    printf("| %-40s | %10.6f |\n", name, time);
}

int main() {
    // matrix of size 100x100 containing student number repeated
    int matrix1[100][100];
    // matrix of size 100x100 containing student number * year of birth repeated
    int matrix2[100][100];
    // matrix of size 100x100 containing result of matrix1 * matrix2
    int result[100][100];
    fillMatrix1(matrix1);
    fillMatrix2(matrix2);
    fillResult(result);
    
    // Array to store the execution times
    double times[8];

    // Perform the tasks and measure the execution times
    times[0] = naiveMultiplyMatrix(matrix1, matrix2, result);
    times[1] = procMultiplyMatrix1(matrix1, matrix2, result);
    times[2] = procMultiplyMatrix2(matrix1, matrix2, result);
    times[3] = thredMultiplyMatrix1(matrix1, matrix2, result);
    times[4] = thredMultiplyMatrix2(matrix1, matrix2, result);
    times[5] = thredMultiplyMatrix3(matrix1, matrix2, result);
    times[6] = thredMultiplyMatrix4(matrix1, matrix2, result);
    times[7] = procMultiplyMatrixOptimized(matrix1, matrix2, result);

    printf("\n");
    // Print the table header
    printf("|------------------------------------------|------------|\n");
    printf("| %-40s | %10s |\n", "Task", "Time (s)");
    printf("|------------------------------------------|------------|\n");

    // Print the table rows
    printRow("Naive", times[0]);
    printRow("Child Processes for each element", times[1]);
    printRow("Child Processes for each row", times[2]);
    printRow("Fixed Child Processes (2)", times[7]);
    printRow("Joinable Threads for each element", times[3]);
    printRow("Joinable Threads for each row", times[4]);
    printRow("Detached Threads for each element", times[5]);
    printRow("Detached Threads for each row", times[6]);
    printf("|------------------------------------------|------------|\n");
    printf("\n");
    return 0;
}