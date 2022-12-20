
; the finite set of states
#Q = {0,copy,back1,back2,last,fresh,halt_accept}

; the finite set of input symbols
#S = {0,1}

; the complete set of tape symbols
#G = {0,1,_,t,r,u,e,f,a,l,s}

; the start state
#q0 = 0

; the blank symbol
#B = _

; the set of final states
#F = {halt_accept}

; the number of tapes
#N = 2

; the transition functions

; State 0: start state
0 0_ 0_ *r copy
0 1_ 1_ *r copy
0 __ __ ** halt_accept ; empty input

; State cp: copy the string to the 2nd tape 
copy 0_ 00 rr copy
copy 1_ 11 rr copy
copy __ __ ll back1

; State back1: move 1st head to the left
back1 00 00 l* back1
back1 01 01 l* back1
back1 10 10 l* back1
back1 11 11 l* back1
back1 _0 _0 r* last
back1 _1 _1 r* last

; State last: move the last letter to the first place
last *0 00 r* back2
last *1 11 r* back2

; State back2: move 1st head to the left
back2 00 00 *l back2
back2 01 01 *l back2
back2 10 10 *l back2
back2 11 11 *l back2
back2 *_ *_ *r fresh

; State fresh: copy the string to the 1st tape 
fresh 00 00 rr fresh
fresh 01 11 rr fresh
fresh 10 00 rr fresh
fresh 11 11 rr fresh
fresh _* _* ** halt_accept


;ergertgetetewtew
;wetwe
;;;
;
