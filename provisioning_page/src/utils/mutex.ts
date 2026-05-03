type UnlockFn = () => void;

/**
 * Super minimalistic mutex implementation.
 */
export class Mutex {
  private tail: Promise<void>;

  constructor() {
    this.tail = Promise.resolve();
  }

  /**
   * Lock the mutex and return a function to unlock it.
   * 
   * The caller is responsible for calling the unlock function to release the mutex.
   * If the unlock function is not called, the mutex will be locked indefinitely.
   * @returns A promise that resolves to the unlock function.
   */
  lock(): Promise<UnlockFn> {
    let unlock: UnlockFn;
    const p = new Promise((resolve) => {
      unlock = resolve as UnlockFn;
    });
    const wait = this.tail.then(() => unlock);
    this.tail = this.tail.then(() => p as Promise<void>);

    return wait;
  }
}
