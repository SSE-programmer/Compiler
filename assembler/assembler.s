.code32

.bss
k:
		.space 4

l:
		.space 4

n:
		.space 4

.text
.globl main
.type main, @function
main:
	pushl %ebp
	movl %esp, %ebp

	xorl %eax, %eax	/*Начало программы вывода параметра writeln*/
/*Константу-строку пушим в стек*/
	pushl  $73
	pushl $73
	pushl $string_format_symbol
	call printf
	pushl $110
	pushl $string_format_symbol
	call printf
	pushl $112
	pushl $string_format_symbol
	call printf
	pushl $117
	pushl $string_format_symbol
	call printf
	pushl $116
	pushl $string_format_symbol
	call printf
	pushl $32
	pushl $string_format_symbol
	call printf
	pushl $116
	pushl $string_format_symbol
	call printf
	pushl $119
	pushl $string_format_symbol
	call printf
	pushl $111
	pushl $string_format_symbol
	call printf
	pushl $32
	pushl $string_format_symbol
	call printf
	pushl $110
	pushl $string_format_symbol
	call printf
	pushl $117
	pushl $string_format_symbol
	call printf
	pushl $109
	pushl $string_format_symbol
	call printf
	pushl $98
	pushl $string_format_symbol
	call printf
	pushl $101
	pushl $string_format_symbol
	call printf
	pushl $114
	pushl $string_format_symbol
	call printf
	pushl $115
	pushl $string_format_symbol
	call printf
	pushl $58
	pushl $string_format_symbol
	call printf
	pushl $32
	pushl $string_format_symbol
	call printf
	leal k, %eax
	pushl %eax
	pushl $decimal_format
	call scanf
	call getchar
	leal l, %eax
	pushl %eax
	pushl $decimal_format
	call scanf
	call getchar
	jmp check0
loop_start0:
	pushl  k
	pushl  l
	popl %ebx
	popl %eax
	cmp %ebx, %eax
	movl $1, %eax
	jge LARGER_OR_EQUAL0
	movl $0, %eax
LARGER_OR_EQUAL0:
	pushl %eax
	popl %eax
	cmpl $0, %eax
	jg if_body0
	pushl  l
	pushl  k
	popl %ebx
	popl %eax
	movl %eax, %edx
	shr $16, %edx
	idiv %ebx
	and $0xFF, %edx
	pushl %edx
	popl  %eax
	movl  %eax, l
	jmp continue0
if_body0:
	pushl  k
	pushl  l
	popl %ebx
	popl %eax
	movl %eax, %edx
	shr $16, %edx
	idiv %ebx
	and $0xFF, %edx
	pushl %edx
	popl  %eax
	movl  %eax, k
continue0:
check0:
	pushl  k
	pushl  $0
	popl %ebx
	popl %eax
	cmp %ebx, %eax
	movl $1, %eax
	jne NOT_EQUAL0
	movl $0, %eax
NOT_EQUAL0:
	pushl %eax
	pushl  l
	pushl  $0
	popl %ebx
	popl %eax
	cmp %ebx, %eax
	movl $1, %eax
	jne NOT_EQUAL1
	movl $0, %eax
NOT_EQUAL1:
	pushl %eax
	popl %ebx
	popl %eax
	cmp $0, %eax
	movl $0, %eax
	jle AND0_1_not_true
	movl $1, %eax
AND0_1_not_true:
	cmp $0, %ebx
	movl $0, %ebx
	jle AND0_2_not_true
	movl $1, %ebx
AND0_2_not_true:
	andl %ebx, %eax
	pushl %eax
	popl %eax
	cmpl $0, %eax
	jg loop_start0
	pushl  k
	pushl  l
	popl %edx
	popl %eax
	addl %edx, %eax
	pushl %eax
	popl  %eax
	movl  %eax, n
	xorl %eax, %eax	/*Начало программы вывода параметра writeln*/
/*Константу-строку пушим в стек*/
	pushl  $78
	pushl $78
	pushl $string_format_symbol
	call printf
	pushl $79
	pushl $string_format_symbol
	call printf
	pushl $68
	pushl $string_format_symbol
	call printf
	pushl $32
	pushl $string_format_symbol
	call printf
	pushl $61
	pushl $string_format_symbol
	call printf
	pushl $32
	pushl $string_format_symbol
	call printf
	xorl %eax, %eax	/*Начало программы вывода параметра writeln*/
	pushl  n
	popl %eax
	pushl %eax
	pushl $decimal_format_nl
	call printf
	addl $8, %esp
	movl $0, %eax
	leave
	ret

.section .rodata
decimal_format:
	.string "%d"

decimal_format_nl:
	.string "%d\n"

string_format:
	.string "%s"

string_format_read:
	.string "%255[^\r\n]"

string_format_symbol:
	.string "%c"

string_format_symbol_nl:
	.string "%c\n"

string_format_nl:
	.string "%s\n"

format_nl:
	.string "\n"

