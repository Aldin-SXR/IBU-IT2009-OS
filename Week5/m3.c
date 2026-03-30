#include <stdio.h>
#include <string.h>

int main() {
    struct {
        int nums[10];
        int is_admin;
    } user;

    user.is_admin = 0;
    // writing past the end of the array
    user.nums[10] = 1;

    printf("is_admin = %d\n", user.is_admin);

    if (user.is_admin != 0) {
        printf("Access granted!\n");
    } else {
        printf("Access denied.\n");
    }

    return 0;
}