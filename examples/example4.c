int main()
{

    int a, b, x, y, z = 0; // assume N is an input value
    int N;
    int i = 0;

    while (i < N)
    {
        x = -((x + 2 * y * 3 * z) % 3);

        y = (3 * x + 2 * y + z) % 11;

        z++;
    }
}
