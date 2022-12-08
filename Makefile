all:
	emcc reader.c -O3 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=@reader_exports.json -s EXPORTED_RUNTIME_METHODS=cwrap -o reader.js
