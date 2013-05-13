#include "MCRuntime.h"
#include "Bird.h"
#include "Bird_Engle.h"
#include "BirdFather.h"

int jumpTarget(id const this, const void* entry, int arg1, int arg2)
{
	static int count = 0;
	printf("count %d\n", count);
	printf("arg2 %d\n", arg2);

	//
	if(count==100)return;
	_clean_jump2(make_msg(this, entry), 300, ++count);
}

int TargetVoid()
{
	printf("TargetVoid\n");
	printf("TargetVoid haha\n");
	printf("TargetVoid hahaha\n");
}

void test_object_life_cycle()
{
		Bird* bird;
		bird = _new(_alloc(Bird), "Bird", Bird_load, Bird_init);
		debug_log("bird pointer %p\n", bird);

		Bird* bird2;
		bird2 = (Bird*)_new(_alloc(Bird), "Bird", Bird_load, Bird_init);
		debug_log("bird2 pointer %p\n", bird2);


		Bird* bird4 = ff(new(Bird), initWithType, 4);
		ff(bird4, fly, nil);

		BirdFather* father = new(BirdFather);

		_push_jump(make_msg(bird, Bird_fly), nil);



		debug_log("after new\n");

		//for(;;){
		ff(bird, fly, nil);
		ff(bird, flyhigh, nil);
		ff(bird, singAsong, "a song from child");
		ff(bird, fatherAge, nil);

		ff(father, fly, nil);
		debug_log("%s is child of %s\n", bird->isa->name, bird->super->isa->name);

		relnil(bird);
		relnil(bird2);
		relnil(bird4);
		relnil(father);
}

//not use
void test_mode_shift()
{
	Bird* bird = new(Bird);
	//mode change
	shift(bird, Bird_Engle);
		ff(bird, engleFly, nil);
		ff(bird, engleFatherAge, nil);
		ff(bird, engleFight, nil);
		ff(bird, engleEat, nil);
	shift_back(bird);
	relnil(bird);
}

void test_method_jumpping()
{
	debug_log("start\n");
	_push_jump(make_msg(nil, jumpTarget), 100, 200);

}

int main(int argc, char const *argv[])
{
	mc_init();
		LOG_LEVEL = VERBOSE;

//		test_method_jumpping();

		test_object_life_cycle();

		//test_mode_shift();

	mc_end();
	return 0;
}
