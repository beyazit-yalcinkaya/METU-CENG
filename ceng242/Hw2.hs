module Hw2 where

import Data.List -- YOU MAY USE THIS MODULE FOR SORTING THE AGENTS

data Level = Newbie | Intermediate | Expert deriving (Enum, Eq, Ord, Show, Read)
data Hunter = Hunter {hID::Int, hlevel::Level, hEnergy::Int, hNumberOfCatches::Int, hActions::[Direction]} deriving (Eq, Show, Read)
data Prey = Prey {pID::Int, pEnergy::Int, pActions::[Direction]} deriving (Eq, Show, Read)
data Cell = O | X | H Hunter | P Prey | T deriving (Eq, Show, Read)
data Direction = N | S | E | W deriving (Eq, Show, Read)
type Coordinate = (Int, Int)
-- DO NOT CHANGE THE DEFINITIONS ABOVE. --


-- INSTANCES OF Ord FOR SORTING, UNCOMMENT AND COMPLETE THE IMPLEMENTATIONS --


instance Ord Hunter where
    compare (Hunter id1 level1 energy1 catches1 _) (Hunter id2 level2 energy2 catches2 _) =
        if level1 /= level2
            then compare level1 level2
            else if energy1 /= energy2
                then compare energy1 energy2
                else if catches1 /= catches2
                    then compare catches1 catches2
                    else compare id2 id1

instance Ord Prey where
    compare (Prey id1 energy1 _) (Prey id2 energy2 _) =
        if energy1 /= energy2
            then compare energy1 energy2
            else compare id2 id1

-- WRITE THE REST OF YOUR CODE HERE --


hmove :: (Hunter, Coordinate) -> ([Coordinate], [Coordinate]) -> (Hunter, Coordinate) 
hmove (hunter, (x, y)) (available_coordinates, traps)
    | head (hActions hunter) == N =
        if elem (x, y - 1) available_coordinates || elem (x, y - 1) traps
            then (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) - 1 in if temp > 0 then temp else 0) (hNumberOfCatches hunter) (tail (hActions hunter)), (x, y - 1))
            else (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) - 1 in if temp > 0 then temp else 0) (hNumberOfCatches hunter) (tail (hActions hunter)), (x, y))
    | head (hActions hunter) == E =
        if elem (x + 1, y) available_coordinates || elem (x + 1, y) traps
            then (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) - 1 in if temp > 0 then temp else 0) (hNumberOfCatches hunter) (tail (hActions hunter)), (x + 1, y))
            else (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) - 1 in if temp > 0 then temp else 0) (hNumberOfCatches hunter) (tail (hActions hunter)), (x, y))
    | head (hActions hunter) == S =
        if elem (x, y + 1) available_coordinates || elem (x, y + 1) traps
            then (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) - 1 in if temp > 0 then temp else 0) (hNumberOfCatches hunter) (tail (hActions hunter)), (x, y + 1))
            else (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) - 1 in if temp > 0 then temp else 0) (hNumberOfCatches hunter) (tail (hActions hunter)), (x, y))
    | head (hActions hunter) == W =
        if elem (x - 1, y) available_coordinates || elem (x - 1, y) traps
            then (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) - 1 in if temp > 0 then temp else 0) (hNumberOfCatches hunter) (tail (hActions hunter)), (x - 1, y))
            else (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) - 1 in if temp > 0 then temp else 0) (hNumberOfCatches hunter) (tail (hActions hunter)), (x, y))

pmove :: (Prey, Coordinate) -> ([Coordinate], [Coordinate]) -> (Prey, Coordinate) 
pmove (prey, (x, y)) (available_coordinates, traps)
    | head (pActions prey) == N =
        if elem (x, y - 1) available_coordinates
            then (Prey (pID prey) (let temp = (pEnergy prey) - 1 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y - 1))
            else if elem (x, y - 1) traps
                then (Prey (pID prey) (let temp = (pEnergy prey) - 11 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y - 1))
                else if elem (x, y) traps
                    then (Prey (pID prey) (let temp = (pEnergy prey) - 11 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y))
                    else (Prey (pID prey) (let temp = (pEnergy prey) - 1 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y))
    | head (pActions prey) == E =
        if elem (x + 1, y) available_coordinates
            then (Prey (pID prey) (let temp = (pEnergy prey) - 1 in if temp > 0 then temp else 0) (tail (pActions prey)), (x + 1, y))
            else if elem (x + 1, y) traps
                then (Prey (pID prey) (let temp = (pEnergy prey) - 11 in if temp > 0 then temp else 0) (tail (pActions prey)), (x + 1, y))
                else if elem (x, y) traps
                    then (Prey (pID prey) (let temp = (pEnergy prey) - 11 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y))
                    else (Prey (pID prey) (let temp = (pEnergy prey) - 1 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y))
    | head (pActions prey) == S =
        if elem (x, y + 1) available_coordinates
            then (Prey (pID prey) (let temp = (pEnergy prey) - 1 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y + 1))
            else if elem (x, y + 1) traps
                then (Prey (pID prey) (let temp = (pEnergy prey) - 11 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y + 1))
                else if elem (x, y) traps
                    then (Prey (pID prey) (let temp = (pEnergy prey) - 11 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y))
                    else (Prey (pID prey) (let temp = (pEnergy prey) - 1 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y))
    | head (pActions prey) == W =
        if elem (x - 1, y) available_coordinates
            then (Prey (pID prey) (let temp = (pEnergy prey) - 1 in if temp > 0 then temp else 0) (tail (pActions prey)), (x - 1, y))
            else if elem (x - 1, y) traps
                then (Prey (pID prey) (let temp = (pEnergy prey) - 11 in if temp > 0 then temp else 0) (tail (pActions prey)), (x - 1, y))
                else if elem (x, y) traps
                    then (Prey (pID prey) (let temp = (pEnergy prey) - 11 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y))
                    else (Prey (pID prey) (let temp = (pEnergy prey) - 1 in if temp > 0 then temp else 0) (tail (pActions prey)), (x, y))

hunter_move :: ([Coordinate], [Coordinate]) -> (Hunter, Coordinate) -> (Hunter, Coordinate) 
hunter_move (available_coordinates, traps) (hunter, (x, y))
    | hEnergy hunter >= 10 = hmove (hunter, (x, y)) (available_coordinates, traps)
    | hEnergy hunter < 10 = (Hunter (hID hunter) (hlevel hunter) ((hEnergy hunter) + 1) (hNumberOfCatches hunter) (tail (hActions hunter)), (x, y))

prey_move :: ([Coordinate], [Coordinate]) -> (Prey, Coordinate) -> (Prey, Coordinate) 
prey_move (available_coordinates, traps) (prey, (x, y))
    | pEnergy prey >= 10 = pmove (prey, (x, y)) (available_coordinates, traps)
    | pEnergy prey < 10 = (Prey (pID prey) (if elem (x, y) traps then 0 else (pEnergy prey) + 1) (tail (pActions prey)), (x, y))

generate_available_coordinates_and_traps :: [[Cell]] -> ([Coordinate], [Coordinate]) -> Int -> Int -> ([Coordinate], [Coordinate])
generate_available_coordinates_and_traps [] (available_coordinates, traps) x y = (available_coordinates, traps)
generate_available_coordinates_and_traps ((X:[]):c) (available_coordinates, traps) x y = generate_available_coordinates_and_traps c (available_coordinates, traps) 0 (y + 1)
generate_available_coordinates_and_traps ((X:b):c) (available_coordinates, traps) x y = generate_available_coordinates_and_traps (b:c) (available_coordinates, traps) (x + 1) y
generate_available_coordinates_and_traps ((T:[]):c) (available_coordinates, traps) x y = generate_available_coordinates_and_traps c (available_coordinates, ((x, y):traps)) 0 (y + 1)
generate_available_coordinates_and_traps ((T:b):c) (available_coordinates, traps) x y = generate_available_coordinates_and_traps (b:c) (available_coordinates, ((x, y):traps)) (x + 1) y
generate_available_coordinates_and_traps ((_:[]):c) (available_coordinates, traps) x y = generate_available_coordinates_and_traps c (((x, y):available_coordinates), traps) 0 (y + 1)
generate_available_coordinates_and_traps ((_:b):c) (available_coordinates, traps) x y = generate_available_coordinates_and_traps (b:c) (((x, y):available_coordinates), traps) (x + 1) y

find_hunters :: [[Cell]] -> [(Hunter, Coordinate)] -> Int -> Int -> [(Hunter, Coordinate)]
find_hunters [] hunters x y = mysort hunters
find_hunters ((H h:[]):c) hunters x y = find_hunters c ((h, (x, y)):hunters) 0 (y + 1)
find_hunters ((H h:b):c) hunters x y = find_hunters (b:c) ((h, (x, y)):hunters) (x + 1) y
find_hunters ((_:[]):c) hunters x y = find_hunters c hunters 0 (y + 1)
find_hunters ((_:b):c) hunters x y = find_hunters (b:c) hunters (x + 1) y

find_preys :: [[Cell]] -> [(Prey, Coordinate)] -> Int -> Int -> [(Prey, Coordinate)]
find_preys [] preys x y = mysort preys
find_preys ((P p:[]):c) preys x y = find_preys c ((p, (x, y)):preys) 0 (y + 1)
find_preys ((P p:b):c) preys x y = find_preys (b:c) ((p, (x, y)):preys) (x + 1) y
find_preys ((_:[]):c) preys x y = find_preys c preys 0 (y + 1)
find_preys ((_:b):c) preys x y = find_preys (b:c) preys (x + 1) y

mysort :: Ord a => [(a, b)] -> [(a, b)]
mysort l = sortBy (\x y -> compare (fst x) (fst y)) l

mydelete :: (Hunter, Coordinate) -> ([(Hunter, Coordinate)], [(Prey, Coordinate)]) -> ([(Hunter, Coordinate)], [(Prey, Coordinate)])
mydelete (hunter, (x, y)) (hunters, preys) = let new_prey = let f a b = snd a == snd b in deleteBy f (fst (preys!!0), (x, y)) (deleteBy f (fst (preys!!0), (x, y)) preys) in (find_and_replace (hunter, (x, y)) (Hunter (hID hunter) (hlevel hunter) (let temp = (hEnergy hunter) + ((length preys) - (length new_prey)) * 20 in if temp > 100 then 100 else temp) ((hNumberOfCatches hunter) + ((length preys) - (length new_prey))) (hActions hunter), (x, y)) hunters, new_prey)


find_and_replace :: (Hunter, Coordinate) -> (Hunter, Coordinate) -> [(Hunter, Coordinate)] -> [(Hunter, Coordinate)] 
find_and_replace a b (c:d) = if a == c then b:d else c:(find_and_replace a b d)

check :: ([(Hunter, Coordinate)], [(Prey, Coordinate)]) -> ([(Hunter, Coordinate)], [(Prey, Coordinate)])
check (hunters, preys) = foldr mydelete (hunters, preys) hunters

start :: ([Coordinate], [Coordinate]) -> ([(Hunter, Coordinate)], [(Prey, Coordinate)]) -> ([(Hunter, Coordinate)], [(Prey, Coordinate)])
start (available_coordinates, traps) (hunters, preys) =
    if (hActions (fst (hunters !! 0))) == [] || preys == []
        then (reverse (mysort hunters), mysort preys)
        else start (available_coordinates, traps) (check (mysort (map (hunter_move (available_coordinates, traps)) hunters), mysort (map (prey_move (available_coordinates, traps)) preys)))

simulate :: [[Cell]] -> ([(Hunter, Coordinate)], [(Prey, Coordinate)])
simulate environment = start (generate_available_coordinates_and_traps environment ([], []) 0 0) (check (find_hunters environment [] 0 0, find_preys environment [] 0 0))
