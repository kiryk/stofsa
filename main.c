#include <stdlib.h>
#include <stdio.h>

struct state;
struct trans;

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

  if ((diff = dat->accepts - key->accepts) != 0)
		return diff;
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
	struct trans *tr = calloc(1, sizeof(*tr));

	tr->next = src->trans;
	src->trans = tr;
	tr->rune = rune;
	tr->state = dst;
}

void free_state(struct state *st)
{
	struct trans *tr, *next;

	for (tr = st->trans; tr; tr = next) {
		next = tr->next;
		free_state(tr->state);
		free(tr);
	}
	free(st);
}

void add_string(struct state *st, char *s)
{
	struct state *nst;
	int i;

	for (i = 0; s[i]; st = nst, i++) {
		nst = calloc(1, sizeof(*nst));
		add_trans(st, nst, s[i]);
	}
	st->accepts = 1;
}

struct state *get_last(struct state *st, char *s, int *last)
{
	struct trans *tr = st->trans;
	int i;

	for (i = 0; s[i]; i++) {
		if (!st->trans || st->trans->rune != s[i])
			break;
		st = st->trans->state;
	}
	*last = i;
	return st;
}

void unify_state(struct state *uniq, struct state *st)
{
	struct state *last = st->trans->state;
	struct trans *tr;

	if (last->trans)
		unify_state(uniq, last);
	for (tr = uniq->trans; tr; tr = tr->next)
		if (cmp_state(last, tr->state) == 0)
			break;
	if (tr) {
		free_state(st->trans->state);
		st->trans->state = tr->state;
	} else {
		add_trans(uniq, last, 0);
	}
}

int main()
{
	struct state uniq = {}; /* points to unique states */
	struct state fsa = {};  /* the FSA being built */
	struct state *last;
	int length;
	char word[50];

	while (scanf("%s", word) >= 0) {
		last = get_last(&fsa, word, &length);
		if (last->trans)
			unify_state(&uniq, last);
		add_string(last, word+length);
	}
	unify_state(&uniq, &fsa);
	exit(0);
}
