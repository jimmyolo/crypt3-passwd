# CRYPT(3) for node.js

A nodejs [CRYPT(3)](http://man7.org/linux/man-pages/man3/crypt.3.html) N-API module for password encrypt and verify.

## Installation

```sh
$ npm install crypt3-passwd
```

## Usage
#### encrypt(password: String, salt: String, algo: String)
  - password (required)
  - salt (optional): if missing, will auto create random salt (length=16) without rounds part
  - algo (optional): 'md5'|'sha256'|'sha512'(default)
#### verify(password: String, hashed: String)

```js
const crypt3 = require('crypt3-passwd')

crypt3.verify(
  'jimmy',
  crypt3.encrypt('jimmy', 'salt', 'sha512')
) 

// user-supplied hashing rounds support as well
crypt3.verify(
  'jimmy',
  crypt3.encrypt('jimmy', 'rounds=1000$salt')
)
// rounds number should be between `1000` and `999999999`
// see: https://man7.org/linux/man-pages/man3/crypt.3.html for more details
```