{
	"targets": [{
		"target_name": "crypt3",
		"sources": [
			"crypt3.c",
		],
		"cflags": [
			"-std=c11",
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