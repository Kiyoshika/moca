# The Plan for Implementing Function Calls (Brainstorming)
```text
func(12, x, "hello")
func(50, y, "hello")
```

```text
.section .data
	func.str0: .asciz "hello"
	func.str1: .asciz "hallo"
```

1. verify number of passed parameters meets expected parameter count

2. check if parameter is a variable name
	1.1. if so, fetch value

3. fetch expected parameter type
`function->parameters[i].variable.type`

4. validate passed parameter type meets expected type (mainly string or int)
	3.1. if it's a string literal, move this into .data section
	3.2. inside function_t, record all string literals as an array and when making a function call, check if the string literal already exists and use its index/address from the .data section

5. get variable size in bytes

6. get parameter index - if it's within the first six, move into correct register (rdi, rsi, rdx, etc.)
	5.1. if >= 7, push onto stack
	5.2. the function will have to pop these into their respective rsp locations, e.g. popq -20(%rsp) (or whatever the actual size is) - will have to pop insto a temp register (say, rbx) and move the correct size into the stack position
	5.3. the exception to 5.2 is if it's a string, we will instead leaq X(%rip) into the register/stack

7. make function call
