:- [pl_prover].

% If A is provable then unifies M with
% the corresponding proof term else fails.
provecert_e2172138(A, M) :-
	set_limit(4), my_provecert_iter(A, M);
	fail.

% Performs iterative deepening DFS.
my_provecert_iter(A, M) :-
	set_cnt(0), % Initialize current depth count
	(
		rst_var, my_provecert([], A, M); % Try with the current limit
		inc_limit, get_limit(L), L < 9, % Increase limit and retry
		my_provecert_iter(A, M)
	).

% init rule
my_provecert(G, A, M) :- member(lab(A, M), G).

% Top right rule
my_provecert(_, top, unit).

% Conjunction right rule
my_provecert(G, A & B, pair(M, N)) :- my_provecert(G, A, M), my_provecert(G, B, N).

% Disjunction right rules
my_provecert(G, A + _, inl(M)) :- my_provecert(G, A, M).
my_provecert(G, _ + B, inr(M)) :- my_provecert(G, B, M).

% Implication right rule
my_provecert(G, A => B, fn(U, M)) :- newvar(U), append(G, [lab(A, U)], GN), my_provecert(GN, B, M).

% my_provecert_left(R, E, C, N) attempts to prove
% P, E --> N:C sequent by using left rules for E.
my_provecert(G, C, N) :-
	inc_cnt,
	ctx_pickleft(G, E, R),
	my_provecert_left(R, E, C, N).

% Bot left rule
my_provecert_left(_, lab(bot, M), _, abort(M)).

% Top left rule
my_provecert_left(R, lab(top, _), C, N) :- my_provecert(R, C, N).

% Conjunction left rule
my_provecert_left(R, lab(A & B, M), C, N) :-
	append(R, [lab(A, fst(M)), lab(B, snd(M))], RN), my_provecert(RN, C, N).

% Disjunction left rule
my_provecert_left(R, lab(A + B, M), C, case(M, U, N1, V, N2)) :-
	newvar(U), append(R, [lab(A, U)], RN1), my_provecert(RN1, C, N1),
	newvar(V), append(R, [lab(B, V)], RN2), my_provecert(RN2, C, N2).

% Implication left rule
my_provecert_left(R, lab(A => B, M), C, N) :-
	append(R, [lab(A =>B, M)], RN1), my_provecert(RN1, A, N1),
	append(R, [lab(B, app(M, N1))], RN2), my_provecert(RN2, C, N).

