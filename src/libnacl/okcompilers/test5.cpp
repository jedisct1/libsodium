extern "C" {
  extern int not3(int);
  extern int bytes(int);
  extern long long shr32(long long);
  extern double double5(void);
}

int main(int argc,char **argv)
{
  if (not3(3)) return 100;

  /* on ppc32, gcc -mpowerpc64 produces SIGILL for >>32 */
  if (!not3(shr32(1))) return 100;

  /* on pentium 1, gcc -march=pentium2 produces SIGILL for (...+7)/8 */
  if (bytes(not3(1)) != 1) return 100;

  /* on pentium 1, gcc -march=prescott produces SIGILL for double comparison */
  if (double5() < 0) return 100;

  return 0;
}
