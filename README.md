# moca - [MO]dified [C] l[A]nguage
My first ever compiled programming language - very experimental.

This is aimed to be a blend of C, C++ and some custom features.

You can think of this being a stripped version of C as its base with some features from C++ & my own features such as:
* RAII
* Struct methods
* Optional auto-generated getters/setters (custom feature)
* References
* More explicit functions calls (custom feature) - see [below](#changes-to-function-calls) for more details
* Lambdas
* Namespaces
* Named Parameters (custom feature)
* Default parameter values

# Language Design
**NOTE:** This is still early design and syntax/functionality may be changed or even removed from the final cut.

## Data Types
The data types will be similar to what you're used to in C.
* `uint8`
* `uint16`
* `uint32`
* `uint64`
* `int8`
* `int16`
* `int32`
* `int64`
* `float`
* `string`

## Memory Allocation & Pointers
As of now, the current design is to use a single syntax for memory addresses to refer to "pointers" or "references". Allocating a block of memory for five `int32` types could be as follows:

```cpp
int32& myints = new int32[5];
delete myints;
```

The syntax is essentially equivalent to C++.

This "reference" syntax is analogous to pointers. E.g., if you do

```cpp
int32& myints = new int32[5];
int32& shared = myints; // shared now points to same location as myints
delete myints;
shared[0] = 10; // oh no! writing to free'd memory
```

Another example to have a "pointer" (reference) to a stack-allocated variable:

```cpp
int32 mylocal = 10;
int32& localref = mylocal; // points to the address of mylocal
```

If `mylocal` goes out of scope, writing to `localref` may exhibit bad behaviour.

## Changes to Function Calls
As mentioned earlier, there are some slight changes to how funciton calls are made depending on the function prototype. Consider the below examples.

```cpp
int32 func(int32 x, int32 y)
{
	return x + y;
}

int32 main()
{
	int32 z = func(2, 5);

	return 0;
}
```

In this example, there are no changes to the syntax from C/C++. However, what if we add references?

```cpp
void func(const int32& x, const int32& y, int32& z)
{
	z = x + y;
}

int32 main()
{
	int32 x = 2;
	int32 y = 5;
	int32 z = 0;
	
	func(const &x, const &y, &z);
	
	return 0;
}
```

This is the first real difference between the traditional C/C++ syntax. When using references, the caller must explicitly state a variable is being passed by reference or constant reference. This makes it clear what is being modified, passed by reference, or passed by value (and copied).

## Struct Resource Management
There are some major overhauls to structs planned. One feature being allocators/deallocators which are analogous to constructors/destructors.

A struct will serve as a "blueprint" for a data structure (easy to use for free API reference) so it's illegal to define anything inside a struct definition. Instead, you must use the new `methods` keyword to implement methods for a struct (this is similar to Rust's `impl`).

Consider the below struct which manages an integer resource. There are reserved keywords `new` and `delete` meant for implementing these ctor/dtor.

```cpp
struct mydata
{
	// resource
	int32& x;

	// ctor and dtor (reserved keywords for methods)
	void new(const int32& x);
	void delete();
};

methods mydata
{
	void new(const int32& x)
	{
		this.x = new int32;
		this.x = x;
	}

	void delete()
	{
		delete this.x;
	}
}
```

You will get a compiler error if you try implement a constructor without a destructor (or vice versa).

## Getters/Setters
Getters and setters can be tedious to write. There will also be tags for generating different getters/setters to avoid writing redundant code (similar to C#).
* `[get]` - generate a getter for the value
* `[set]` - generate a setter for the value
* `[get&]` - generate a getter for the reference/address
* `[set&]` - generate a setter for the reference/address

Example:
```cpp
struct mydata
{
	int32 x [get, set, get&];
};

int32 main()
{
	struct mydata data;
	data.set_x(10); // auto-generated from "set" tag

	int32& x_ref = data.get_x_ref(); // auto-generated from "get&" tag
	x_ref = 12; // changes will be reflected in x in the struct since we modified its reference

	return 0;
}
```

## RAII
One of the best features of C++ to make resource management easier. I want to also implement this into `moca`. Consider the below example:

```cpp
struct mydata
{
	int32& x [get];
	
	void new(const int32& x);
	void delete();
};

methods mydata
{
	void new(const int32& x)
	{
		this.x = new int32;
		this.x = x;
	}

	void delete()
	{
		delete this.x;
	}
};

void somefunc()
{
	struct mydata& data = new mydata(5);
	printf("%d\n", data.get_x());
}

int32 main()
{
	somfunc();

	return 0;
}
```

Whenever we create the new struct, we actually have two pointers - one pointer to `data` and the pointer to `x` inside of `data`. Notice we're not freeing either of them. When the function goes out of scope, it will call the destructor (if any) to clean up the memory inside of the struct before also deallocating the pointer to `data` automatically.

TODO: expand on some of the other features mentioned: lambdas, namespaces, etc.

# Building from Source
* git clone this repo
* `cd moca`
* `mkdir build && cd build`
* `cmake -DCMAKE_BUILD_TYPE=Debug ..` (or can use `Release` mode)
* `make`

Will add steps on how to compile `moca` code once the actual compiler is written and functional to some degree.

# CTags
This project (optionally) uses ctags, use `make tags` from the project root to generate the tags file and be sure to set `set tags=./tags;$HOME` in your `~/.vimrc`.

# Tests
I'm not using a testing library so all the tests are "hand-crafted". Inside the `test/` directory in your build folder you can use:

`make CTEST_OUTPUT_ON_FAILURE=1 test`

to run the test suite which will display the error (if any) to `stderr` in the console.
