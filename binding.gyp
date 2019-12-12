{
	"targets": [{
		"target_name": "crypt3",
		"sources": [
			"crypt3.c",
		],
		"include_dirs": [
      "<!(node -e \"require('napi-macros')\")"
    ],
		"cflags": [
			"-std=c99",
			"-O3",
			"-pedantic",
		],
		"conditions": [
			[ "OS == 'linux'", {
				"libraries": [
					"-lcrypt",
				],
			}],
		],
	}],
}