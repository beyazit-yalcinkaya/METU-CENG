lambda = 4;
e_to_m_lambda = exp(-lambda);
N = 0.25 * (1.96 / 0.005)^2;
TotalCaught = zeros(1, N);
TotalWeight = zeros(1, N);
TotalExpected = zeros(1, N);
for i = 1: N
	caughtMinions = 0;
	countMinions = 0;
	sumForExpected = 0;
	for r=1: 5
		U = rand;
		while (U >= e_to_m_lambda)
			U = U * rand;
			caughtMinions = caughtMinions + 1;
		end
	end
	for j = 1: caughtMinions
                w = rand * 16;
                s = rand * 16;
                z = rand * 0.2;
                while (z > w * s * exp(-w - s))
                        w = rand * 16;
                        s = rand * 16;
                        z = rand * 0.2;
                end
                if (w >= 2 * s);
                        countMinions = countMinions + 1;
                end
		TotalWeight(i) = TotalWeight(i) + w;
        	a = (-0.5) * log(1 - rand);
		b = -8 + rand * 16;
		c = rand * 0.4;
		while (c > (exp(-(b^2) / 2) / (sqrt(2 * pi))))
			b = -8 + rand * 16;
			c = rand * 0.4;
		end
		sumForExpected = sumForExpected + ((a + 2 * b) / (1 + abs(b)));
	end
      	TotalCaught(i) = countMinions;
	TotalExpected(i) = sumForExpected / caughtMinions;
end
EstimatedProbability = sum(TotalCaught > 6) / N
EstimatedTotalWeight = mean(TotalWeight)
EstimatedExpected = mean(TotalExpected)

