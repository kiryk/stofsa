#include <stdlib.h>
#include <stdio.h>

#define UtfMax 7

struct state;
struct trans;

struct trans {
	struct trans *next;
	struct state *state;
	int rune;
};

struct state {
	struct trans *trans;
	int id;
	int indeg;
	int accepts;
};

/* calculate the number of extra bytes */
#define Extra(ch) ( \
	((ch) & 0x80) == 0x00? 0 : \
	((ch) & 0xe0) == 0xc0? 1 : \
	((ch) & 0xf0) == 0xe0? 2 : \
	((ch) & 0xf8) == 0xf0? 3 : \
	((ch) & 0xfc) == 0xf8? 4 : \
	((ch) & 0xfe) == 0xfc? 5 : -1)

/* calculate the number of bytes needed to encode a character */
#define Bytes(ch) ( \
	(ch) <= 0x7f?       1 : \
	(ch) <= 0x7ff?      2 : \
	(ch) <= 0xffff?     3 : \
	(ch) <= 0x1fffff?   4 : \
	(ch) <= 0x3ffffff?  5 : \
	(ch) <= 0x7fffffff? 6 : -1)

int utf8_to_int(char *s)
{
	int n = Extra(*s);
	int ch;

	/* read value from the header */
	if (n == 0)
		ch = *s & 0x7f;
	else if (n > 0)
		ch = *s & (0x1f >> n);
	else
		return -1;

	/* read values from the extra bytes */
	while (n-- > 0) {
		if ((*++s & 0xc0) != 0x80)
			return -1;
		ch = (ch << 6) | *s & 0x3f;
	}

	return ch;
}

char *utf8_from_int(char *s, int ch)
{
	int n = Bytes(ch);
	int i;

	if (n <= 0) {
		n = 0;
	} else if (n == 1) {
		s[0] = ch;
	} else {
		for (i = n-1; i > 0; i--) {
			s[i] = 0x80 | (ch & 0x3f); /* 0x10XXXXXX */
			ch >>= 6;
		}
		s[0] = 0;
		for (i = 0; i < n; i++)
			s[0] = (s[0] >> 1) | 0x80;
		s[0] |= (ch & 0xff);
	}

	s[n] = '\0';

	return s;
}

void utf8_decode(int *runes, char *s)
{
	int ch, i, j;

	for (i = j = 0; s[i]; i += 1+Extra(s[i]), j++)
		runes[j] = utf8_to_int(s+i);
	runes[j] = 0;
}

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
	dst->indeg++;
}

void free_state(struct state *st)
{
	struct trans *tr, *next;

	if (--st->indeg > 0)
		return;
	for (tr = st->trans; tr; tr = next) {
		next = tr->next;
		free_state(tr->state);
		free(tr);
	}
	free(st);
}

void print_state(struct state *st)
{
	struct trans *tr;
	char utf[UtfMax];

	for (tr = st->trans; tr; tr = tr->next)
		printf("%d %d %s\n", st->id, tr->state->id, utf8_from_int(utf, tr->rune));
	for (tr = st->trans; tr; tr = tr->next)
		print_state(tr->state);
}

void add_string(struct state *st, int *s)
{
	static int max_id = 0;

	struct state *nst;
	int i;

	for (i = 0; s[i]; st = nst, i++) {
		nst = calloc(1, sizeof(*nst));
		nst->id = ++max_id;
		add_trans(st, nst, s[i]);
	}
	st->accepts = 1;
}

struct state *get_last(struct state *st, int *s, int *last)
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
	for (tr = uniq->trans; tr; tr = tr->next) {
		if (cmp_state(last, tr->state) == 0)
			break;
	}
	if (tr) {
		tr->state->indeg++;
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
	int runes[128];
	char word[128];

	while (scanf("%s", word) >= 0) {
		utf8_decode(runes, word);
		last = get_last(&fsa, runes, &length);
		if (last->trans)
			unify_state(&uniq, last);
		add_string(last, runes+length);
	}
	unify_state(&uniq, &fsa);
	print_state(&fsa);
	exit(0);
}
