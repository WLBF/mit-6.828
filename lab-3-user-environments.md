# Lab 3: User Environments

**Exercise 4.**

**Questions**

1. What is the purpose of having an individual handler function for each exception/interrupt? (i.e., if all exceptions/interrupts were delivered to the same handler, what feature that exists in the current implementation could not be provided?)

   It is hard to determine whether have error code on the stack if all interrupts are delivered to same handler.

2. Did you have to do anything to make the `user/softint` program behave correctly? The grade script expects it to produce a general protection fault (trap 13), but `softint`'s code says `int $14`. *Why* should this produce interrupt vector 13? What happens if the kernel actually allows `softint`'s `int $14` instruction to invoke the kernel's page fault handler (which is interrupt vector 14)?

   We need to do nothing to make `user/softint` program behave correctly. The produce interrupt vector 13 because `int $14` violate protection level check. If kernel allows `softint` invoke the  page fault handler, it will cause program terminated and  buggy or malicious user code could compromise the kernel.

**Exercise 6**

**Questions**

1. The break point test case will either generate a break point exception or a general protection fault depending on how you initialized the break point entry in the IDT (i.e., your call to `SETGATE` from `trap_init`). Why? How do you need to set it up in order to get the breakpoint exception to work as specified above and what incorrect setup would cause it to trigger a general protection fault?

   Gate's DPL bits shoud be set to 3, so the interrupt from user code be able call the handler.

2. What do you think is the point of these mechanisms, particularly in light of what the `user/softint` test program does?

   Prevent buggy or malicious user code compromise the kernel.

