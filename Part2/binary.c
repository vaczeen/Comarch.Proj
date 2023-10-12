#include <stdio.h>

int main()
{
    int memory[] = {8454151, 9043971, 655361, 16842754, 16842749, 29360128, 25165824, 5, -1, 2};
    int num_values = sizeof(memory) / sizeof(memory[0]);

    printf("Binary values are (32 bits):\n");

    for (int i = 0; i < num_values; i++)
    {
        unsigned int dec = (unsigned int)memory[i];
        unsigned int mask = 1 << 31; // เริ่มตำแหน่งบิตตัวแรก
        for (int j = 0; j < 32; j++)
        {
            int bit = (dec & mask) ? 1 : 0;
            printf("%d", bit);
            mask >>= 1; // เลื่อน mask ไปทางขวา
    
        }
        printf("\n");
    }

    return 0;
}