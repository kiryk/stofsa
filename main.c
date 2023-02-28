#include <stdlib.h>
#include <stdio.h>

struct state;
struct transition;

struct trans {
	struct trans *next;
	struct state *state;
	int rune;
};

struct state {
	struct trans *trans;
	int accepts;
};

int cmp_state(struct state *key, struct state *dat)
{
	struct trans *ktr = key->trans;
	struct trans *dtr = dat->trans;

	int diff;

	while (ktr && dtr) {
		if ((diff = ktr->rune - dtr->rune) != 0)
			return diff;
		if ((diff = cmp_state(ktr->state, dtr->state)) != 0)
			return diff;
		ktr = ktr->next;
		dtr = dtr->next;
	}
	if (ktr)
		return 1;
	if (dtr)
		return -1;
	return 0;
}

void add_trans(struct state *src, struct state *dst, int rune)
{
	struct trans *tr = malloc(sizeof(*tr));

	tr->next = src->trans;
	src->trans = tr;
	tr->rune = rune;
	tr->state = dst;
}

void free_state(struct state *st)
{
	struct trans *tr = st->trans;

	for (; tr; tr = tr->next) {
		free_state(tr->state);
		free(tr);
	}
	free(st);
}

int main()
{
	struct state unique; /* points to unique states */
	char word[50];
	while (scanf("%s", word) >= 0)
		;
	exit(0);
}
