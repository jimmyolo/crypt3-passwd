'use strict'

const test = require('tap').test
const { encrypt, verify, createSalt } = require('../index.js')

/**
 * openssl >= 1.1.1
 *  $ openssl passwd -6 -salt salt/SHA512 jimmy
 *    > $6$salt/SHA512$dfLSBj9voHuXzHhCmaaZvQmaIZ9f7UsOJUsoq4t3mOqKgz4ls.vKb9k6QvNND2Vwb86R1PV3rKmwBHIg4HtQj/
 * 
 *  $ openssl passwd -5 -salt salt/SHA256 jimmy
 *    > $5$salt/SHA256$e29bUBnmpbTU4CDLX/ZQXmn0vqFa.ONpCQrDvPA07RD
 * 
 *  $ openssl passwd -1 -salt salt/MD5 jimmy
 *    > $1$salt/MD5$PXiUmTinagihoesHizaho1
 */
const data = {
  'sha512': {
    openssl: 'openssl passwd -6 -salt salt/SHA512 jimmy',
    salt: 'salt/SHA512',
    algo: 'sha512',
    passwd: 'jimmy',
    hashed: '$6$salt/SHA512$dfLSBj9voHuXzHhCmaaZvQmaIZ9f7UsOJUsoq4t3mOqKgz4ls.vKb9k6QvNND2Vwb86R1PV3rKmwBHIg4HtQj/'
  },
  'sha256': {
    openssl: 'openssl passwd -5 -salt salt/SHA256 jimmy',
    salt: 'salt/SHA256',
    algo: 'sha256',
    passwd: 'jimmy',
    hashed: '$5$salt/SHA256$e29bUBnmpbTU4CDLX/ZQXmn0vqFa.ONpCQrDvPA07RD'
  },
  'md5': {
    openssl: 'openssl passwd -1 -salt salt/MD5 jimmy',
    salt: 'salt/MD5',
    algo: 'md5',
    passwd: 'jimmy',
    hashed: '$1$salt/MD5$PXiUmTinagihoesHizaho1'
  }
}

test('Encryption', (batch) => {
  const password = 'jimmy'

  batch.test('sha512', (t) => {
    t.plan(1)
    const hashedPassword = encrypt(password, 'salt/SHA512', 'sha512')
    t.ok(hashedPassword === data['sha512'].hashed)
  })

  batch.test('sha256', (t) => {
    t.plan(1)
    const hashedPassword = encrypt(password, 'salt/SHA256', 'sha256')
    t.ok(hashedPassword === data['sha256'].hashed)
  })

  batch.test('md5', (t) => {
    t.plan(1)
    const hashedPassword = encrypt(password, 'salt/MD5', 'md5')
    t.ok(hashedPassword === data['md5'].hashed)
  })

  batch.test('default sha512', (t) => {
    t.plan(1)
    const hashedPassword = encrypt(password, 'salt/SHA512')
    t.ok(hashedPassword === data['sha512'].hashed)
  })

  batch.test('auto generate slat', (t) => {
    t.plan(3)
    t.ok(verify(password, encrypt(password, null, 'sha512')))
    t.ok(verify(password, encrypt(password, null, 'sha256')))
    t.ok(verify(password, encrypt(password, null, 'md5')))
  })

  batch.test('wrong arguments.', (t) => {
    t.plan(2)
    t.throws(() => encrypt(), Error('Missing password argument.'))
    t.throws(() => encrypt('password', 'salt', 'algo'), Error('Unknown algorithm.'))
  })

  batch.end()
})

test('Verify', (batch) => {
  const password = 'jimmy'

  batch.test('sha512', (t) => {
    t.plan(1)
    const isAuth = verify(password, data['sha512'].hashed)
    t.ok(isAuth === true)
  })

  batch.test('sha256', (t) => {
    t.plan(1)
    const isAuth = verify(password, data['sha256'].hashed)
    t.ok(isAuth === true)
  })

  batch.test('md5', (t) => {
    t.plan(1)
    const isAuth = verify(password, data['md5'].hashed)
    t.ok(isAuth === true)
  })

  batch.test('wrong arguments.', (t) => {
    t.plan(3)
    t.throws(() => verify(), Error('Missing password argument.'))
    t.throws(() => verify('password'), Error('Missing hash argument.'))
    t.throws(() => verify('password', 'unknown'), Error('Unknown algorithm.'))
  })

  batch.end()
})

test('genSalt', (batch) => {
  batch.test('random generate salt string', t => {
    let salt1 = createSalt()
    t.ok(salt1.length === 16)

    salt1 = createSalt(8)
    t.ok(salt1.length === 8)
    
    let salt2 = createSalt(8)
    t.ok(salt2.length === 8)
    t.ok(salt1 !== salt2)

    t.end()
  })

  batch.end()
})