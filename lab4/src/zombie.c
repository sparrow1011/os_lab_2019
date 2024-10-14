#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid > 0) {
        // Родительский процесс
        printf("Родительский процесс, PID: %d\n", getpid());
        printf("Дочерний процесс завершён, но родитель не вызвал wait().\n");
        printf("Запустите команду 'ps aux' в другой консоли, чтобы увидеть зомби-процесс.\n");
        sleep(30);  // Родитель не вызывает wait() и ждет 30 секунд

        // Теперь родитель вызывает wait(), чтобы "похоронить" зомби-процесс
        int status;
        wait(&status);
        printf("Зомби-процесс убран после вызова wait().\n");
    } else if (pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс, PID: %d\n", getpid());
        printf("Дочерний процесс завершён.\n");
        exit(0);  // Дочерний процесс завершён
    } else {
        printf("Ошибка создания процесса.\n");
        return 1;
    }

    return 0;
}
