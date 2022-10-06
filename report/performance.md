
## Read and Write Time Comparison


Input filename|Size (MB)|Content
---|---|---
data/output/persons.json|897.95|Text,integers

Environment|Windows, Intel
---|---
Computer|Dell Mobile Precision 2015, Intel Xeon E3-1535M v5, 32GB memory, 1TB SSD
Operating system|Windows 2010
Compiler|Visual Studio 2019

Library|Version
---|---
[jsoncons](https://github.com/danielaparker/jsoncons)|0.168.7
[nlohmann](https://github.com/nlohmann/json)|3.11.2
[cJSON](https://github.com/DaveGamble/cJSON)|1.7.15
[json11](https://github.com/dropbox/json11)|master (2df9473)
[rapidjson](https://github.com/miloyip/rapidjson)|1.1.0
[jsoncpp](https://github.com/open-source-parsers/jsoncpp)|1.9.5
[json_spirit](https://github.com/png85/json_spirit)|4.0.8
[taojson](https://github.com/taocpp/json)|1.0.0-beta.13

Library|Time to read (s)|Time to write (s)|Memory footprint of json value (MB)|Remarks
---|---|---|---|---
