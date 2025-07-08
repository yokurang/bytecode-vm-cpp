Previously, we implemented a tree-walk interpreter. The model behind this interpreter is to parse the source code into an intermediary representation (AST), and then evaluate that intermediary representation into a runtime representation.

However, this process is slow. In this project, we will tackle the problem of interpreting from source code through a different model: the bytecode.

# Chunks of Bytecode

Before we get into bytecodes, let us first consider the trade-offs for AST.

The tree-walk interpreter is a simple interpreter where the runtime representation of the source code directly maps to the intermediary representation of the source code. 

However, using AST is not memory-efficient. We need a AST node for every piece of syntax. We end up allocating a lot of memory to pointers for a simple expression like `1 + 2` where each pointer holds a piece of syntax. Worse, the data is distributed across the heap in a loosely connected web of objects, which does bad things for spatial locality.

Modern CPUs process data way faster than they can pull it from the RAM. To compensate for that, chips have multiple layers of caching. If a piece of memory it needs is already in the cache, it can be loaded more quickly. We are talking upwards of 100 times faster.

How does data get into that cache? The machine speculatively stuffs things in there for you. Its heuristic is pretty simple. Whenever the CPU reads a bit of data from RAM, it pulls in a whole little bundle of adjacent bytes and stuffs them in the cache.

If the program next requests for some data close enough to be inside that cache line, the CPU runs like a well-oiled conveyer belt in a factory. We really want to take advantage of this. To use the cache effectively, the way we represent code in memory should be dense and ordered like it is a read.

Now look up at the tree. The sub-objects could be anywhere in the heap. Every step the tree-walker takes where it follows a reference to a child node could be outside bounds of the cache and force the CPU to stall until a new lump of data arrives from the RAM. The overhead of those tree nodes with all their pointer fields and object headers tend to push objects away from each other and out of the cache. The AST walker also has additional overhead around interface dispatch and the Visitor pattern, but the locality issue alone are enough to justify better code representation.

# Why not compile native code?

If you want to go fast, you want to get all the layers of indirection out of the way, which is machine code.

Compiling directly to a native instruction set the chip supports is what the fast languages do. Targetting native code has been the most efficient option since way back in the early days when engineers actually handwrote programs in machine code.

Native code is a dense series of operations, encoded directly in binary. Each instruction is between one and a few bytes long. The CPU cranks through the binary instructions, deociding and executing in a loop. There is no tree structure like in an AST. Control flow is handled by jumping from one point in the code directly to another. No indirection, no overhead, no unnecessary skipping around or chasing pointers.

Lightning fast, but the performance comes at a cost. First of all, compiling to native code is not easy. Most chips in wide use today have sprawling Bynzantine architectures with heaps of instructions that accreted over decades. They require sophisticated register allocation, pipelining, and instruction scheduling. Of course, you will also throw portability out the window.

# What is bytecode?

Fix those two points in your mind. On one end, a tree-walk interpreter is simple, portable, and slow. On the other hand, native code is complex and platform-specific but fast. Bytecode sits in the middle. It retains the portability of a tree-walker, and we do not need to get our hands dirty with assembly code. It sacrifices some simplicity for performance, though not as fast as going fully native.

Structurally, bytecode resembles machine code. It is a dense, linear sequence of binary instructions. That keeps overhead low and plays well with the cache. However, it is much simpler, higher-level instruction set than any real machine out there. In many bytecode, a single instruction is one byte long, hence the name bytecode.

Imagine you are writing a native compiler from some source language, and you are given carte blanche to define the easiest possible architecture to target. Bytecode is kind of like that. It is an idealised fantasy instruction set that makes your life as the compiler easier to write. In other words, it is a model for native binary instructions in a structure that is easier for the compiler to translate one language to.

The problem with a model like bytecode is that the architecture does not actually exist. We solve that by writing an emulator-a simulated chip that is written in software that interprets and executes the bytecode instructions one at a time. This is like a virtual machine. The emulation layer is additional overhead, making it slower than native code, but adds portability. 