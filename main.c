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

int cmp_state(const void *keyp, const void *datp)
{
	const struct state *key = keyp;
	const struct state *dat = datp;

	struct state *ktr = key->trans;
	struct state *dtr = dat->trans;

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

int main()
{
}
