/*


THE 1

Beyazıt Yalçınkaya
2172138


*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define MAX_CHAR 201

char Infix[MAX_CHAR], Number_Of_Variables_Table[26], Char_Table[26];
int Jump_For_Float, N, Number_Of_Variables = 0, Number_Of_Elements_In_Postfix = 0;
long int Count_Of_Experiments;
double Postfix[MAX_CHAR], Stack[MAX_CHAR], Input[26][103], Final_Max, Final_Min, Char_Table_Values[26], Posibilities[100];

int is_continuos(char *p)
{
	if ((*(p + 1) >= 48 && *(p + 1) <= 57) || *(p + 1) == '.') return 1;
	else return 0;
}

double convert_to_float(char *Infix)
{
	char temp_Stack[MAX_CHAR];
	double out = 0;
	int i = 1, c = 0, f = 0;
	if (Infix[0] == '.')
	{
		temp_Stack[0] = '0';
		temp_Stack[1] = Infix[0];
		c = 1, f = 1;
		while (1)
		{
			if ((Infix[i] >= 48 && Infix[i] <= 57) || Infix[i] == '.')
			{
				temp_Stack[i + 1] = Infix[i];
				i++;
			}
			else break;
		}
		Jump_For_Float = i - 1;
	}
	else
	{
		temp_Stack[0] = Infix[0];
		while (1)
		{
			if ((Infix[i] >= 48 && Infix[i] <= 57) || Infix[i] == '.')
			{
				temp_Stack[i] = Infix[i];
				if (Infix[i] == '.') c = i, f = 1;
				i++;
			}
			else break;
		}
		i--;
		Jump_For_Float = i;
	}
	if (f)
	{
		c -= i;/*# decimals after dot*/
	}
	for (; i >= 0; i--)
	{
		if (f)
		{
			if (temp_Stack[i] == '.')
			{
				out += (temp_Stack[--i] - 48) * pow(10, c++);
			}
			else
			{
				out += (temp_Stack[i] - 48) * pow(10, c++);
			}
		}
		else
		{
			out += (temp_Stack[i] - 48) * pow(10, c++);
		}
	}
	return out;
}

int pop_until_lp(int j, int k)
{
	int i = 0;
	for (;Stack[j] != '('; j--)
	{
		Postfix[k++] = -Stack[j];
		i++;
	}
	--j;
	if (Stack[j] == 's' || Stack[j] == 'c' || Stack[j] == 'q' || Stack[j] == 'l' || Stack[j] == '~')
	{
		Postfix[k++] = -Stack[j];
	}
	return i + 1;
}

int is_safe(char operation, int j)
{
	char previous;
	if (j == 0) return 1;
	else
	{
		previous = Stack[j - 1];
		if (operation == '+')
		{
			if (previous == '(') return 1;
		}
		else if (operation == '-')
		{
			if (previous == '(') return 1;
		}
		else if (operation == '*')
		{
			if (previous == '+' || previous == '-'|| previous == '(') return 1;
		}
		else if (operation == '/')
		{
			if (previous == '+' || previous == '-' || previous == '(') return 1;
		}
		else if (operation == '^')
		{
			if (previous == '+' || previous == '-' || previous == '*'|| previous == '/'|| previous == '(' || previous == '^') return 1;
		}
		else if (operation == '~' || operation == 's' || operation == 'c' || operation == 'q' || operation == 'l')
		{
			if (previous == '+' || previous == '-'|| previous == '*'|| previous == '/'|| previous == '^'|| previous == '(') return 1;
		}
	}
	return 0;
}

int make_available(char operation, int j, int k, int c)
{
	c++;
	Postfix[k++] = -Stack[--j];
	if (!is_safe(operation, j)) return make_available(operation, j, k, c);
	else return c;
}

void make_it_Postfix() /*Because of the return value of the function we will know how many Variables we have*/
{
	char t, temp;
	int i, j = 0, k = 0, w;/*i = current index in Infix string, j = current index in Stack, k = current index in Postfix*/
	for (i = 0; Infix[i] != 0; i++)
	{
		Number_Of_Elements_In_Postfix++;
		t = Infix[i];
		if (t >= 65 && t <= 90)
		{
			Postfix[k++] = -t;/*Variables are negative in Postfix*/
			if (Number_Of_Variables_Table[t - 65] == 0)
			{
				Number_Of_Variables++;
				Number_Of_Variables_Table[t - 65] = 1;
			}
		}
		else if ((t >= 48 && t <= 57) || (t == '.'))
		{
			if (is_continuos(Infix + i))
			{
				Postfix[k++] = convert_to_float(Infix + i);
				i += Jump_For_Float;
			}
			else
			{
				Postfix[k++] = t - 48;
			}
		}
		else if (t == 's' || t == 'c' || t == 'l' || t == '+' || t == '-' || t == '*' || t == '/' || t == '^' || t == '~')
		{
			if (t == 's')
			{
				if (Infix[i + 1] == 'i')/*if t is sin*/
				{
					if (!is_safe('s', j))
					{
						w = make_available('s', j, k, 0);
						j -= w;
						k += w;
					}
					Stack[j++] = 's';
					i += 2;
				}
				else if (Infix[i + 1] == 'q')/*if t is sqrt*/
				{
					if (!is_safe('q', j))
					{
						w = make_available('q', j, k, 0);
						j -= w;
						k += w;
					}
					Stack[j++] = 'q';
					i += 3;
				}
			}
			else if (t == 'c')/*if t is cos*/
			{
				if (!is_safe('c', j))
				{
					w = make_available('c', j, k, 0);
					j -= w;
					k += w;
				}
				Stack[j++] = 'c';
				i += 2;
			}
			else if (t == 'l')/*if t is ln*/
			{
				if (!is_safe('l', j))
				{
					w = make_available('l', j, k, 0);
					j -= w;
					k += w;
				}
				Stack[j++] = 'l';
				i += 1;
			}
			else if (t == '+' || t == '-' || t == '*' || t == '/' || t == '^' || t == '~')/*if t is an ordinary operator or ~*/
			{
				if (!is_safe(t, j))
				{
					w = make_available(t, j, k, 0);
					j -= w;
					k += w;
				}
				Stack[j++] = t;
			}
		}
		else if (t == '(')
		{
			Number_Of_Elements_In_Postfix--;
			Stack[j++] = t;
			/*Push t to the Stack*/

		}
		else if (t == ')')
		{
			Number_Of_Elements_In_Postfix--;
			temp = pop_until_lp(j - 1, k);
			j -= temp;/*deleting (*/
			k += (temp - 1);/*new index available on Postfix*/
			/*Pop until you reach the ( then delete (*/
		}
		else
		{
			Number_Of_Elements_In_Postfix--;
			continue;
		}
	}
	if (j != 0)/*if there exists still operation in Stack*/
	{
		for (--j; j >= 0; j--)
		{
			Postfix[k++] = -Stack[j];
		}
	}
}

int choose(int i)
{
	int j;
	double a;
	a = ((double)rand())/((double)RAND_MAX);
	for (j = 3; j < N + 3; j++)
	{
		if ((a -= Input[i][j]) <= 0) return j - 2;
	}
	return 0;
}

double number_from_interval(int which_interval, double lower, double upper)
{
	return lower + (which_interval - 1) * (upper - lower) / (double)N + ((upper - lower) / (double)N) * ((double)rand() / RAND_MAX);
}

double random_value(double variable)
{
	int i, which_interval;
	double value;
	for (i = 0; i < Number_Of_Variables; i++)
	{
		if (variable == Input[i][0]) break;
	}
	if (Char_Table[i])
	{
		return Char_Table_Values[i];
	}
	else
	{
		which_interval = choose(i);
		while (!which_interval)
		{
			which_interval = choose(i);
		}
		value = number_from_interval(which_interval, Input[i][1], Input[i][2]);
		Char_Table[i] = 1;
		Char_Table_Values[i] = value;
		return value;
	}
}

double give_value(double x)
{
	if (x < 0 && x > -126.0)
	{
		return random_value(-x);
	}
	else if (x <= -126.0)
	{
		return x + 126.0;
	}
	else return x;
}

int give_me_other_index(char table[], int i)
{
	for (; i >= 0; i--)
	{
		if (table[i] == 1)
		{
			table[i] = 0;
			break;
		}
	}
	return i;
}

double evaluate()
{
	int i, j;
	double virtual_postfix[MAX_CHAR], temp;
	char table[MAX_CHAR];
	for (i = 0; i < Number_Of_Elements_In_Postfix; i++) virtual_postfix[i] = Postfix[i];
	for (i = 0; i < Number_Of_Elements_In_Postfix; i++) (Postfix[i] == -'+' || Postfix[i] == -'-' || Postfix[i] == -'*' || Postfix[i] == -'/' || Postfix[i] == -'^' || Postfix[i] == -'~' || Postfix[i] == -'s' || Postfix[i] == -'c' || Postfix[i] == -'q' || Postfix[i] == -'l') ? (table[i] = -1) : (table[i] = 1);
	for (i = 0; i < Number_Of_Elements_In_Postfix; i++)
	{
		if (virtual_postfix[i] == -'+')
		{
			table[i - 1] = 0;
			table[i] = 1;
			j = give_me_other_index(table, i - 1);
			temp = give_value(virtual_postfix[j]) + give_value(virtual_postfix[i - 1]);
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (virtual_postfix[i] == -'-')
		{
			table[i - 1] = 0;
			table[i] = 1;
			j = give_me_other_index(table, i - 1);
			temp = give_value(virtual_postfix[j]) - give_value(virtual_postfix[i - 1]);
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (virtual_postfix[i] == -'*')
		{
			table[i - 1] = 0;
			table[i] = 1;
			j = give_me_other_index(table, i - 1);
			temp = give_value(virtual_postfix[j]) * give_value(virtual_postfix[i - 1]);
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (virtual_postfix[i] == -'/')
		{
			table[i - 1] = 0;
			table[i] = 1;
			j = give_me_other_index(table, i - 1);
			temp = give_value(virtual_postfix[j]) / give_value(virtual_postfix[i - 1]);
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (virtual_postfix[i] == -'^')
		{
			table[i - 1] = 0;
			table[i] = 1;
			j = give_me_other_index(table, i - 1);
			temp = pow(give_value(virtual_postfix[j]), give_value(virtual_postfix[i - 1]));
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (virtual_postfix[i] == -'~')
		{
			table[i - 1] = 0;
			table[i] = 1;
			temp = (-1.0) * give_value(virtual_postfix[i - 1]);
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (virtual_postfix[i] == -'s')
		{
			table[i - 1] = 0;
			table[i] = 1;
			temp = sin(give_value(virtual_postfix[i - 1]));
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (virtual_postfix[i] == -'c')
		{
			table[i - 1] = 0;
			table[i] = 1;
			temp = cos(give_value(virtual_postfix[i - 1]));
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (virtual_postfix[i] == -'q')
		{
			table[i - 1] = 0;
			table[i] = 1;
			temp = sqrt(give_value(virtual_postfix[i - 1]));
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
		else if (Postfix[i] == -'l')
		{
			table[i - 1] = 0;
			table[i] = 1;
			temp = log(give_value(virtual_postfix[i - 1]));
			virtual_postfix[i] = ((temp < 0) ? (temp - 126) : (temp));
		}
	}
	return give_value(virtual_postfix[i - 1]);
}

void getting_Input()
{
	int i, j;
	char temp_char;/*Infix[MAX_CHAR] = given definition of the function, temp = for holding temporary chars*/
	scanf("%[^\n]s", Infix);/*Getting given function*/
	make_it_Postfix(Infix);/*Number_Of_Variables = make_it_Postfix(Infix) (converted to Postfix and stored in Postfix and Number_Of_Variables holds the count of random Variables)*/
	scanf("%d %ld", &N, &Count_Of_Experiments);
	for (i = 0; i < Number_Of_Variables; i++)
	{
		for (j = 0; j < N + 3; j++)
		{
			if (j == 0)
			{
				scanf(" %c", &temp_char);
				Input[i][j] = (double)temp_char;
			}
			else
			{
				scanf(" %lf", &Input[i][j]);
			}
		}
	}
}

void generate_Posibilities()
{
	double temp;
	int i, index;
	clock_t time_value;
	time_value = time(NULL);
	srand(time_value);
	for (i = 0; i < Count_Of_Experiments; i++)
	{
		temp = evaluate();
		memset(Char_Table, 0, sizeof(Char_Table));
		if (i == 0) Final_Max = Final_Min = temp;
		else
		{
			if (temp > Final_Max) Final_Max = temp;
			else if (temp < Final_Min) Final_Min = temp;
		}
	}
	srand(time_value);
	for (i = 0; i < Count_Of_Experiments; i++)
	{
		temp = evaluate();
		memset(Char_Table, 0, sizeof(Char_Table));
		if (temp > Final_Max || temp < Final_Min)
		{
			i--;
		}
		else if (temp > Final_Min && temp < Final_Max)
		{
			index = ((int)(((temp - Final_Min) / (Final_Max - Final_Min)) * N));
			Posibilities[(index <= 0) ? (0) : ((index >= N) ? (N - 1) : (index))] += 1;
		}
		else if (temp == Final_Max)
		{
			Posibilities[N - 1] += 1;
		}
		else if (temp == Final_Min)
		{
			Posibilities[0] += 1;
		}
	}
}

int main()
{
	int i;
	getting_Input();
	generate_Posibilities();
	printf("%f %f", Final_Min, Final_Max);
	for (i = 0; i < N; i++)
	{
		printf(" %f", (Posibilities[i]) / ((double)Count_Of_Experiments));
	}
	printf(" ");
	return 0;
}