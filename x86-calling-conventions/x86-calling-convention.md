# Calling Convention

Note about x86 assembly calling convention.

References：

[http://www.cs.virginia.edu/~evans/cs216/guides/x86.html](http://www.cs.virginia.edu/~evans/cs216/guides/x86.html)
[https://en.wikibooks.org/wiki/X86_Assembly](https://en.wikibooks.org/wiki/X86_Assembly)

![](x86-registers.png)

![](stack-convention.png)

## Make a subrouting call

1. Before calling a subrouting, save the *caller-saved* registers (`EAX, ECX, EDX`).

2. Push the parameters onto the stack.

3. Use `call` instruction, places the retrun address onto stack, and branches to the subroutine code.

## Callee prologue

1. Push old base pointer value, for restore it after subroutine.

``` asm
push ebp
mov ebp, esp
```

2. Allocate local variables by make space on the stack. e.g.

```asm
sub esp, 12
```

3. Save the values of the *callee-saved* register push them onto stack (`EBX, EDI, ESI`).

## Callee body proceed

## Callee epilogue

1. Leave the return value in `EAX`.

2. Restore the old values of any *callee-saved* registers (`EDI, ESI, EBX`).

3. Deallocate local variables.

```asm
mov esp, ebp
```

4. Restore the caller's base pointer value by poping `EBP` from the stack.

5. Return to the caller by executing a return `ret` instruction, find and reomve the *return address* from the stack.

## After subroutine return

1. Remove the parameters from stack.

2. Restore the contents of *caller-saved* registers (`EAX, ECX, EDX`)