#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NIT_SHORT_NAMES
#include <nit/list.h>
#include <nit/hmap.h>

typedef uint64_t State;
typedef uint32_t Sym;
typedef char     Tape[];

typedef enum {
	LEFT,
	RIGHT,
	STAY
} Action;

typedef struct {
	State state;
	Sym out;
	Action action;
} Rule;

typedef struct {
	State state;
	Sym in;
} World;

typedef struct {
	int pos;
	State state;
	Nit_hmap *rules;
} Stm;

int
world_compare(const void *entry_key, uint32_t entry_key_size,
	      const void *key, uint32_t key_size)
{
	const World *w1 = entry_key;
	const World *w2 = key;

	return (w1->state == w2->state) && (w1->in == w2->in);
}

void
rule_free(void *key, void *storage)
{
	free(key);
	free(storage);
}

Stm *
stm_new(State start)
{
	Stm *stm = malloc(sizeof(*stm));

	stm->pos = 0;
	stm->state = start;
	stm->rules = hmap_new(0, world_compare, rule_free);
}

void
stm_rule_new(Stm *stm, State state, Sym in, State next, Sym out, Action action)
{
	World *world = calloc(1, sizeof(World));
	Rule *rule = malloc(sizeof(*rule));

	world->state = state;
	world->in = in;

	rule->state = next;
	rule->out = out;
	rule->action = action;

	hmap_add(stm->rules, world, sizeof(World), rule);
}

int
stm_run(Stm *stm, Tape tape)
{
	World world = { 0 };

	world.state = stm->state;
	world.in = tape[stm->pos];

	Rule *rule = hmap_get(stm->rules, &world, sizeof(world));

	if (!rule)
		return 1;

	tape[stm->pos] = rule->out;
	stm->state = rule->state;

	switch (rule->action) {
	case LEFT:
		--stm->pos;
		break;
	case RIGHT:
		++stm->pos;
		break;
	case STAY:
		break;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	Stm *stm = stm_new(0);
	Tape tape = "acdef";

	stm_rule_new(stm, 0, 'a', 1, 'b', RIGHT);
	stm_rule_new(stm, 1, 'c', 2, 'd', STAY);
	while (!stm_run(stm, tape));

	printf("%s\n", tape);
}
