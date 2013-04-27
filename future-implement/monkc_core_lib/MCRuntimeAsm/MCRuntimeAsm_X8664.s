/*
Copyright (c) <2013>, <Sun Yuli>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <Monk-C> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#;System V AMD64 ABI

#;integer parameters will pass on these registers
#;define int_arg1 %rdi
#;define int_arg2 %rsi
#;define int_arg3 %rdx
#;define int_arg4 %rcx
#;define int_arg5 %r8
#;define int_arg6 %r9

#;registers must preserved across function calls(without stack pointer %rbp %rsp)
#;define preserved_reg1 %rbx
#;define preserved_reg2 %r12
#;define preserved_reg3 %r13
#;define preserved_reg4 %r14
#;define preserved_reg5 %r15

#;void* _ff(id const obj, const unsigned hashkey, ...);
#;void* _resolve_method(id const obj, const unsigned hashkey);

.text
.globl _ff
.p2align 4, 0x90 #;16 byte align, X86_64 will round up arguments to 8 byte
_ff:
	#;we can get the argument by esp+4n at the very first
	#;caller of _ff() will prepare the argumnets in reverse order
	#;here is the meaning of c-language arguments copy-in-copy-out

	#;save stack registers
	pushq %rbp
	movq %rsp, %rbp
	#;va arguments function call need the %rax to be the count of SSE(float, double) type argument
	pushq %rax
	#;save the parameters
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	pushq %r9
	#;pass parameters via %rdi %rsi
	#;both pointer and int type are machine type INTEGER
	#;the _resolve_method deal with no floating point data. so no need to save xmm0~xmm15
	#;return value will in the %rax
	call __response_to
	movq %rax, %r10
	#;restore parameters
	popq %r9
	popq %r8
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi
	#;restore count of float
	popq %rax
	#;restore stack registers
	movq %rbp, %rsp
	popq %rbp
	#;confirm return address not nil
	cmpq $0, %r10
	je 0f
	#;jump to the method with current stack frame. and did not return back here.
	#;this code made the method() call just like the ff() call. it takes what arguments
	#;ff() take. and return values at where ff() called. 
	#;stack frame of method() is prepared by caller of ff() and cleaned by it.
	jmp	*%r10
0:
	ret

#;void* _push_jump(id const obj, void* addr, ...);

.text
.globl _push_jump
.p2align 4, 0x90
_push_jump:
	cmpq $0, %rsi		#; confirm return address not nil
	je 0f
	jmp *%rsi
0:
	ret

#;void* _clean_jump2(id const obj, void* addr, ...);

.text
.globl _clean_jump1
.p2align 4, 0x90
_clean_jump1:
	movq %rbp, %rsp			#; unwind the current start frame
	popq %rbp				#;
	cmpq $0, %rsi		
	je 0f
	jmp *%rsi
0:
	ret


.text
.globl _clean_jump2
.p2align 4, 0x90
_clean_jump2:
	movq %rbp, %rsp			#; unwind the current start frame
	popq %rbp				#;
	cmpq $0, %rsi		
	je 0f
	jmp *%rsi
0:
	ret


.text
.globl _clean_jump3
.p2align 4, 0x90
_clean_jump3:
	movq %rbp, %rsp			#; unwind the current start frame
	popq %rbp				#;
	cmpq $0, %rsi		
	je 0f
	jmp *%rsi
0:
	ret


.text
.globl _clean_jump4
.p2align 4, 0x90
_clean_jump4:
	movq %rbp, %rsp			#; unwind the current start frame
	popq %rbp				#;
	cmpq $0, %rsi		
	je 0f
	jmp *%rsi
0:
	ret


.text
.globl	mc_compareAndSwapInteger
.p2align 4, 0x90
mc_compareAndSwapInteger:
	movq %rsi, %rax
	lock cmpxchgq %rdx, (%rdi)
	xorq %rax, %rax
	jne	0f

	movq $0, %rax
	ret
0:
	movq $-1, %rax
	ret


.text
.globl	mc_compareAndSwapPointer
.p2align 4, 0x90
mc_compareAndSwapPointer:
	movq %rsi, %rax
	lock cmpxchgq %rdx, (%rdi)
	xorq %rax, %rax
	jne	0f

	movq $0, %rax
	ret
0:
	movq $-1, %rax
	ret