{
	"targets": [
		{
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
					"defines": ["_GNU_SOURCE"]
				}],
				[ "OS == 'mac'", {
					"defines": ["_XOPEN_SOURCE"]
				}],
			],
		},
		{
			"target_name": "crypt3Async",
			"sources": [
				"crypt3Async.c",
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
					"defines": ["_GNU_SOURCE"]
				}],
				[ "OS == 'mac'", {
					"defines": ["_XOPEN_SOURCE"]
				}],
			],
		}
	],
}