# CRYPT(3) for node.js

A nodejs [CRYPT(3)](http://man7.org/linux/man-pages/man3/crypt.3.html) N-API module for password encrypt and verify.

## Installation

```sh
$ npm install crypt3-passwd
```

## Usage
#### encrypt(password: String, salt: String, algo: String)
  - password (required)
  - salt (optional): if missing, will auto create random salt
  - algo (optional): 'md5'|'sha256'|'sha512'(default)
#### verify(password: String, hashed: String)

```js
const crypt3 = require('crypt3-passwd')

crypt3.encrypt('jimmy', 'salt', 'sha512') // '$6$salt$n6uyPG4ghvr5KGSCwnvMIoR7415LScAxExYuSwntPu3nzYunXfOyoxGjztZipEmt72wJaBwALWuV24MscmUBe1'
crypt3.verify('jimmy', '$6$salt$n6uyPG4ghvr5KGSCwnvMIoR7415LScAxExYuSwntPu3nzYunXfOyoxGjztZipEmt72wJaBwALWuV24MscmUBe1') // true
```