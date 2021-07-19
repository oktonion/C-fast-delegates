#include "./testsuit/testsuit.h"

#include "..//delegates/delegate.h"


bool func_called = false;

static void void_func() {func_called = true;}
void void_func_void_p(void*)
{
    func_called = true;
}

TEST_CASE("Testing cpp delegate 0") {
	
	using delegates::delegate;
	using delegates::bind;

	SUBCASE("Delegate 0 empty construction")
	{
		delegate<void> d0;
		
		CHECK(!d0);
	}

	SUBCASE("Delegate 0 in-place construction")
	{
		delegate<void> d0(&void_func);
		
		CHECK(d0);
	}

	SUBCASE("Delegate 0 copy construction")
	{
		delegate<void> d0 = delegate<void>(&void_func);

		CHECK(d0);
	}

	SUBCASE("Delegate 0 free func invocation")
	{
        delegate<void> d0;
		d0 = delegate<void>(&void_func);

		func_called = false;
		d0();
		CHECK(true == func_called);

        void *vptr = 0;
        int *iptr;

        d0 = delegate<void>(vptr, &void_func_void_p);

		func_called = false;
		d0();
		CHECK(true == func_called);

		func_called = false;
	}
}
