import { expect, test } from 'vitest'
import { Mutex } from './mutex.js'

test('lock returns an unlock function and queues callers', async () => {
  const mutex = new Mutex()

  const unlockFirst = await mutex.lock()

  let secondResolved = false
  const secondLock = mutex.lock().then(unlock => {
    secondResolved = true
    return unlock
  })

  expect(typeof unlockFirst).toBe('function')
  expect(secondResolved).toBe(false)

  unlockFirst()

  const unlockSecond = await secondLock
  expect(secondResolved).toBe(true)
  expect(typeof unlockSecond).toBe('function')

  unlockSecond()
})

test('ensures concurrent tasks run sequentially', async () => {
  const mutex = new Mutex()
  const order: number[] = []

  const task = async (id: number) => {
    const release = await mutex.lock()
    try {
      order.push(id)
      await new Promise(resolve => setTimeout(resolve, 0))
    } finally {
      release()
    }
  }

  await Promise.all([1, 2, 3].map(task))
  expect(order).toEqual([1, 2, 3])
})