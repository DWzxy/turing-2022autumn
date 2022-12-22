; This example program checks if the input string is a binary palindrome.
; Input: a string of 0's and 1's, e.g. '1001001'

; the finite set of states
#Q = {0,plus2,plus3,plus3_1,plus3_2,back3,back1,cmp13,accept,accept2,accept3,accept4,halt_accept,reject,reject2,reject3,reject4,reject5,halt_reject}

; the finite set of input symbols
#S = {1}

; the complete set of tape symbols
#G = {1,_,t,r,u,e,f,a,l,s}

; the start state
#q0 = 0

; the blank symbol
#B = _

; the set of final states
#F = {halt_accept}

; the number of tapes
#N = 3
;the 1st 
;the 2nd is a 1string length i
;the 3rd is a 1string length i^2
;tip: (n+1)^2=n^2=2n+1

; the transition functions

; State 0: start state
0 *** *** *** cmp13

; State plus2: 
plus2 *1* *1* *r* plus2
plus2 *_* *1* *** cmp13

;State plus3_1: i^2+=i
plus3_1 *1_ *11 *lr plus3_1
plus3_1 *_* *_* *r* plus3_2

;State plus3_2: i^2+=i
plus3_2 *1_ *11 *rr plus3_2
plus3_2 *_* *_* *l* plus3

; State plus3: 
plus3 **_ **1 *** back3

;State back3: move 3 to the left
back3 **1 **1 **l back3
back3 **_ **_ **r back1

;State back1: move 1 to the left
back1 1** 1** l** back1
back1 _** _** r** plus2

; State cmp13: compare if 13 have the same length
cmp13 1*1 1*1 r*r cmp13
cmp13 _*1 _*1 l** reject
cmp13 1*_ 1*_ *** plus3_1 ; can continue
cmp13 _*_ _*_ l** accept

; State accept*: write 'true' on 1st tape
accept 1** _** l** accept
accept _** t** r** accept2
accept2 _** r** r** accept3
accept3 _** u** r** accept4
accept4 _** e** *** halt_accept

; State reject*: write 'false' on 1st tape
reject 1** _** l** reject
reject _** f** r** reject2
reject2 _** a** r** reject3
reject3 _** l** r** reject4
reject4 _** s** r** reject5
reject5 _** e** r** halt_reject
