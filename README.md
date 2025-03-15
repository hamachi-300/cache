# cache
cache memory simulation. computer architecture project

# content of mapping_simulation
- Directed Mapping Technique
- N Way Set Associative Technique
- Fully Set Associative Technique
- LRU (Least Recently Used) in set associative

# content of proxy_caching

# todo
- Memory Simulation: You can simulate larger memory spaces (e.g., simulate the impact of cache on a program running in memory, including multithreaded environments).

- Cache Hierarchies: If your project involves simulating multi-level cache hierarchies (L1, L2, L3 caches), modify the DirectMappedCache and SetAssociativeCache 
  classes to support multiple cache levels. You would need to consider the interactions between L1 and L2 caches.

- Parallelization: If simulating a multi-core processor or similar environment, consider how to parallelize the cache access 
  (e.g., simulating the impact of multiple threads accessing memory).

- Operating System Simulations: Cache simulation could be useful when implementing memory management or simulating the effect of caching on virtual memory.

- CPU Simulators: Integrating the cache simulation with a full CPU architecture simulator would allow you to model how the cache interacts with CPU 
  instructions and memory access patterns.

- Extend the program to handle Hybrid Cache Designs, such as Non-Uniform Cache Access (NUCA) or Content Addressable Memory (CAM), if needed.

- Implement Cache Prefetching mechanisms, where the cache tries to predict the next address based on the access pattern and preloads data accordingly.

- Multi-Level Cache Performance Analysis | Objective: Analyze the performance of L1, L2, and L3 caches in a processor.

- Cache Prefetching Techniques | Objective: Implement and compare different cache prefetching strategies.

- Cache Coherence in Multi-Core Processors | Objective: Simulate cache coherence protocols like MESI, MOESI, MSI. #

- GPU Cache Memory Optimization | Objective: Analyze and optimize shared memory and caching strategies in GPUs.

- Web Cache Proxy Simulator | Objective: Simulate a web cache proxy (like a browser cache) using a caching algorithm. #