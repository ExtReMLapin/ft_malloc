# ft_malloc


First project working with low-level access to the memory, handeling the malloc function myself, preventing any fragmentation by filling ram "holes" after data was freed

code looks like shit because we are supposed to follow retarded rules at school (less than 25 lines funcs ets, fkin bullshit)


Things to fix :

mallocs are not on a %16 alignement on macos, which is required to speedup the cpu malloc read thing, but working fine on linux.
	
Not making checksum of the malloc structs to check of some tard' is writing over my malloc structs, im lazy so fuck off


ignoring laws of physic since 2018

![](https://puu.sh/z8yW6/db4c4ae49e.png)
![](https://puu.sh/z8yYc/b9e5d3ae8d.png)
![](https://puu.sh/z8yYK/cbea68e25c.png)
![](https://puu.sh/z8yZp/8aa545df7e.png)





``` C


int main(void)
{
	void *dada;
	void* dede;
	
	_malloc(128);
	dede = _malloc(128);
	_malloc(120);
	_malloc(32);
	dada = _malloc(3);
	_malloc(8);

	_free(dada);
	_free(dede);

	_malloc(50000);
	_malloc(900);

	show_alloc_mem();
	return 0;
}

```




```

TINY : 0x105873000 
	Plage #1 with size 49152 @ 0x105873000
		#1 0x105873068 [0x105873088 -> 0x1058730b0] 32 bytes
		free space of 59 bytes from 0x1058730b1 to 0x1058730ea 
		#2 0x1058730eb [0x10587310b -> 0x10587311b] 8 bytes
SMALL : 0x10585b000 
	Plage #1 with size 98304 @ 0x10585b000
		#1 0x10585b068 [0x10585b088 -> 0x10585b110] 128 bytes
		free space of 184 bytes from 0x10585b111 to 0x10585b1c7 
		#2 0x10585b1c8 [0x10585b1e8 -> 0x10585b268] 120 bytes
LARGE : 0x10587f000 
	Custom plage #1 with size 53248 @ 0x10587f000
	Custom plage #2 with size 4096 @ 0x10588c000
[Finished in 0.1s]
```
