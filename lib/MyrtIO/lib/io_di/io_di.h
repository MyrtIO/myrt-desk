#pragma once

#define IO_PROVIDE_INSTANCE(I, T) \
T instanceOf##T; \
I* getImplementationOf##I() { \
  return &instanceOf##T; \
}

#define IO_PROVIDE(I, T) \
extern T instanceOf##T; \
I* getImplementationOf##I()

#define IO_INJECT(I) getImplementationOf##I()

#define IO_INJECT_INSTANCE(T) &instanceOf##T
