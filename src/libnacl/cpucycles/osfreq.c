static double osfreq(void)
{
  FILE *f;
  double result;
  int s;

  f = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r");
  if (f) {
    s = fscanf(f,"%lf",&result);
    fclose(f);
    if (s > 0) return 1000.0 * result;
  }

  f = fopen("/sys/devices/system/cpu/cpu0/clock_tick", "r");
  if (f) {
    s = fscanf(f,"%lf",&result);
    fclose(f);
    if (s > 0) return result;
  }

  f = fopen("/proc/cpuinfo","r");
  if (f) {
    for (;;) {
      s = fscanf(f,"cpu MHz : %lf",&result);
      if (s > 0) break;
      if (s == 0) s = fscanf(f,"%*[^\n]\n");
      if (s < 0) { result = 0; break; }
    }
    fclose(f);
    if (result) return 1000000.0 * result;
  }

  f = fopen("/proc/cpuinfo","r");
  if (f) {
    for (;;) {
      s = fscanf(f,"clock : %lf",&result);
      if (s > 0) break;
      if (s == 0) s = fscanf(f,"%*[^\n]\n");
      if (s < 0) { result = 0; break; }
    }
    fclose(f);
    if (result) return 1000000.0 * result;
  }

  f = popen("/usr/sbin/lsattr -E -l proc0 -a frequency 2>/dev/null","r");
  if (f) {
    s = fscanf(f,"frequency %lf",&result);
    pclose(f);
    if (s > 0) return result;
  }

  f = popen("/usr/sbin/psrinfo -v 2>/dev/null","r");
  if (f) {
    for (;;) {
      s = fscanf(f," The %*s processor operates at %lf MHz",&result);
      if (s > 0) break;
      if (s == 0) s = fscanf(f,"%*[^\n]\n");
      if (s < 0) { result = 0; break; }
    }
    pclose(f);
    if (result) return 1000000.0 * result;
  }

  return 0;
}
