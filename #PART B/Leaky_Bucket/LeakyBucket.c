#include <stdio.h>
#include <string.h>

int min(int x, int y) {
    if (x < y)
        return x;
    else
        return y;
}

int main() {
    int drop = 0, mini, nsec, cap, count = 0, i, inp[25], process;

    printf("Enter the bucket size: ");
    scanf("%d", &cap);

    printf("Enter the processing rate: ");
    scanf("%d", &process);

    printf("Enter the number of seconds you want to simulate: ");
    scanf("%d", &nsec);

    for (i = 0; i < nsec; i++) {
        printf("Enter the size of the packet entering at %d sec: ", i + 1);
        scanf("%d", &inp[i]);
    }

    printf("\n Second | Packet Received | Packet Sent | Packet Left | Dropped \n");
    printf("--------------------------------------------------------------------------\n");

    for (i = 0; i < nsec; i++) {
        count += inp[i];
        if (count > cap) {
            drop = count - cap;
            count = cap;
        }
        printf("   %2d   |      %5d      |    %5d    |    %5d    |   %5d\n",
               i + 1, inp[i], min(count, process), count - min(count, process), drop);

        count -= min(count, process);
        drop = 0;
    }

    // Process remaining packets in the bucket
    while (count != 0) {
        if (count > cap) {
            drop = count - cap;
            count = cap;
        }
        printf("   %2d   |      %5d      |    %5d    |    %5d    |   %5d\n",
               i + 1, 0, min(count, process), count - min(count, process), drop);

        count -= min(count, process);
        i++;
        drop = 0;
    }

    return 0;
}
