'use strict'
const bindings = require('node-gyp-build')(__dirname)
bindings.verify = (passwd, hash, algo) => hash === bindings.encrypt(passwd, hash.split('$')[2], algo)

const ENUM_ALGO = {
  '1': 'md5',
  'md5': 'md5',

  '5': 'sha256',
  'sha256': 'sha256',

  '6': 'sha512',
  'sha512': 'sha512',
}

const SALT_SPACES = './0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'.split('')
function createSalt (length = 16) {
  let salt = ''
  const saltSpaceLength = SALT_SPACES.length
  while (length--) salt += SALT_SPACES[Math.floor(Math.random() * saltSpaceLength) % saltSpaceLength]
  return salt
}

module.exports = {
  encrypt (passwd, salt, algo) {
    if (!passwd) throw Error('Missing password argument.')
    algo = ENUM_ALGO[(algo || '6').toString().toLowerCase()]
    if (!algo) throw Error('Unknown algorithm.')
    salt = salt || createSalt(algo === 'md5' ? 8 : 16)

    switch (algo) {
      case 'md5': return bindings.encrypt(passwd, salt, 1)
      case 'sha256': return bindings.encrypt(passwd, salt, 5)
      case 'sha512': return bindings.encrypt(passwd, salt, 6)
    }
  },

  verify (passwd, hashed, algo) {
    if (!passwd) throw Error('Missing password argument.')
    if (!hashed) throw Error('Missing hash argument.')
    algo = ENUM_ALGO[(algo || hashed.split('$')[1] || '').toString().toLowerCase()]
    if (!algo) throw Error('Unknown algorithm.')

    switch (algo) {
      case 'md5': return bindings.verify(passwd, hashed, 1)
      case 'sha256': return bindings.verify(passwd, hashed, 5)
      case 'sha512': return bindings.verify(passwd, hashed, 6)
    }
  },

  createSalt,
}
