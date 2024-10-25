#pragma once

#include "io_unit.h"

#define DI_REGISTER_INSTANCE(Type, ...) \
	Type instanceOf##Type = Type(__VA_ARGS__); \
	io::Unit* getUnitOf##Type() { return &instanceOf##Type; }

#define DI_PROVIDE_INSTANCE(Type) \
	io::Unit* getUnitOf##Type()

#define DI_REGISTER_IMPLEMENTATION(Interface, Type, ...) \
  Type implementationOf##Interface = Type(__VA_ARGS__); \
  Interface* getImplementationOf##Interface() {         \
    return &implementationOf##Interface;                \
  };                                                     \
  io::Unit* getUnitOf##Interface() {                      \
	return &implementationOf##Interface;                \
  }

// Macros to inject implementation headers
#define DI_PROVIDE_IMPLEMENTATION(Interface)                  \
  Interface* getImplementationOf##Interface(); \
  io::Unit* getUnitOf##Interface()

#define DI_INJECT(Interface) getImplementationOf##Interface()
#define DI_INJECT_UNIT(Type) getUnitOf##Type()
