export const env = {
  NODE_ENV: process.env.NODE_ENV as string,
  JWT_SECRET: process.env.JWT_SECRET as string,
}

export function validateEnvironment() {
  if (process.env.CHECK_ENVIRONMENT_VARIABLES === 'false') return

  const missingEnvironmentKeys = Object.entries(env)
    .map(([key, value]) => {
      if (value === undefined) return key
    })
    .filter((error) => error !== undefined)

  if (missingEnvironmentKeys.length > 0 && process.env.NODE_ENV !== 'test') {
    console.error(`You need to set environment variables ${missingEnvironmentKeys}`)
    throw new Error(`Missing environment variables: ${missingEnvironmentKeys.join(', ')}`)
  }
}

validateEnvironment()

export default env
