# Usage

```c
#include <sodium.h>

int main(void)
{
    if (sodium_init() == -1) {
        return -1;
    }
    ...
}
```

`sodium.h` is the only header that has to be included.

The library is called `sodium` (use `-lsodium` to link it) and proper compilation/linker flags can be obtained using `pkg-config` on systems where it is available:

```bash
CFLAGS=$(pkg-config --cflags libsodium)
LDFLAGS=$(pkg-config --libs libsodium)
```

`sodium_init()` initializes the library and should be called before any other function provided by Sodium.
The function can be called more than once, but should not be evaluated by multiple threads simultaneously. Add appropriate locks around the function call if this scenario can happen in your application.

After this function returns, all the other functions provided by Sodium will be thread-safe.

`sodium_init()` doesn't perform any memory allocations. However, on Unix systems, it opens `/dev/urandom`, and keeps the descriptor open so that the device remains accessible after a `chroot()` call.
Multiple calls to `sodium_init()` do not cause additional descriptors to be opened.



