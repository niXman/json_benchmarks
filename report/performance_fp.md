
## Read and Write Time Comparison


Input filename|Size (MB)|Content
---|---|---
data/output_fp/persons_fp.json|414.218|Text,doubles

Environment|Linux, AMD
---|---
Computer|ASUSTeK COMPUTER INC-FA507RE, AMD Ryzen 7 6800H with Radeon Graphics, 14 GB
Operating system|Linux Mint 21, x86_64
Compiler|g++-12.1.0

Library|Version
---|---
[jsoncons](https://github.com/danielaparker/jsoncons)|0.168.7
[flatjson](https://github.com/niXman/flatjson)|0.0.3
[cJSON](https://github.com/DaveGamble/cJSON)|1.7.15
[jsoncpp](https://github.com/open-source-parsers/jsoncpp)|1.9.5
[json11](https://github.com/dropbox/json11)|master (2df9473)

Library|Time to read s|Time to write s|Memory footprint on read MB|Memory footprint on write MB|Allocations on read|Allocations on write|Remarks
---|---|---|---|---|---|---|---
