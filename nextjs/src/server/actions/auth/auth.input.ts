import type { InputTypesFromSchemas } from '@/server/types'
import z from 'zod'

const initAuth = z.object({})

const pollAuth = z.object({
  orderRef: z.string(),
})

const getRefreshToken = z.object({
  userId: z.string(),
})

const deleteRefreshToken = z.object({
  userId: z.string(),
  refreshToken: z.string().min(1, 'Refresh token is required'),
})

const updateRefreshToken = z.object({
  userId: z.string(),
  refreshToken: z.string().min(1, 'Refresh token is required'),
})

const authInputs = {
  initAuth,
  pollAuth,
  getRefreshToken,
  updateRefreshToken,
  deleteRefreshToken,
}

export type AuthInputTypes = InputTypesFromSchemas<typeof authInputs>
export default authInputs
