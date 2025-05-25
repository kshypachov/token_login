#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

typedef int (*legacy_write_fn)( // Function prototype for writing a password to shared memory
    const char *pathname_for_ftok,
    const char *password_id,
    const char *password,
    int password_length,
    int permissions
);

typedef int (*legacy_read_fn)( // Function prototype for reading a password from shared memory
        const char *pathname_for_ftok,
        const char *password_id,
        char ** return_buff,
        int* return_buff_len
    );

int main() {
    const char *lib_path = "/usr/share/uxp/lib/libpasswordstore.so";  // Path to the shared library used for password handling
    const char *password = getenv("UXP_TOKEN_PASS"); // Environment variable used to provide the password to the process

    if (!password) {
        fprintf(stderr, "UXP_TOKEN_PASS env var is not set\n");
        return 1;
    }

    void *handle = dlopen(lib_path, RTLD_LAZY); // Dynamically load the shared library
    if (!handle) {
        fprintf(stderr, "Failed to load lib: %s\n", dlerror());
        return 1;
    }

    legacy_write_fn write_fn = (legacy_write_fn)dlsym(handle, "LEGACY_passwordWrite");
    legacy_read_fn read_fn = (legacy_read_fn)dlsym(handle, "LEGACY_passwordRead");
    if (!write_fn) {
        fprintf(stderr, "Failed to find function: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    if (!read_fn) {
        fprintf(stderr, "Failed to find function: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    const char *pathname = "/";      // must match the ftok path in a target process
    const char *id = "0";            // must match password_id in a target process
    int length = strlen(password);   // length of password in bytes
    int permissions = 0600;          // permissions for access to shared mem

    int write_result = write_fn(pathname, id, password, length, permissions);  // Send the password to shared memory. The return code does not indicate correctness.
    fprintf(stdout,"LEGACY_passwordWrite returned: %d\n", write_result);

    sleep(10); // Attempt to read the password. If return_buff_len == 0, the password was likely rejected and cleared.

    // read and verify
    char *return_buff = NULL;
    int return_buff_len = 0;
    int read_result =0;

    read_result = read_fn(pathname, id, &return_buff, &return_buff_len); //Call for reading password. If return_buff_len==0 this is mean, password was wrong and was pruned.
    fprintf(stdout,"LEGACY_passwordRead returned len=%d\n", return_buff_len);


    if (return_buff) {
        free(return_buff);
    }

    dlclose(handle);
    if (write_result != 0) {
        fprintf(stderr,"Call Write token error!");
        return 1;
    }
    if (read_result != 0) {
        fprintf(stderr,"Call Read token error!");
        return 1;
    }

    if (return_buff_len == 0) {
        fprintf(stderr,"Login failed! Maybe wrong password.\n");
        return 2;
    } else {
        fprintf(stdout,"Logged in!.\n");
        return 0;
    }
}