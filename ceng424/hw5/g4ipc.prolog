:- [pl_prover].

% If A is provable then unifies M with
% the corresponding proof term else fails.
g4ipc_e2172138(A, M) :-
	rst_var, my_g4ipc([], A, M);
	fail.

% init rule
my_g4ipc(G, A, M) :- atomic(A), member(lab(A, M), G).

% Top right rule
my_g4ipc(_, top, unit).

% Conjunction right rule
my_g4ipc(G, A & B, pair(M, N)) :- my_g4ipc(G, A, M), my_g4ipc(G, B, N).

% Disjunction right rules
my_g4ipc(G, A + _, inl(M)) :- my_g4ipc(G, A, M).
my_g4ipc(G, _ + B, inr(M)) :- my_g4ipc(G, B, M).

% Implication right rule
my_g4ipc(G, A => B, fn(U, M)) :- newvar(U), append(G, [lab(A, U)], GN), my_g4ipc(GN, B, M).

% my_g4ipc_left(R, E, C, N) attempts to prove
% E, R --> N:C sequent by using left rules for E.
my_g4ipc(G, C, N) :-
	ctx_pickleft(G, E, R),
	my_g4ipc_left(R, E, C, N).

% Bot left rule
my_g4ipc_left(_, lab(bot, M), _, abort(M)).

% Top left rule
my_g4ipc_left(R, lab(top, _), C, N) :- my_g4ipc(R, C, N).

% Conjunction left rule
my_g4ipc_left(R, lab(A & B, M), C, N) :-
	append(R, [lab(A, fst(M)), lab(B, snd(M))], RN), my_g4ipc(RN, C, N).

% Disjunction left rule
my_g4ipc_left(R, lab(A + B, M), C, case(M, U, N1, V, N2)) :-
	newvar(U), append(R, [lab(A, U)], RN1), my_g4ipc(RN1, C, N1),
	newvar(V), append(R, [lab(B, V)], RN2), my_g4ipc(RN2, C, N2).

% P=>L rule
my_g4ipc_left(R, lab(A => B, M), C, N) :-
	atomic(A), member(lab(A, N1), R),
	append(R, [lab(B, app(M, N1))], RN), my_g4ipc(RN, C, N).

% &=>L rule
my_g4ipc_left(R, lab((A1 & A2) => B, M), C, N) :-
	newvar(U), newvar(V),
	append(R, [lab(A1 => (A2 => B), fn(U, fn(V, app(M, pair(U, V)))))], RN), my_g4ipc(RN, C, N).

% Top=>L rule
my_g4ipc_left(R, lab(top => B, M), C, N) :-
       append(R, [lab(B, app(M, unit))], RN), my_g4ipc(RN, C, N).

% +=>L rule
my_g4ipc_left(R, lab((A1 + A2) => B, M), C, N) :-
	newvar(U), newvar(V),
	append(R, [lab(A1 => B, fn(U, app(M, inl(U)))), lab(A2 => B, fn(V, app(M, inr(V))))], RN), my_g4ipc(RN, C, N).

% Bot=>L rule
my_g4ipc_left(R, lab(top => _, _), C, N) :- my_g4ipc(R, C, N).

% =>=>L rule
my_g4ipc_left(R, lab((A1 => A2) => B, M), C, N) :-
	newvar(U), newvar(V),
	append(R, [lab(A2 => B, fn(U, app(M, fn(V, U)))), lab(A1, V)], RN1), my_g4ipc(RN1, A2, N1),
	append(R, [lab(B, app(M, fn(V, N1)))], RN2), my_g4ipc(RN2, C, N).


