:- module(hw5, [configuration/3]).
:- [hardware].

section(Sec) :-
        sections(Secs),
        member(Sec, Secs).

is_close_to(Sec1, Sec2) :- adjacent(Sec1, Sec2), !.
is_close_to(Sec1, Sec2) :- adjacent(Sec2, Sec1), !.

does_have_outer_edge(Sec) :-
        aggregate_all(count, adjacent(Sec, _), Count1),
        aggregate_all(count, adjacent(_, Sec), Count2),
        Count is Count1 + Count2,
        Count =\= 4.

check([], _, _, []) :- !.
check([Comp | TailComp], ConstList, StackPlaced, [put(Comp, Sec) | TailPlace]) :-
        section(Sec),
        not(member(put(_, Sec), StackPlaced)),
        (member(outer_edge(Comp), ConstList) ->
		does_have_outer_edge(Sec);
                (((member(close_to(Comp, X), ConstList);
		  member(close_to(X, Comp), ConstList)),
		  member(put(X, S), StackPlaced)) ->
			is_close_to(Sec, S);
			true)),
	check(TailComp, ConstList, [put(Comp, Sec) | StackPlaced], TailPlace).

configuration([], _, []) :- !.
configuration(CompList, ConstList, PlaceList) :-
	sections(Secs),
        length(CompList, X),
        length(Secs, Y),
        X > Y -> fail;
	check(CompList, ConstList, [], PlaceList).


