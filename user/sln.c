#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[]) {

    if (argc != 3){
        fprintf(2, "Usage: sln target linkname\n");
        exit();
    }

    if (symlink(argv[1], argv[2]) < 0){
        fprintf(2, "symlink %s %s: failed\n", argv[1], argv[2]);
        exit();
    }

    exit();

}
