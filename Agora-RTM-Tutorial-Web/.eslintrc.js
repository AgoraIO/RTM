module.exports = {
	"env": {
		"browser": true,
		"es6": true,
		"node": true,
		"jquery": true,
	},
	"parser": "babel-eslint",
	"extends": "eslint:recommended",
	"globals": {
		"Atomics": "readonly",
		"SharedArrayBuffer": "readonly"
	},
	"parserOptions": {
		"ecmaVersion": 2018,
		"sourceType": "module"
	},
	"rules": {
		"indent": [
			"warn",
			"tab"
		],
		"quotes": [
			"warn",
			"double"
		],
		"semi": [
			"warn",
			"never"
		],
		"no-unused-vars": 0,
	}
}