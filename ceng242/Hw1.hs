module Hw1 where

data Cell = H | P | O | X deriving (Read,Show,Eq)
data Result = Fail | Caught (Int,Int) deriving (Read,Show,Eq)
data Direction = N | S | E | W deriving (Read,Show,Eq)

simulate :: [[Cell]] -> [(Direction, Direction)] -> Result
-- DO NOT CHANGE ABOVE THIS LINE, WRITE YOUR CODE BELOW --

move :: (Int, Int) -> [(Int, Int)] -> Direction -> (Int, Int)
move (x, y) available_coordinates direction
	| direction == N = if elem (x, y - 1) available_coordinates then (x, y - 1) else (x, y)
	| direction == E = if elem (x + 1, y) available_coordinates then (x + 1, y) else (x, y)
	| direction == S = if elem (x, y + 1) available_coordinates then (x, y + 1) else (x, y)
	| direction == W = if elem (x - 1, y) available_coordinates then (x - 1, y) else (x, y)

generate_available_coordinates :: [[Cell]] -> [(Int, Int)] -> Int -> Int -> [(Int, Int)]
generate_available_coordinates [] available_coordinates x y = available_coordinates
generate_available_coordinates ((X:[]):c) available_coordinates x y = generate_available_coordinates c available_coordinates 0 (y + 1)
generate_available_coordinates ((X:b):c) available_coordinates x y = generate_available_coordinates (b:c) available_coordinates (x + 1) y
generate_available_coordinates ((_:[]):c) available_coordinates x y = generate_available_coordinates c ((x, y):available_coordinates) 0 (y + 1)
generate_available_coordinates ((_:b):c) available_coordinates x y = generate_available_coordinates (b:c) ((x, y):available_coordinates) (x + 1) y

start :: (Int, Int) -> (Int, Int) -> [(Int, Int)] -> [(Direction, Direction)] -> Result
start h_coordinates p_coordinates [] directions
	| h_coordinates == p_coordinates = Caught h_coordinates
	| otherwise = Fail
start h_coordinates p_coordinates available_coordinates []
	| h_coordinates == p_coordinates = Caught h_coordinates
	| otherwise = Fail
start h_coordinates p_coordinates available_coordinates ((h_dir, p_dir):rest_dir)
	| h_coordinates == p_coordinates = Caught h_coordinates
	| otherwise = start (move h_coordinates available_coordinates h_dir) (move p_coordinates available_coordinates p_dir) available_coordinates rest_dir

find_coordinates :: Int -> Int -> Cell -> [[Cell]] -> (Int, Int)
find_coordinates x y h_or_p [] = (-1, -1)
find_coordinates x y h_or_p ((a:[]):c)
	| h_or_p == a = (x, y)
	| otherwise = find_coordinates 0 (y + 1) h_or_p c
find_coordinates x y h_or_p ((a:b):c)
	| h_or_p == a = (x, y)
	| otherwise = find_coordinates (x + 1) y h_or_p (b:c)

simulate environment directions = start (find_coordinates 0 0 H environment) (find_coordinates 0 0 P environment) (generate_available_coordinates environment [] 0 0) directions