/*
Copyright (c) <2013>, <Sun Yuli>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <Monk-C> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __MCRuntime__
#define __MCRuntime__

/* Mocha use many C99 standard features, make sure your compiler and platform support C99 standard */
//#pragma warning(disable:3)

#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>

#include "Log.h"
#include "Vectors.h"

//max memory useage for class  table is: 4Byte x 4000 = 16KB
//max memory useage for method table is: 4Byte x 4000 x 1000 = 16000KB = 16M

//1000 classes 16M    
//100  classes 1.6M
//10   classes 160KB

//MAX_METHOD_NUM set to 4 time of the space needed
//10level x 100 x 4 = 4000

#ifndef MAX_METHOD_NAME_CHAR_NUM
#define MAX_METHOD_NAME_CHAR_NUM 100
#endif
#ifndef MAX_CLASS_NAME_CHAR_NUM
#define MAX_CLASS_NAME_CHAR_NUM 50
#endif
#ifndef ANONY_POOL_SIZE
#define ANONY_POOL_SIZE 1000
#endif

typedef int BOOL;
#define YES 1
#define NO 0
typedef int RES;
#define SUCCESS 0
#define ERROR -1

#define xxx void* xxx
#define nil ((void*)0)
#define S(value) #value
#define A_B(a, b) a##_##b

//method info struct
typedef struct MCMethod_struct
{
	struct MCMethod_struct* next;
	void* addr;
	unsigned level;
	unsigned hash;
	unsigned index;
	char name[MAX_METHOD_NAME_CHAR_NUM];
}MCMethod;

typedef struct MCHashTable_struct
{
	unsigned level;
	MCMethod* data[];
}MCHashTable;

//meta class, the struct is a node for inherit hierarchy
typedef struct MCClass_struct
{
	struct MCClass_struct* next;
	int method_count;
	unsigned level;
	unsigned hash;
	unsigned index;
	char name[MAX_CLASS_NAME_CHAR_NUM];
	MCHashTable* table;
}MCClass;

//for type cast, every object have the 3 var members
typedef struct MCObject_struct
{
	struct MCObject_struct* super;
	MCClass* isa;
	MCClass* saved_isa;
	MCClass* mode;
	int ref_count;
}MCObject;

//id is a object pointer for any class
typedef MCObject* id;

//use for message sending
typedef struct MCMessage_struct
{
	id object;
	void* addr;
}MCMessage;

//for class define
#define implements(protocol)
#define extends(super)
#define class(cls) _newline;\
typedef struct cls##_struct{\
	struct cls##_struct* super;\
	MCClass* isa;\
	MCClass* saved_isa;\
	MCClass* mode;\
	int ref_count;
#define end(cls) }cls;\
MCClass* cls##_load(MCClass* const class);\
cls* cls##_init(cls* const this);

//callback function pointer types
typedef MCClass* (*loaderFP)(MCClass*);
typedef MCObject* (*initerFP)(MCObject*);

//callbacks
#define loader(cls)					MCClass* cls##_load(MCClass* const class)
#define initer(cls)						cls* cls##_init(cls* const this)
//category
#define category(ori, cat) 			MCClass* ori##cat##_load(MCClass* const class);\
										ori* ori##cat##_init(ori* const this)
#define catloader(ori, cat)			MCClass* ori##cat##_load(MCClass* const class)
#define catiniter(ori, cat)				ori* ori##cat##_init(ori* const this)
//mode
#define classmode(ori, cat) 		MCClass* ori##cat##_load(MCClass* const class);\
										ori* ori##cat##_init(ori* const this)
#define modloader(ori, cat)			MCClass* ori##cat##_load(MCClass* const class)
#define modiniter(ori, cat)				ori* ori##cat##_init(ori* const this)			

//callback caller
#define load(cls)					_load(S(cls), cls##_load)
#define init(obj, cls)			    cls##_init(obj)
#define clean(obj, cls)				cls##_clean(obj)

//method binding
#define binding(cls, met, ...)  	_binding(class, S(met), A_B(cls, met))
#define override(cls, met, ...) 	_override(class, S(met), A_B(cls, met))
#define method(cls, name, ...) 		void* cls##_##name(cls* const this, const void* entry, __VA_ARGS__)
#define protocol(pro, name, ...)  	static void* pro##_##name(id const this, const void* entry, __VA_ARGS__)
#define This(cls)      				((cls*)this)
#define Cast(cls, obj) 				((cls*)obj)
#define returns(type)

//allocators
#define _alloc(cls) 				(cls*)mc_malloc(sizeof(cls))//private macro, usr should not call this
#define _alloc_clear(cls) 			(cls*)mc_calloc(sizeof(cls))//private macro, usr should not call this
#define _alloc_onstack(cls) 		(cls*)alloca(sizeof(cls))//private macro, usr should not call this
#define _alloc_anony(cls) 			(cls*)mc_malloc_anony(sizeof(cls))//private macro, usr should not call this

//for create object
#define new(cls)					_new(_alloc(cls), S(cls), cls##_load, cls##_init)
#define new_clear(cls, ...)         _new(_alloc_clear(cls), S(cls), cls##_load, cls##_init)
#define new_onstack(cls, ...)       _new(_alloc_onstack(cls), S(cls), cls##_load, cls##_init)
#define new_anony(cls, ...)         _new(_alloc_anony(cls), S(cls), cls##_load, cls##_init)
#define new_category(ori, cat)		_new_category(_alloc(ori), S(ori), ori##_load, ori##_init, S(cat), ori##cat##_load, ori##cat##_init)

//for call method
#define call(this, cls, name, ...)      cls##_##name(this, cls##_##name, __VA_ARGS__)//call other class method
#define response_to(obj, met) 			_response_to(obj, S(met))
#define ff(obj, met, ...)				_push_jump(_response_to(obj, S(met)), __VA_ARGS__)
#define shift(obj, ori, mode)			_shift(obj, S(mode), ori##mode##_load)
#define shift_back(obj)					_shift_back(obj)

//Reference Count
#define REFCOUNT_NO_MM 		-1
#define REFCOUNT_ANONY_OBJ 	-100
#define relnil(obj) _relnil(&obj)
void release(id const this);
void retain(id const this);
void _relnil(MCObject** const this);

//method handling
unsigned _binding(MCClass* const aclass, const char* methodname, void* value);
unsigned _override(MCClass* const aclass, const char* methodname, void* value);

MCMessage _response_to(id const obj, const char* methodname);
MCMessage _self_response_to(id const obj, const char* methodname);
MCMessage make_msg(id const obj, const void* entry);

//make a thread-safe allocator
void* mc_malloc(size_t size);
void* mc_malloc_anony(size_t size);
void* mc_calloc(size_t size);
void* mc_realloc(void* ptr, size_t size);
void  mc_free(void *ptr);

//language context
void mc_init();
void mc_end();

//hash table
unsigned hash(const char *s);
void init_table(MCHashTable** const table_p, unsigned initlevel);
unsigned set_method(MCHashTable** const table_p, MCMethod* const method, BOOL isOverride);
unsigned set_class(MCClass* const aclass);
unsigned get_size_by_level(const unsigned level);
MCMethod* get_method_by_name(const MCHashTable** table_p, const char* name);
MCMethod* get_method_by_hash(const MCHashTable** table_p, const unsigned hashval, const char* refname);
MCMethod* get_method_by_index(const MCHashTable** table_p, const unsigned index);
MCClass* get_class_by_name(const char* name);
MCClass* get_class_by_hash(const unsigned hashval, const char* refname);
MCClass* get_class_by_index(const unsigned index);

//string operate
void mc_copyMethodName(MCMethod* method, const char* name);
int mc_compareMethodName(MCMethod* method, const char* name);
void mc_copyClassName(MCClass* aclass, const char* name);
int mc_compareClassName(MCClass* aclass, const char* name);

//class pool
MCClass* _load(const char* name_in, loaderFP loader);
id _new(id const this, const char* name_in, loaderFP loader, initerFP initer);
id _new_category(id const this, 
	const char* name_origin, loaderFP loader, initerFP initer,
	const char* name_cat, loaderFP loader_cat, initerFP initer_cat);
MCClass* _get_class(unsigned index);
void _set_class(unsigned index, MCClass* aclass);
void _shift(id const obj, const char* modename, loaderFP loader);
void _shift_back(id const obj);

//write by asm
void* _push_jump(MCMessage msg, ...);
void* _clean_jump1(MCMessage msg, ...);
void* _clean_jump2(MCMessage msg, ...);
void* _clean_jump3(MCMessage msg, ...);
void* _clean_jump4(MCMessage msg, ...);
void mc_atomic_set_integer(int* target, int value);
void mc_atomic_set_pointer(void** target, void* value);

#endif