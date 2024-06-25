# c_containers

##### 这是基于C语言开发的一系列泛型容器集合，这些集合的基本实现思想完全基于内存分配只关心大小不关心类型这一理念。
##### 正常在C中实现泛型数据结构，不外乎 C11 的 _Generic，宏模拟，void\*以及 linux 内核中的侵入式链表，但是我在设计时，考虑将链表节点的大小加上存储元素的大小一起合并起来用malloc一次分配，即链表节点地址向后移动自身的长度，就是存储元素的地址，然后用户使用已经编写好的宏来拿到存储元素的地址，对其进行强制类型转换，最后赋值。
##### 所有容器都提供了对应的宏来获取容器结构体内部成员，同时全部都提供了一个元素删除回调函数，这是为了方便你在使用自己编写的数据类型时，可以直接将它们的删除函数挂载到容器中，让容器自己去帮你做删除工作。
##### 这一套机制仍然无法避免强制类型转换，但是可以容纳各种类型，也规避了传统的二次分配过程，比如链表节点分配出来后，你仍然需要为数据域额外进行一次malloc，分配2次是没有必要的。另外，性能方面，我只做了一些简单的测试，但是我发现这种实现方案与显式类型容器相比，差距小的不可思议。
##### 这个项目同时包含了一个代码生成器，有C和python 2个版本，这主要是为了有时候直接生成特定类型的容器，减小时间的浪费。(实际上代码生成器编写的初衷是在一次极其痛苦的宏容器debug过程后)

##### ==================================================================================================================================================================================================================

##### This is a series of generic container collections developed based on C language, and the basic implementation idea of these collections is completely based on the concept of memory allocation only caring about size and not type.
##### The normal implementation of generic data structures in C includes C11's _Generic, macro simulation, void\*, and intrusive linked lists in the Linux kernel. However, when designing, I considered combining the size of linked list nodes with the size of storage elements and using malloc to allocate them all at once. That is, the linked list node address is moved backwards by its own length, which is the address of the storage element. Then, the user uses the pre written macro to obtain the address of the storage element, performs a forced type conversion on it, and finally assigns a value.
##### All containers provide corresponding macros to obtain internal members of the container structure, and all provide an element deletion callback function. This is to facilitate you to directly mount their deletion functions to the container when using your own data types, allowing the container to help you with the deletion work.
##### This mechanism still cannot avoid forced type conversion, but it can accommodate various types and avoid the traditional secondary allocation process. For example, after a linked list node is allocated, you still need to perform an additional malloc for the data field, which is unnecessary to allocate twice. Additionally, in terms of performance, I only conducted some simple tests, but I found that this implementation solution has an incredible small gap compared to explicit type containers.
##### This project includes both C and Python versions of a code generator, mainly to generate specific types of containers directly and reduce time waste. (In fact, the original intention of writing a code generator was after an extremely painful macro container debugging process.)

