# Language Design
**NOTE:** This is still early design and syntax/functionality may be changed or even removed from the final cut.

** Contents **
1. [Data Types](#data-types)
2. [Memory Allocation & Pointers](#memory-allocation-pointers)
3. [Changes to Function Calls](#changes-to-function-calls)
4. [Struct Resource Management](#struct-resource-management)
5. [Struct Method Unions](#struct-method-unions)
5. [Getters & Setters](#getters-setters)
6. [RAII](#raii)

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

## Struct Method Unions
Here is another deviation from C/C++ (and somewhat other languages.) In moca, a `union` is a way to merge methods into other structs (think of a set theory union). This is useful for functions that are used across many structs without having to reimplement them each time (especially if the implementation changes). Below we go over the syntax and an example use case.

Typically methods are implemented for structs. For example,
```c
methods mystruct
{
	// ...
}
```

If `mystruct` is not defined, this will throw an error. The purpose of a `union` is to use these methods in multiple structs - there will be no members associated with it, and thus, no base struct is necessary. For this, we introduce "anonymous" methods which, by themselves, are struct-less but are then able to be used in other structs.

```c
anon methods my_methods
{
	// ...
}
```

The `anon` keyword denotes these methods are not associated to one struct, and we name this group of methods `my_methods`. We can then use these methods inside multiple structs.

```c
anon methods my_methods
{
	// ...
}

struct A
{
	// ...
};

methods A union my_methods
{
	// ...
}

struct B
{
	// ...
};

methods B union my_methods
{
	// ...
}
```

First we create a set of common methods inside `my_methods` and place them into both struct `A` and `B`. This way, we won't have to copy and paste the same implementation in both structs, and if this implementation changes (or need to add a new common method) we only need to do so in one place.

So what's the use case? Consider you are implementing a machine learning library where you have multiple types of linear models (different algorithms). But each model shares a common score function (to evaluate predictive capability). You can make the score function an anonymous method and use it in all linear models.

```c
anon methods linear_model_common
{
	float mean_squared_error(
		const struct data_t& X,
		const struct data_t& Y)
	{
		// ...
	}
}

struct least_squares_t
{
	// ...
};

methods least_squares_t union linear_model_common
{
	// ...
}

struct svm_t
{
	// ...
};

methods svm_t union linear_model_common
{
	// ...
}
```

In this case we have a least squares model and a support vector machine (SVM) but both can utilize a common `mean_squared_error` method. 

**NOTE:** Anonymous methods also have access to a `this` pointer, so you can modify struct members through anonymous methods. This is the primary difference between anonymous methods and a global function. This requires every struct to have the same member name (for whichever ones are being modified). For example,

```c
anon methods my_methods
{
	void modify_x()
	{
		this.x = 10;
	}
}

struct A
{
	int32 x;
};

struct B
{
	int32 x;
};

methods A union my_methods {} // no additional methods added
methods B union my_methods {} // no additional methods added

// can now make the following calls
struct A a;
a.modify_x(); // a.x = 10

struct B b;
b.modify_x(); // b.x = 10
```

If `x` is not found in the calling struct, an error will be thrown.

**NOTE:** Anonymous methods cannot be overloaded/overwritten. Attempting to redefine an anonymous method inside a struct will result in an error.

## Getters & Setters
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
