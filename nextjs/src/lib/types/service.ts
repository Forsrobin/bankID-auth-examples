import z, { type ZodTypeAny } from 'zod'

/**
 * ServiceAsyncReturnType
 * This utility type extracts the return type of an asynchronous function.
 * It is useful for defining the expected return type of service methods that return promises.
 */
export type ServiceAsyncReturnType<T> = {
  [K in keyof T]: T[K] extends (...args: any) => Promise<infer R>
    ? R
    : T[K] extends object
    ? {
        [K2 in keyof T[K]]: T[K][K2] extends (...args: any) => Promise<infer R2> ? R2 : never
      }
    : never
}

export type InputTypesFromSchemas<T> = {
  [K in keyof T]: T[K] extends ZodTypeAny ? z.infer<T[K]> : T[K] extends object ? InputTypesFromSchemas<T[K]> : never
}
