# The SBas Language

SBas is a really simple language targeting Linux x86-64.
It takes a user defined function - given at a text file - and
JIT compiles it to `mmap`ed area of the main process's virtual memory,
allowing an arbitrary number of executions for the lifetime of the program.

After successful compilation, the mapped memory region is W^X protected,
preventing unauthorized and accidental writes to it.

## Syntax
### Variables and assignment
SBas features exactly five local variables named `v1` through `v5`:
```
var := <v1 | v2 | v3 | v4 | v5>
```
These can be assigned values with the following syntax:
```
assign:= var : <pY | var | $snum>
```

where:
- `pY` are parameters passed to the function following the System V ABI. 
Currently, SBas supports only 3 parameters (`p1`, `p2`, and `p3`).
- `snum` is a 32-bit signed integer.

### Expressions
An SBas expression is the 2-arity function that modifies a local variable:
```
expr:= var = <var | $snum> op <var | $snum>
```

where `op` is `< + | - | * >`.

### Conditionals
In order to alter control flow, SBas features the `iflez` construct, short
for *IF Less than or Equal to Zero* in the form:
```
dif:= iflez var num
```

where `num` is the non-zero, positive 32-bit integer of the **line in the source file**
to jump to.

### Return
Finally, every SBas function **must** return some value, even if unused by the caller.
A return is the expression:
```
return := ret <var | $snum>
```

## Usage
Run the `release` Makefile task to compile the compiler. By default, it will be saved
to `/tmp/sbas`. Then, you can JIT compile the SBas function passing up to three
arguments to it.
```
make release
/tmp/sbas </path/to/source> <param1> <param2> <param3>
```
The `sbas` binary will compile **and** execute your declared function.
