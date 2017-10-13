#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "the2.h"


#define KEY(x) ((x)->key)
#define VALUE(x) ((x)->value)
#define NEXT(x) ((x)->next)
#define DOWN(x) ((x)->down)


node *find_at_level(node *p, int key)
{
	if (p)
	{
		while (NEXT(p))
		{
			if (key > KEY(NEXT(p))) p = NEXT(p);
			else break;
		}
		return p;
	}
	return NULL;
}

node *find(node *list, int key)
{
	if (list)
	{
		if (key > 0)
		{
			node *p = DOWN(list);
			node *t;
			while (p)
			{
				t = find_at_level(p, key);
				if (NEXT(t) && (key == KEY(NEXT(t)))) return NEXT(t);
				if (DOWN(t)) p = DOWN(t);
				else return NULL;
			}
		}	
		return NULL;
	}
	return NULL;
}

int which_level(node *list, node *p)
{
	if (list)
	{
		node *level_head = DOWN(list);
		node *q;
		while (level_head)
		{
			q = level_head;
			while (q)
			{
				if (q == p) break;
				else
				{
					q = NEXT(q);
				}
			}
			if (q == p) break;
			else
			{
				level_head = DOWN(level_head);
			}
		}
		return -KEY(level_head);
	}
	else return -1;
}

node *goto_level(node *list, int level_num)
{
	if (list)
	{
		list = DOWN(list);
		while(list)
		{
			if (level_num == -KEY(list)) break;
			else if (level_num > -KEY(list)) list = NULL;
			else list = DOWN(list);
		}
		return list;
	}
	else return NULL;
}

node *previous_at_row(node *list, node *p)
{
	if (list)
	{
		node *t, *q = DOWN(list);
		while (q)
		{
			t = q;
			while (t && (NEXT(t) != p))
			{
				t = NEXT(t);
			}
			if (t && (NEXT(t) == p)) break;
			else q = DOWN(q);
		}
		if (t && NEXT(t) == p) return t;
		else return NULL;
	}
	else return NULL;
}

node *previous_at_col(node *list, node *p)

{
	if (list)
	{
		if (DOWN(list) == p)
		{
			return list;
		}
		else
		{
			int level = which_level(list, p);
			node *q = goto_level(list, level + 1);
			while (q)
			{
				if (DOWN(q) == p) break;
				else q = NEXT(q);
			}
			return q;
		}
	}
	else return NULL;
}

node *create_new_node(int key, char *value, node *next, node *down)
{
	node *new_node = malloc( sizeof(node) );
	if (!new_node) return NULL;
	KEY(new_node) = key;
	if (value) strncpy(VALUE(new_node), value, V_SIZE);
	NEXT(new_node) = next;
	DOWN(new_node) = down;
	return new_node;
}

node *init(int branch)
{
	node *list_head = malloc( sizeof(node) );
	node *level_head = malloc( sizeof(node) );
	if (!list_head || !level_head) return NULL;
	KEY(list_head) = branch * LIMIT;
	KEY(level_head) = 0;
	NEXT(list_head) = NULL;
	NEXT(level_head) = NULL;
	DOWN(list_head) = level_head;
	DOWN(level_head) = NULL;
	return list_head;
}

int branch_factor(node *list)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list)) return (KEY(list) / LIMIT);
	else return 0;
}

int num_nodes(node *list)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list)) return (KEY(list) % LIMIT);
	else return 0;
}

void clear(node *list)
{
	if (list)
	{
		node *level_head, *temp, *p;
		level_head = DOWN(list);
		DOWN(list) = NULL;
		KEY(list) = 0;
		free(list);
		while (level_head)
		{
			if (NEXT(level_head))
			{
				p = NEXT(level_head);
				while (p)
				{
					temp = p;
					p = NEXT(p);
					free(temp);
				}
			}
			temp = level_head;
			level_head = DOWN(level_head);
			free(temp);
		}
	}
	return;
}

int is_empty(node *list)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list))
	{
		if (NEXT(DOWN(list))) return 0;
	}
	return 1;	
}

int num_levels(node *list)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list)) return ((KEY(DOWN(list))) * (-1) + 1);
	else return 0;
}

node *insert(node *list, int key, char *value)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list))
	{
		node *p = find(list, key);
		if (p)
		{
			while(DOWN(p))
			{
				strncpy(VALUE(p), value, V_SIZE);
				p = DOWN(p);
			}
			strncpy(VALUE(p), value, V_SIZE);
			return p;
		}
		else
		{
			int n, b, i, j, k;
			node *temp = NULL, *new_node;
			KEY(list)++;
			n = num_nodes(list);
			b = branch_factor(list);
			k = 1;
			for (j = 0; 1; j++)
			{
				if (n % (k * b) == 0) k *= b;
				else break;
			}
			for (i = j; i >= 0; i--)
			{
				new_node = create_new_node(key, value, NULL, NULL);
				if (!new_node) return NULL;
				p = goto_level(list, i);
				if (p)
				{
					node *q = find_at_level(p, key);
					NEXT(new_node) = NEXT(q);
					NEXT(q) = new_node;
					if (temp) DOWN(temp) = new_node;
				}
				else
				{
					int last_level = num_levels(list) - 1;
					int needed_level = i;
					int diff = needed_level - last_level;
					if (diff)
					{
						int k;
						node *t = goto_level(list, last_level);
						node *new_level;
						for (k = last_level + 1; k <= needed_level; k++)
						{
							new_level = create_new_node(-k, NULL, NULL, t);
							if (!new_level) return NULL;
							t = new_level;	
						}
						DOWN(list) = t;
						NEXT(t) = new_node;
					}
				}
				temp = new_node;
			}
			return temp;
		}
	}
	return NULL;
}

int delete(node *list, int key)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list))
	{
		node *p = find(list, key);
		if (p)
		{
			node *pre_row, *temp;
			KEY(list)--;
			while (p)
			{
				pre_row = previous_at_row(list, p);
				if (KEY(pre_row) < 0 && !NEXT(p))
				{
					node *q = previous_at_col(list, pre_row);
					DOWN(q) = DOWN(pre_row);
					free(pre_row);
				}
				else
				{
					NEXT(pre_row) = NEXT(p);
				}
				temp = p;
				p = DOWN(p);
				free(temp);
			}
			return 1;
		}
		else return 0;
	}
	return 0;
}

void help_path(node *list, node *p)
{
	if (list == p)
	{
		printf("%*d", K_SIZE, KEY(list));
		return;
	}
	else
	{
		node *pre_row;
		node *pre_col;
		pre_row = previous_at_row(list, p);
		pre_col = previous_at_col(list, p);
		if (pre_col)
		{
			help_path(list, pre_col);
			printf(" v %*d", K_SIZE, KEY(p));
		}
		else
		{
			if (pre_row)
			{
				help_path(list, pre_row);
				printf(" > %*d", K_SIZE, KEY(p));
			}
		}
		return;
	}
}

void path(node *list, int key)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list))
	{
		if (key > 0)
		{
			node *p = find(list, key);
			if (p)
			{
				help_path(list, p);
				printf(":%*s\n", V_SIZE, VALUE(p));
			}
		}
	}
	return;
}

void print(node *list)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list))
	{
		int i, level_num, n_levels = num_levels(list);
		node *p = NEXT(goto_level(list, 0));
		printf("%*d      ", K_SIZE, num_nodes(list));
		for (i = 0; i < n_levels; i++) printf(" %d", i);
		printf("\n         ");
		for (i = 0; i < n_levels; i++) printf(" +");
		printf(" -\n");
		while (p)
		{
			printf("%*d:%*s", K_SIZE, KEY(p), V_SIZE, VALUE(p));
			level_num = which_level(list, find(list, KEY(p))) + 1;
			for (i = 0; i < level_num; i++) printf(" +");
			printf("\n");
			p = NEXT(p);
		}
		return;
	}
	return;
}

void print_level(node *list, int level)
{
	if (list && DOWN(list) && !NEXT(list) && KEY(list))
	{
		if (level != num_levels(list) - 1)
		{
			node *upper, *lower, *p, *q;
			upper = goto_level(list, level + 1);
			lower = goto_level(list, level);
			p = NEXT(upper);
			q = NEXT(lower);
			printf("             ");
			while (p)
			{
				if (DOWN(p) == q)
				{
					printf("%*d:%*s -> ", K_SIZE, KEY(p), V_SIZE, VALUE(p));
					p = NEXT(p);
					q = NEXT(q);
				}
				else
				{
					printf("             ");
					q = NEXT(q);
				}
			}
			printf("\n");
			printf("             ");
			p = NEXT(upper);
			q = NEXT(lower);
			while (p)
			{
				if (DOWN(p) == q)
				{
					printf("   |         ");
					p = NEXT(p);
					q = NEXT(q);
				}
				else
				{
					printf("             ");
					q = NEXT(q);
				}
			}
			printf("\n");
			q = lower;
			while(q)
			{
				if (KEY(q) <= 0)
				{
					printf("%*d:      -> ", K_SIZE, KEY(q));
				}
				else
				{
					printf("%*d:%*s -> ", K_SIZE, KEY(q), V_SIZE, VALUE(q));
				}
				q = NEXT(q);
			}
			printf("\n");
		}
		else
		{
			node *q = goto_level(list, level);
			while(q)
			{
				if (KEY(q) <= 0)
				{
					printf("%*d:      -> ", K_SIZE, KEY(q));
				}
				else
				{
					printf("%*d:%*s -> ", K_SIZE, KEY(q), V_SIZE, VALUE(q));
				}
				q = NEXT(q);
			}
			printf("\n");
		}
	}
	return;
}
